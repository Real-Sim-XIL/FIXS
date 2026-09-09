#include <string>
#include <ctime>
#include <chrono>
#include <filesystem>
#include <system_error>
#include <cstdlib>
#include <cstring>

#include "TrafficHelper.h"
#include "DSProxyMode.h"

#include "RealSimVersion.h"

// Uncomment the line below to enable performance timing
// #define ENABLE_PERF_TIMING
#include "PerformanceTimer.h"

using namespace std;

// Global shutdown state - single point of access for cleanup coordination
struct ShutdownState {
    volatile bool shutdownRequested = false;
    bool initialized = false;

    // Pointers to resources that need cleanup (set after initialization)
    TrafficHelper* traffic = nullptr;
    SocketHelper* socket = nullptr;
    MsgHelper* msgClient = nullptr;
    vector<int>* clientSockets = nullptr;
    double* simTime = nullptr;

    // Configuration flags (set during initialization)
    bool enableVissim = false;
    bool enableClient = false;
    bool enableRealSim = false;

    // Track if traffic simulator connection was already closed
    bool trafficSimulatorClosed = false;

    // Reset all state
    void reset() {
        shutdownRequested = false;
        initialized = false;
        traffic = nullptr;
        socket = nullptr;
        msgClient = nullptr;
        clientSockets = nullptr;
        simTime = nullptr;
        enableVissim = false;
        enableClient = false;
        enableRealSim = false;
        trafficSimulatorClosed = false;
    }
};

// Single global instance
static ShutdownState g_shutdown;

namespace {
std::string GetExecutableDirectory() {
#ifdef WIN32
    std::vector<char> buffer(MAX_PATH);
    DWORD length = 0;
    while (true) {
        length = GetModuleFileNameA(nullptr, buffer.data(), static_cast<DWORD>(buffer.size()));
        if (length == 0) {
            return std::string();
        }
        if (length < buffer.size() - 1) {
            return std::filesystem::path(std::string(buffer.data(), length)).parent_path().string();
        }
        buffer.resize(buffer.size() * 2);
    }
#else
    std::vector<char> buffer(PATH_MAX);
    ssize_t length = readlink("/proc/self/exe", buffer.data(), buffer.size() - 1);
    if (length == -1) {
        return std::string();
    }
    buffer[length] = '\0';
    return std::filesystem::path(std::string(buffer.data())).parent_path().string();
#endif
}

void ConfigureSumoLibraryPath(const ConfigHelper& config) {
    static bool configured = false;
    if (configured) return;
    configured = true;

    std::filesystem::path exeDir(GetExecutableDirectory());
    std::vector<std::filesystem::path> candidates;

    // Override path first
    if (!config.SumoSetup.RuntimeLibraryPath.empty()) {
        std::filesystem::path override(config.SumoSetup.RuntimeLibraryPath);
        candidates.push_back(override.is_absolute() ? override : exeDir / override);
    }

    // Standard locations
    if (!exeDir.empty()) {
        candidates.push_back(exeDir / "CommonLib" / "libsumo" / "bin");
        std::filesystem::path parent = exeDir.parent_path();
        for (int i = 0; i < 5 && !parent.empty(); ++i, parent = parent.parent_path()) {
            candidates.push_back(parent / "CommonLib" / "libsumo" / "bin");
        }
    }
    candidates.push_back(std::filesystem::current_path() / "CommonLib" / "libsumo" / "bin");

    // Try each candidate
    for (const auto& path : candidates) {
        std::error_code ec;
        if (!std::filesystem::is_directory(path, ec)) continue;

        std::string pathStr = path.string();
#ifdef WIN32
        if (!SetDllDirectoryA(pathStr.c_str())) continue;
#else
        std::string newPath = pathStr;
        if (const char* current = std::getenv("LD_LIBRARY_PATH")) {
            newPath += ':' + std::string(current);
        }
        setenv("LD_LIBRARY_PATH", newPath.c_str(), 1);
#endif
        printf("Using SUMO library directory: %s\n", pathStr.c_str());
        return;
    }

    // Only error if SUMO is selected as traffic simulator
    if (config.SimulationSetup.SelectedTrafficSimulator != "VISSIM") {
        printf("ERROR: Unable to locate SUMO library directory.\n");
        printf("Please check SumoSetup.RuntimeLibraryPath in your configuration file.\n");
        exit(-1);
    }
}
}

//!!!!! NEED TO
// -multithread
// -add lane position
// -might need to put a timeout on recv or something. because the message can be larger than buffer size. 
// -limit number of vehicles to send
// -need to define if want to directly overwrite SUMO speed or send as speed guidance
// -handle vehicle enter the network and exiting the network
// -timing log
//
// !!! NEED TO RECV UNTIL EMPTY

const bool ENABLE_TIMING = false;

typedef struct Timer_t {
	LARGE_INTEGER StartingTime;
	LARGE_INTEGER EndingTime;
	LARGE_INTEGER ElapsedMicroseconds;
	LARGE_INTEGER Frequency;
};

ConfigHelper Config_c;
SocketHelper Sock_c;



// Perform graceful cleanup with proper shutdown sequence
void performCleanup(bool emergencyShutdown) {
	if (emergencyShutdown) {
		printf("\nEmergency shutdown initiated...\n");
	}
	else {
		printf("\nGraceful shutdown initiated...\n");
	}

	// Step 1: Notify all connected clients about shutdown (state=0)
	if (g_shutdown.initialized && g_shutdown.enableClient &&
		g_shutdown.socket && g_shutdown.msgClient &&
		g_shutdown.clientSockets && g_shutdown.simTime) {

		try {
			printf("Notifying clients of shutdown...\n");
			MsgHelper* msgClient = g_shutdown.msgClient;
			msgClient->clearSendStorage();

			float simTimeSend = *g_shutdown.simTime;
			uint8_t simStateSend = 0; // 0 = shutdown signal

			for (unsigned int iC = 0; iC < g_shutdown.clientSockets->size(); iC++) {
				try {
					g_shutdown.socket->sendData((*g_shutdown.clientSockets)[iC], iC,
						simTimeSend, simStateSend, *msgClient);
				}
				catch (...) {
					// Continue notifying other clients even if one fails
				}
			}
			printf("Client notification complete.\n");

			// Give clients time to receive and process shutdown notification
			Sleep(100);
		}
		catch (const std::exception& e) {
			printf("Warning: Error notifying clients: %s\n", e.what());
		}
		catch (...) {
			printf("Warning: Unknown error notifying clients.\n");
		}
	}

	// Step 2: Close SUMO/VISSIM connection gracefully
	if (g_shutdown.initialized && g_shutdown.traffic && g_shutdown.enableRealSim && !g_shutdown.trafficSimulatorClosed) {
		try {
			printf("Closing traffic simulator connection...\n");
			g_shutdown.traffic->close();
			printf("Traffic simulator connection closed.\n");
		}
		catch (const std::exception& e) {
			printf("Warning: Error closing traffic simulator: %s\n", e.what());
		}
		catch (...) {
			printf("Warning: Unknown error closing traffic simulator.\n");
		}
	}
	else if (g_shutdown.trafficSimulatorClosed) {
		printf("Traffic simulator already closed by user.\n");
	}

	// Step 3: Close all sockets
	if (g_shutdown.socket) {
		try {
			printf("Closing sockets...\n");
			g_shutdown.socket->socketShutdown();
			printf("Sockets closed.\n");
		}
		catch (const std::exception& e) {
			printf("Warning: Error closing sockets: %s\n", e.what());
		}
		catch (...) {
			printf("Warning: Unknown error closing sockets.\n");
		}
	}

	printf("Shutdown complete.\n");
}

BOOL WINAPI CtrlHandler(DWORD fdwCtrlType)
{
	switch (fdwCtrlType)
	{
		// Handle the CTRL-C signal.
	case CTRL_C_EVENT:
		printf("Ctrl-C event caught\n\n");
		g_shutdown.shutdownRequested = true;
		return TRUE;

	// Handle Ctrl-Break signal
	case CTRL_BREAK_EVENT:
		printf("Ctrl-Break event caught\n\n");
		g_shutdown.shutdownRequested = true;
		return TRUE;

	// Handle console window close (X button)
	case CTRL_CLOSE_EVENT:
		printf("Console close event caught\n\n");
		performCleanup(true); // Emergency cleanup (Windows gives ~5 seconds)
		return TRUE;

	// Handle user logoff
	case CTRL_LOGOFF_EVENT:
		printf("User logoff event caught\n\n");
		performCleanup(true); // Emergency cleanup
		return TRUE;

	// Handle system shutdown
	case CTRL_SHUTDOWN_EVENT:
		printf("System shutdown event caught\n\n");
		performCleanup(true); // Emergency cleanup
		return TRUE;

	default:
		return FALSE;
	}
}

static void show_usage(std::string name)
{
	std::cerr << "Usage: " << name << endl
		<< "Options:\n"
		<< "\t-h,--help\t\tShow this help message\n"
		<< "\t-f,--file PATH\\FILENAME\tSpecify the path and filename of configuration yaml file"
		<< std::endl;
}

#ifdef WIN32
// Find all YAML files in a directory recursively (Windows implementation)
void findYamlFilesRecursive(const std::string& directory, std::vector<std::string>& yamlFiles) {
	WIN32_FIND_DATAA findData;
	std::string searchPath = directory + "\\*";
	HANDLE hFind = FindFirstFileA(searchPath.c_str(), &findData);

	if (hFind == INVALID_HANDLE_VALUE) {
		return;
	}

	do {
		std::string fileName = findData.cFileName;
		if (fileName == "." || fileName == "..") continue;

		std::string fullPath = directory + "\\" + fileName;

		if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			// Recursively search subdirectories
			findYamlFilesRecursive(fullPath, yamlFiles);
		}
		else {
			// Check if file has .yaml or .yml extension
			size_t dotPos = fileName.find_last_of('.');
			if (dotPos != std::string::npos) {
				std::string ext = fileName.substr(dotPos);
				if (ext == ".yaml" || ext == ".yml") {
					yamlFiles.push_back(fullPath);
				}
			}
		}
	} while (FindNextFileA(hFind, &findData));

	FindClose(hFind);
}
#else
// Find all YAML files in a directory recursively (POSIX implementation)
#include <dirent.h>
#include <sys/stat.h>
void findYamlFilesRecursive(const std::string& directory, std::vector<std::string>& yamlFiles) {
	DIR* dir = opendir(directory.c_str());
	if (!dir) return;

	struct dirent* entry;
	while ((entry = readdir(dir)) != nullptr) {
		std::string fileName = entry->d_name;
		if (fileName == "." || fileName == "..") continue;

		std::string fullPath = directory + "/" + fileName;

		struct stat statbuf;
		if (stat(fullPath.c_str(), &statbuf) == 0) {
			if (S_ISDIR(statbuf.st_mode)) {
				// Recursively search subdirectories
				findYamlFilesRecursive(fullPath, yamlFiles);
			}
			else if (S_ISREG(statbuf.st_mode)) {
				// Check if file has .yaml or .yml extension
				size_t dotPos = fileName.find_last_of('.');
				if (dotPos != std::string::npos) {
					std::string ext = fileName.substr(dotPos);
					if (ext == ".yaml" || ext == ".yml") {
						yamlFiles.push_back(fullPath);
					}
				}
			}
		}
	}
	closedir(dir);
}
#endif

std::vector<std::string> findYamlFiles(const std::string& directory) {
	std::vector<std::string> yamlFiles;
	findYamlFilesRecursive(directory, yamlFiles);
	return yamlFiles;
}

int main(int argc, char* argv[]) {

	printf("==================================================\n");
	printf("\t\tRealSim Interface\n");
	printf("\t\t    v%s\n",REALSIM_VERSION_STRING);
#ifdef _DEBUG
	printf("\t\t    (DEBUG build)\n");
#else
	printf("\t\t    (RELEASE build)\n");
#endif
	printf("==================================================\n");

	// control-c handles
	if (SetConsoleCtrlHandler(CtrlHandler, TRUE))
	{
		//printf("\nThe control-c handler is set.\n");
	}
	else
	{
		printf("\nERROR: Could not set control-c handler");
		exit(-1);
	}


	string TrafficLayerErrorFile = "TrafficLayer.err";

	auto simStartTimestamp = chrono::system_clock::to_time_t(chrono::system_clock::now());
	char simStartTimestampChar[100];
	ctime_s(simStartTimestampChar, sizeof simStartTimestampChar, &simStartTimestamp);
	fstream f(TrafficLayerErrorFile, std::fstream::in | std::fstream::out | std::fstream::app);
	f << endl << "=============================================" << endl;
	f << "Traffic Layer Starts at  " << simStartTimestampChar << endl;
	f.close();

	
	string LogPath = ".\\RealSim_tmp";
	if (CreateDirectoryA(LogPath.c_str(), NULL) ||
		ERROR_ALREADY_EXISTS == GetLastError())
	{
		// CopyFile(...)
	}
	else
	{
		// Failed to create directory.
		printf("cannot create RealSim_tmp folder, exiting...");
		return -1;
	}

	char MasterLogNameChar[100];

	time_t rawtime;
	struct tm* timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);

	char formatTimeBuffer[100];
	strftime(formatTimeBuffer, 100, "%Y%m%d_%H%M%S", timeinfo);
	snprintf(MasterLogNameChar, sizeof(char) * 100, ".\\RealSim_tmp\\TrafficLayer_%s.log", formatTimeBuffer);
	DWORD dwAttrib = GetFileAttributesA(MasterLogNameChar);

	bool logNameExist = (dwAttrib != INVALID_FILE_ATTRIBUTES &&
		!(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));

	int iFile = 1;
	while (logNameExist) {
		snprintf(MasterLogNameChar, sizeof(char) * 100, ".\\RealSim_tmp\\TrafficLayer_%s_%d.log", formatTimeBuffer,iFile);
		DWORD dwAttrib = GetFileAttributesA(MasterLogNameChar);
		iFile++;
		logNameExist = (dwAttrib != INVALID_FILE_ATTRIBUTES &&
			!(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));	
	}
	
	string MasterLogName(MasterLogNameChar);

	// reset logs
	if (1) {
		ifstream f(MasterLogName.c_str());
		if (f.good()) {
			f.close();
			remove(MasterLogName.c_str());
		}
	}

	string configPath;
	bool configSpecified = false;

	// Parse command-line arguments
	for (int i = 1; i < argc; i++) {
		string arg = argv[i];
		if (arg == "-h" || arg == "--help") {
			show_usage(argv[0]);
			return 0;
		}
		else if (arg == "-f" || arg == "--file") {
			if (i + 1 < argc) {
				configPath = argv[++i];
				configSpecified = true;
			}
			else {
				std::cerr << "--path option requires one argument." << std::endl;
				return -1;
			}
		}
		else {
			printf("Check options\n");
			show_usage(argv[0]);
			return 0;
		}
	}

	// Auto-discover config if not specified
	if (!configSpecified) {
		// First, check for TrafficLayer/.active_config file
		std::ifstream activeConfigFile("TrafficLayer/.active_config");
		if (activeConfigFile.good()) {
			std::getline(activeConfigFile, configPath);
			activeConfigFile.close();
			printf("Using config from TrafficLayer/.active_config: %s\n", configPath.c_str());
		}
		else {
			// Auto-discover in tests/UserScenarios/
			std::vector<std::string> yamlFiles = findYamlFiles("tests/UserScenarios");

			if (yamlFiles.empty()) {
				printf("ERROR: No configuration specified and no YAML found in tests/UserScenarios/\n");
				printf("Options:\n");
				printf("  - Use: -f path/to/config.yaml\n");
				printf("  - Add scenario to tests/UserScenarios/\n");
				printf("  - Create TrafficLayer/.active_config with path to your config\n");
				return -1;
			}
			else if (yamlFiles.size() == 1) {
				configPath = yamlFiles[0];
				printf("Auto-discovered config: %s\n", configPath.c_str());
			}
			else {
				printf("ERROR: Multiple YAML files found in tests/UserScenarios/\n");
				printf("Please create TrafficLayer/.active_config file with one of these paths:\n");
				for (const auto& yaml : yamlFiles) {
					printf("  - %s\n", yaml.c_str());
				}
				printf("\nExample: echo tests/UserScenarios/issue_85/config.yaml > TrafficLayer/.active_config\n");
				return -1;
			}
		}
	}

	// ===========================================================================
	// 			READ Config File
	// ===========================================================================
	printf("Reading Configuration file %s\n", configPath.c_str());
	if (Config_c.getConfig(configPath) < 0) {
		printf("Please check path and filename of the configuration yaml\n");
		show_usage(argv[0]);
		exit(-1);
	}
	else {
		printf("Read configuration file success\n");
	}

	{
		FILE* f = fopen(MasterLogName.c_str(), "a");
		fprintf(f, "\nConfigFile %s\n", configPath.c_str());
		fclose(f);
	}

	// Stage A dispatch (issue #158): if VissimSetup.EnableDSProxy is true,
	// TrafficLayer drives VISSIM via PTV's DrivingSimulatorProxy.dll instead
	// of the legacy DriverModel socket path below (where the FIXS DriverModel
	// DLL runs inside VISSIM and TrafficLayer talks to it via TCP). The
	// DSProxy code path is fully self-contained; the rest of mainTrafficLayer
	// is bypassed and we return its exit code. The bypass is intentional for
	// 0.9.0 to avoid risking SUMO/legacy regressions; absorption into a
	// unified loop is tracked by issue #117 (XIL orchestrator).
	if (Config_c.VissimSetup.EnableDSProxy) {
		return FIXS::DSProxy::runDSProxyMode(Config_c);
	}

	// initialize Traffic Layer setup variables
	bool ENABLE_REALSIM = true;
	bool ENABLE_VISSIM = false;
	bool ENABLE_CLIENT = false;
	bool ENABLE_VERBOSE = false;
	bool ENABLE_EXT_DYN = false;
	// Potentially, the 'ENABLE_VEH_SIMULATOR' and 'ENABLE_CARLA' flags can be merged into a single flag
	// ENABLE_VEH_SIMULATOR: CarMaker or other vehicle simulator
	// ENABLE_CARLA: CARLA simulator
	bool ENABLE_VEH_SIMULATOR = false;
	bool ENABLE_CARLA = false;
	bool ENABLE_CARLA_EXTERNAL_CONTROL = false;
	vector <int> selfServerPortAll = {};
	vector <string> serverAddr = {};
	vector <int> serverPort = {};
	vector <string> serverNames = {};

	// each application, map port to socket
	unordered_map <int, int> selfServerPort2Sock_um;
	vector <int> selfServerPortUserInput = {};

	vector <int> vissimSock;
	// Index-stable: actualClientSock[iC] pairs with selfServerPortUserInput[iC]
	// for every declared client, whether or not it is connected yet. activeIdx
	// lists the ones currently being served, ascending -- which is also the
	// sequential-overlay order a later client depends on.
	vector <int> actualClientSock;
	vector <int> activeIdx;

	// config Traffic Layer setup variables
	if (Config_c.SimulationSetup.EnableRealSim) {
		ENABLE_REALSIM = true;
	}
	else {
		ENABLE_REALSIM = false;
	}
	if (Config_c.SimulationSetup.SelectedTrafficSimulator.compare("VISSIM") == 0) {
		ENABLE_VISSIM = true;
		printf("Traffic Simulator: VISSIM\n");
	}
	else {
		ENABLE_VISSIM = false;
		printf("Traffic Simulator: SUMO\n");
		// Configure SUMO library path for runtime DLL loading
		ConfigureSumoLibraryPath(Config_c);
	}
	if (Config_c.SimulationSetup.EnableVerboseLog) {
		ENABLE_VERBOSE = true;
		printf("Verbose logging: Enabled\n");
	}
	else {
		ENABLE_VERBOSE = false;
	}
	if (Config_c.ApplicationSetup.EnableApplicationLayer) {
		ENABLE_CLIENT = true;
	}
	else {
		if (Config_c.XilSetup.EnableXil) {
			ENABLE_CLIENT = true;
		}
		else {
			ENABLE_CLIENT = false;
		}
	}

	if (Config_c.SimulationSetup.EnableExternalDynamics) {
		ENABLE_EXT_DYN = true;
	}
	else {
		ENABLE_EXT_DYN = false;
	}

	ENABLE_VEH_SIMULATOR = Config_c.CarMakerSetup.EnableCosimulation;
	ENABLE_CARLA = Config_c.CarlaSetup.EnableCosimulation;
	ENABLE_CARLA_EXTERNAL_CONTROL = (Config_c.EgoSetup.Dynamics == "virenv");
	if (ENABLE_VERBOSE) {
		//FILE* f = fopen(MasterLogName.c_str(), "a");
		//fprintf(f, "\n============================================");
		//fprintf(f, "Traffic Layer Starts at %s\n", simStartTimestampChar);
		//fclose(f);
	}

	if (!Config_c.ApplicationSetup.EnableApplicationLayer && Config_c.XilSetup.EnableXil){
		if (Config_c.XilSetup.VehicleSubscription.size() > 0) {
			serverAddr.push_back(Config_c.SimulationSetup.TrafficSimulatorIP);
			serverPort.push_back(Config_c.SimulationSetup.TrafficSimulatorPort);
			if (ENABLE_VISSIM) {
				serverNames.push_back("vissimDriver");
			}
		}
		if (Config_c.XilSetup.DetectorSubscription.size() > 0) {
			serverAddr.push_back(Config_c.SimulationSetup.TrafficSimulatorIP);
			serverPort.push_back(Config_c.SimulationSetup.TrafficSimulatorPort+1);
			if (ENABLE_VISSIM) {
				serverNames.push_back("vissimSignal");
			}
		}
	}
	else {
		if (Config_c.ApplicationSetup.VehicleSubscription.size() > 0) {
			serverAddr.push_back(Config_c.SimulationSetup.TrafficSimulatorIP);
			serverPort.push_back(Config_c.SimulationSetup.TrafficSimulatorPort);
			if (ENABLE_VISSIM) {
				serverNames.push_back("vissimDriver");
			}
		}
		if (Config_c.ApplicationSetup.DetectorSubscription.size() > 0) {
			serverAddr.push_back(Config_c.SimulationSetup.TrafficSimulatorIP);
			serverPort.push_back(Config_c.SimulationSetup.TrafficSimulatorPort+1);
			if (ENABLE_VISSIM) {
				serverNames.push_back("vissimSignal");
			}
		}
	}


	// ===========================================================================
	// 			Check Configuration
	// ===========================================================================
	if (serverAddr.size() == 0 || serverPort.size() == 0) {
		printf("ERROR: must define at least a VehicleSubscription or DetectorSubscription!\n\n");
		exit(-1);
	}

	if (ENABLE_CLIENT) {
		if (!Config_c.ApplicationSetup.EnableApplicationLayer && Config_c.XilSetup.EnableXil) {

			for (int i = 0; i < Config_c.XilSetup.VehicleSubscription.size(); i++) {
				vector <int> port_v;
				port_v = get<3>(Config_c.XilSetup.VehicleSubscription[i]);
				for (int iP = 0; iP < port_v.size(); iP++) {
					selfServerPortAll.push_back(port_v[iP]);
				}
			}
			for (int i = 0; i < Config_c.XilSetup.SignalSubscription.size(); i++) {
				vector <int> port_v;
				port_v = get<3>(Config_c.XilSetup.SignalSubscription[i]);
				for (int iP = 0; iP < port_v.size(); iP++) {
					selfServerPortAll.push_back(port_v[iP]);
				}
			}
			for (int i = 0; i < Config_c.XilSetup.DetectorSubscription.size(); i++) {
				vector <int> port_v;
				port_v = get<3>(Config_c.XilSetup.DetectorSubscription[i]);
				for (int iP = 0; iP < port_v.size(); iP++) {
					selfServerPortAll.push_back(port_v[iP]);
				}
			}
		}
		else {
			for (int i = 0; i < Config_c.ApplicationSetup.VehicleSubscription.size(); i++) {
				vector <int> port_v;
				port_v = get<3>(Config_c.ApplicationSetup.VehicleSubscription[i]);
				for (int iP = 0; iP < port_v.size(); iP++) {
					selfServerPortAll.push_back(port_v[iP]);
				}
			}
			for (int i = 0; i < Config_c.ApplicationSetup.SignalSubscription.size(); i++) {
				vector <int> port_v;
				port_v = get<3>(Config_c.ApplicationSetup.SignalSubscription[i]);
				for (int iP = 0; iP < port_v.size(); iP++) {
					selfServerPortAll.push_back(port_v[iP]);
				}
			}
			for (int i = 0; i < Config_c.ApplicationSetup.DetectorSubscription.size(); i++) {
				vector <int> port_v;
				port_v = get<3>(Config_c.ApplicationSetup.DetectorSubscription[i]);
				for (int iP = 0; iP < port_v.size(); iP++) {
					selfServerPortAll.push_back(port_v[iP]);
				}
			}
		}

	}

	// number of ports equal to size of Sock_c.clientSock
	// map each port to an element of Sock_c.clientSock
	selfServerPortUserInput = selfServerPortAll;
	sort(selfServerPortUserInput.begin(), selfServerPortUserInput.end());
	auto it = unique(selfServerPortUserInput.begin(), selfServerPortUserInput.end());
	selfServerPortUserInput.resize(distance(selfServerPortUserInput.begin(), it));
	for (int i = 0; i < selfServerPortUserInput.size(); i++) {
		int port = selfServerPortUserInput[i];
		// if not exist
		if (selfServerPort2Sock_um.find(port) == selfServerPort2Sock_um.end()) {
			selfServerPort2Sock_um[port] = selfServerPort2Sock_um.size();
		}
	}


	for (int i = 0; i < selfServerPortUserInput.size(); i++) {
		if (selfServerPortUserInput[i] == Config_c.SimulationSetup.TrafficSimulatorPort || selfServerPortUserInput[i] == Config_c.SimulationSetup.TrafficSimulatorPort+1) {
			printf("ERROR: %d and %d are reserved ports, please select other ports for Application Layer!\n", Config_c.SimulationSetup.TrafficSimulatorPort, Config_c.SimulationSetup.TrafficSimulatorPort+1);
			exit(-1);
		}
	}

	// Print client ports summary
	if (selfServerPortUserInput.size() > 0) {
		printf("Client ports: ");
		for (int i = 0; i < selfServerPortUserInput.size(); i++) {
			if (i > 0) printf(", ");
			printf("%d", selfServerPortUserInput[i]);
		}
		printf("\n");
	}

	double simTime = 0;
	int ii = 0;

	/********************************************
	* Timing Analysis
	*********************************************/
	LARGE_INTEGER StartingTime, EndingTime, ElapsedMicroseconds;
	LARGE_INTEGER Frequency;

	QueryPerformanceFrequency(&Frequency);

	/********************************************
	* Connection Setups
	*********************************************/
	//if (selfServerPortUserInput.size() > 1 && !ENABLE_VEH_SIMULATOR) {
	//	printf("\nERROR: currently only support one application layer\n");
	//	exit(-1);
	//}

	TrafficHelper Traffic_c;
	if (ENABLE_VISSIM) {
		//// has client
		//if (ENABLE_CLIENT && selfServerPortUserInput.size() > 0) {
		//	//Sock_c.socketSetup(serverAddr, serverPort, selfServerPortUserInput);
		//	Sock_c.socketSetup(selfServerPortUserInput);
		//}
		//else {
		//	Sock_c.socketSetup(serverAddr, serverPort);
		//}
		//if (!ENABLE_CLIENT) {
		//	Sock_c.disableClient();
		//}
		int N_ACT_CLIENT = selfServerPortUserInput.size();

		vector <int> vissimSockIdx;
		for (int i = 0; i < serverAddr.size(); i++) {
			vissimSockIdx.push_back(selfServerPortUserInput.size());
			if (ENABLE_REALSIM) {
				selfServerPortUserInput.push_back(serverPort[i]);
			}
		}
		Sock_c.socketSetup(selfServerPortUserInput);

		Sock_c.N_ACT_CLIENT = N_ACT_CLIENT;

		Sock_c.disableWaitClientTrigger();
		Sock_c.disableServerTrigger();
		if (Sock_c.initConnection(TrafficLayerErrorFile) < 0) {
			printf("Connect to VISSIM failed! Make sure start one instance of VISSIM/SUMO \n");
			exit(-1);
		}

		if (vissimSockIdx.size() == 0) {
			printf("\nERROR: must define at least a VehicleSubscription or DetectorSubscription!\n\n");
			exit(-1);
		}
		for (int i = 0; i < vissimSockIdx[0]; i++) {
			actualClientSock.push_back(Sock_c.clientSock[i]);
		}
		for (int i = 0; i < vissimSockIdx.size(); i++) {
			if (ENABLE_REALSIM) {
				vissimSock.push_back(Sock_c.clientSock[vissimSockIdx[i]]);
			}
		}
	}
	else {

		if (ENABLE_VEH_SIMULATOR) {
			bool needAddPort = true;
			for (int curPort : selfServerPortUserInput) {
				if (curPort == Config_c.CarMakerSetup.CarMakerPort) {
					needAddPort = false;
					break;
				}
			}
			if (needAddPort) {
				selfServerPortUserInput.push_back(Config_c.CarMakerSetup.CarMakerPort);
			}
		}

		Sock_c.socketSetup(selfServerPortUserInput); // no Server 
		if (!ENABLE_CLIENT) {
			Sock_c.disableClient();
		}

		Sock_c.N_ACT_CLIENT = selfServerPortUserInput.size();

		//Sock_c.enableWaitClientTrigger();
		Sock_c.disableWaitClientTrigger();

		if (!ENABLE_VEH_SIMULATOR) {
			// #86: with a warm-up configured, bind and listen now but do not block
			// waiting for the clients -- the main loop accepts them when the
			// warm-up ends. Their connect() still succeeds immediately (the socket
			// is listening), so nothing on the client side changes; TrafficLayer
			// just stops waiting for them before it can start warming up.
			Sock_c.DeferAcceptClients = Config_c.SimulationSetup.WarmUpUntilEgoEntry ||
				Config_c.SimulationSetup.WarmUpTime > 0;

			if (Sock_c.initConnection(TrafficLayerErrorFile) > 0) {
				printf("Connect to SUMO failed! Make sure start Traffic Simulator first and start one instance of VISSIM/SUMO \n");
				exit(-1);
			}

			actualClientSock.assign(Sock_c.clientSock.size(), 0);
			if (!Sock_c.DeferAcceptClients) {
				for (int i = 0; i < (int)Sock_c.clientSock.size(); i++) {
					actualClientSock[i] = Sock_c.clientSock[i];
					activeIdx.push_back(i);
				}
			}
			else if (!Config_c.SimulationSetup.WarmUpServePorts.empty()) {
				// These clients have to OBSERVE the warm-up (see WarmUpServePorts):
				// accept them now, blocking as TrafficLayer did before warm-ups
				// existed, and serve them from the first step. Everyone else is
				// accepted when the warm-up ends.
				vector<int> servedIdx;
				for (int i = 0; i < (int)selfServerPortUserInput.size() &&
				                i < (int)Sock_c.clientSock.size(); i++) {
					for (int p : Config_c.SimulationSetup.WarmUpServePorts) {
						if (selfServerPortUserInput[i] == p) { servedIdx.push_back(i); break; }
					}
				}
				if (!servedIdx.empty()) {
					if (Sock_c.acceptClients(TrafficLayerErrorFile, &servedIdx) < 0) {
						printf("Accepting the warm-up clients failed\n");
						exit(-1);
					}
					for (int i : servedIdx) {
						actualClientSock[i] = Sock_c.clientSock[i];
						activeIdx.push_back(i);
						printf("Serving port %d through the warm-up.\n", selfServerPortUserInput[i]);
					}
				}
			}
		}

	}


	// pass configuration to Traffic_c
	Traffic_c.Config_c = &Config_c;
	Traffic_c.getConfig();

	/********************************************
	* Traffic Simulator send and recv setups
	*********************************************/
	try {
		if (ENABLE_VISSIM) {
			Traffic_c.connectionSetup(Sock_c.NCLIENT);
			Traffic_c.selectVISSIM(vissimSock, serverNames);
		}
		else {
			string trafficIp = Config_c.SimulationSetup.TrafficSimulatorIP;
			int trafficPort = Config_c.SimulationSetup.TrafficSimulatorPort;


			Traffic_c.connectionSetup(trafficIp, trafficPort, Sock_c.NCLIENT, Config_c.SumoSetup.ExecutionOrder);
			Traffic_c.selectSUMO();
		}
	}
	catch (const std::exception& e) {
		printf("Error: connect to traffic simulator failed\n");
		std::cout << e.what();
		exit(-1);
	}
	catch (...) {
		printf("Error: connect to traffic simulator failed\n");
		exit(-1);
	}

	Traffic_c.MasterLogName = MasterLogName;
	Sock_c.MasterLogName = MasterLogName;

	Traffic_c.ENABLE_VEH_SIMULATOR = ENABLE_VEH_SIMULATOR;
	Traffic_c.ENABLE_CARLA = ENABLE_CARLA;
	Traffic_c.ENABLE_CARLA_EXTERNAL_CONTROL = ENABLE_CARLA_EXTERNAL_CONTROL;
	/********************************************
	* Message Setups
	*********************************************/
	MsgHelper MsgServer_c;
	MsgHelper MsgClient_c;

	MsgServer_c.getConfig(Config_c);
	MsgClient_c.getConfig(Config_c);

	Timer_t TimerDelay = { 0 };
	QueryPerformanceFrequency(&TimerDelay.Frequency);

	///********************************************
	//* Start connection
	//*********************************************/
	//Sock_c.initConnection();

	// Initialize shutdown state for graceful cleanup
	g_shutdown.traffic = &Traffic_c;
	g_shutdown.socket = &Sock_c;
	g_shutdown.msgClient = &MsgClient_c;
	g_shutdown.clientSockets = &actualClientSock;
	g_shutdown.simTime = &simTime;
	g_shutdown.enableVissim = ENABLE_VISSIM;
	g_shutdown.enableClient = ENABLE_CLIENT;
	g_shutdown.enableRealSim = ENABLE_REALSIM;
	g_shutdown.initialized = true;

	PERF_INIT("TrafficLayerPerf.log");

	bool isVeryFirstStep = true;

	// Warm-up state (#86). One flag for both triggers: while it is false the FIXS
	// boundary is closed -- the loop returns to the top before any client I/O, and
	// the clients have not even been accepted yet.
	const bool WARMUP_CONFIGURED = Config_c.SimulationSetup.WarmUpUntilEgoEntry ||
		Config_c.SimulationSetup.WarmUpTime > 0;
	bool warmUpFinished = !WARMUP_CONFIGURED;

	// Clients are accepted once, after the warm-up. Already true here when
	// initConnection() accepted them before the loop (no warm-up, no CarMaker).
	bool clientsAccepted = !ENABLE_VEH_SIMULATOR && !WARMUP_CONFIGURED;

	//while (simTime <= tSimuEnd && ii < nT) {
	while (!g_shutdown.shutdownRequested) {

		PERF_TIC("main_loop");

		///****************************************************
		// RUN one-step simulation
		///****************************************************

		PERF_TIC("traffic_step");
		if (ENABLE_REALSIM && !g_shutdown.trafficSimulatorClosed) {
			try {
				Traffic_c.runOneStepSimulation();
			}
			catch (const std::exception& e) {
				printf("ERROR: Traffic simulator step failed: %s\n", e.what());
				printf("\tTraffic simulator may have been closed\n");
				printf("\tInitiating graceful shutdown...\n");
				g_shutdown.trafficSimulatorClosed = true;
				g_shutdown.shutdownRequested = true;
				PERF_TOC("traffic_step");
				PERF_TOC("main_loop");
				break;
			}
			catch (...) {
				printf("ERROR: Traffic simulator step failed (unknown error)\n");
				printf("\tTraffic simulator may have been closed\n");
				printf("\tInitiating graceful shutdown...\n");
				g_shutdown.trafficSimulatorClosed = true;
				g_shutdown.shutdownRequested = true;
				PERF_TOC("traffic_step");
				PERF_TOC("main_loop");
				break;
			}
		}
		PERF_TOC("traffic_step");
#ifdef ENABLE_PERF_TIMING
		// Log number of vehicles received from traffic simulator
		PERF_LOG("t=%.2f vehicles_in_network=%d\n", simTime, (int)MsgServer_c.VehDataRecv_um.size());
#endif

		if (ENABLE_VERBOSE) {
			printf("\n===========New time step==============\n");
			printf("===========SimTime %f==============\n", simTime);

			FILE* f = fopen(MasterLogName.c_str(), "a");
			//fprintf(f, "\n===========New time step==============\n");
			fprintf(f, "\n===SimTime %f\n", simTime);
			fclose(f);
		}
		// print every 10 simulation seconds
		else if (abs(simTime / 10 - round(simTime / 10)) < 1e-5) {
			printf("===========SimTime %f==============\n", simTime);
		}

		// ===================================================================
		// 			Warm-up gate (#86)
		// ===================================================================
		// While the warm-up runs the FIXS boundary is CLOSED: this block returns
		// to the top of the loop before every client send/recv below, and no
		// client has been accepted yet, so CARLA's map load and CarMaker's start-up
		// overlap the warm-up instead of queueing ahead of it.
		//
		// The two triggers are exclusive (ConfigHelper enforces it) because they
		// are different loops: WarmUpTime is a single batch step that observes
		// nothing on the way, WarmUpUntilEgoEntry has to look between steps.
		if (!warmUpFinished) {
			if (Config_c.SimulationSetup.WarmUpTime > 0) {
				printf("Warm-up: advancing the traffic simulator to t=%.2f with the FIXS boundary closed...\n",
					Config_c.SimulationSetup.WarmUpTime);
				Traffic_c.runSimulation(Config_c.SimulationSetup.WarmUpTime);
				Traffic_c.getSimulationTime(&simTime);
				printf("Warm-up complete at t=%.2f\n", simTime);
				warmUpFinished = true;
			}
			else if (Traffic_c.isWarmUpEgoInNetwork(&simTime)) {
				warmUpFinished = true;
			}
			else if (activeIdx.empty()) {
				// Tick again; nothing downstream sees this step.
				continue;
			}
			// else: some clients must OBSERVE the warm-up (WarmUpServePorts), so
			// fall through and run the ordinary exchange for them. What they see
			// is a feed identical to a run with no warm-up at all -- which is the
			// point: a controller that learns the road by watching it cannot be
			// handed the road already in motion and be expected to agree with a
			// run that watched it from the start.
		}

		// ===================================================================
		// 			Accept the clients (once, after the warm-up)
		// ===================================================================
		// Without a warm-up this is a no-op: initConnection() already accepted
		// before the loop (or, on the CarMaker path, does so here on the first
		// step exactly as it always has). With one, it waits until the warm-up is
		// actually over -- otherwise the clients that were meant to sit it out
		// would be pulled in on its first step.
		if (!clientsAccepted && warmUpFinished) {
			clientsAccepted = true;

			if (ENABLE_VEH_SIMULATOR) {
				if (Sock_c.initConnection(TrafficLayerErrorFile) > 0) {
					printf("Connect to SUMO failed! Make sure start Traffic Simulator first and start one instance of VISSIM/SUMO \n");
					exit(-1);
				}
				// Index-stable, and ascending: actualClientSock[iC] must keep pairing
				// with selfServerPortUserInput[iC], and the sequential overlay depends
				// on the order. Anyone served through the warm-up is already in here.
				actualClientSock.assign(Sock_c.clientSock.size(), 0);
				activeIdx.clear();
				for (int i = 0; i < (int)Sock_c.clientSock.size(); i++) {
					actualClientSock[i] = Sock_c.clientSock[i];
					activeIdx.push_back(i);
				}
			}
			else if (WARMUP_CONFIGURED) {
				// Bound and listening since before the loop; only the blocking
				// accept was held back until now -- and only for the clients that
				// sat the warm-up out. Asking for the ones already served would
				// block in select() on a listening socket no one is going to
				// connect to again, which hangs the handover at ego entry.
				vector<int> pending;
				for (int i = 0; i < (int)Sock_c.clientSock.size(); i++) {
					if (find(activeIdx.begin(), activeIdx.end(), i) == activeIdx.end())
						pending.push_back(i);
				}
				if (!pending.empty() &&
				    Sock_c.acceptClients(TrafficLayerErrorFile, &pending) < 0) {
					printf("Accepting clients after the warm-up failed\n");
					exit(-1);
				}
				// Index-stable, and ascending: actualClientSock[iC] must keep pairing
				// with selfServerPortUserInput[iC], and the sequential overlay depends
				// on the order. Anyone served through the warm-up is already in here.
				actualClientSock.assign(Sock_c.clientSock.size(), 0);
				activeIdx.clear();
				for (int i = 0; i < (int)Sock_c.clientSock.size(); i++) {
					actualClientSock[i] = Sock_c.clientSock[i];
					activeIdx.push_back(i);
				}
			}
		}

		// if veryFirstStep and vehicle simulator, add the ego vehicle
		if (isVeryFirstStep && ENABLE_VEH_SIMULATOR) {
			Traffic_c.addEgoVehicle(simTime);
		}

		///****************************************************
		// VISSIM/SUMO =====>>>>> Traffic Layer
		// get Traffic Simulator simulation INFO
		///****************************************************
		try {
			MsgServer_c.clearRecvStorage();

			if (ENABLE_REALSIM && !g_shutdown.trafficSimulatorClosed) {
				if (Traffic_c.recvFromTrafficSimulator(&simTime, MsgServer_c) < 0) {
					if (WSAGetLastError() != WSAEINTR) {

						printf("WARNING: receive from traffic simulator fails\n");

						printf("\tVISSIM or SUMO may already be closed\n");
						printf("\tInitiating graceful shutdown...\n");
					}
					g_shutdown.trafficSimulatorClosed = true; // Mark that simulator is already closed
					g_shutdown.shutdownRequested = true;
					break;
				};

				// end simulation if simulation time is greater than simulation end time setup
				if (simTime > Config_c.SimulationSetup.SimulationEndTime) {
					printf("Simulation end time reached.\n");
					g_shutdown.shutdownRequested = true;
					break;
				}
			}

			if (ENABLE_VERBOSE) {
				for (auto& it : MsgServer_c.VehDataRecv_um) {
					MsgServer_c.printVehData(it.second);

					MsgServer_c.printVehDataToFile(MasterLogName, it.second);
				}
			}
		}
		catch (const std::exception& e) {
			printf("ERROR: Exception in traffic simulator receive: %s\n", e.what());
			g_shutdown.shutdownRequested = true;
			break;
		}
		catch (...) {
			printf("UNKNOWN ERROR: receive from traffic simulator fails\n");
			g_shutdown.shutdownRequested = true;
			break;
		}

		///****************************************************
		// Traffic Layer =====>>>>> Clients (Controller, Vehicle Simulator, Models)
		// send traffic to clients, e.g. Simulink/dSPACE
		///****************************************************
		if (ENABLE_TIMING) {

			QueryPerformanceCounter(&EndingTime);
			ElapsedMicroseconds.QuadPart = EndingTime.QuadPart - StartingTime.QuadPart;
			ElapsedMicroseconds.QuadPart *= 1000000;
			ElapsedMicroseconds.QuadPart /= Frequency.QuadPart;

			fstream debugLog;
			debugLog.open("timing_sumo.txt", fstream::in | fstream::out | fstream::app);
			debugLog << simTime << ";" << ElapsedMicroseconds.QuadPart / 1000000.0 << endl;
			debugLog.close();
		}

		if (MsgServer_c.VehDataRecv_um.size() > 0) {
			int aa = 1;
		}

		if (ENABLE_CLIENT) {
			PERF_TIC("prepare_send");
			try {
				MsgClient_c.clearSendStorage();
				MsgClient_c.clearRecvStorage();   // #174 sequential: B (client-return bucket) starts empty this tick

				for (int iC : activeIdx) {
				//for (unsigned int iC = actualClientSock.size()-1; iC > 0; iC--) {
					//!!!! NEED TO DISTRIBUTE MsgServer_c data to MsgClient_c
					//!!!! MsgServer_c VehDataRecv_um TlsDataRecvAll_v DetDataRecvAll_v ==>>> MsgClient_c VehDataSend_v[iclient]  VehIdSend_v[iclient] DetDataSend_v[iclient] TlsDataSend_v[iclient]

					//MsgClient_c.VehDataSend_um[Sock_c.clientSock[iC]] = MsgServer_c.VehDataRecvAll_v;

					int curPort = selfServerPortUserInput[iC];

					MsgClient_c.VehDataSend_um[actualClientSock[iC]] = {};
					MsgClient_c.TlsDataSend_um[actualClientSock[iC]] = {};
					MsgClient_c.DetDataSend_um[actualClientSock[iC]] = {};
					// #174 sequential clients: publish the SUMO snapshot A OVERLAID with
					// returns already gathered this tick from lower-port clients
					// (B = MsgClient_c.VehDataRecv_um), B winning per id -- so a later
					// client (e.g. VirCarlaEnv) sees an earlier client's (e.g. a speed-
					// advisory controller) ego record. B is empty for the first
					// (lowest-port) client, so single-client flows stay byte-identical.
					unordered_map<string, VehFullData_t> pubSrc = MsgServer_c.VehDataRecv_um;
					for (auto& bIt : MsgClient_c.VehDataRecv_um) pubSrc[bIt.first] = bIt.second;
					for (auto it : pubSrc) {
						if (ENABLE_VEH_SIMULATOR) {
							// if id is ego, and is the first socket (XIL), send it 
							if (it.second.id.compare(Config_c.EgoSetup.Id) == 0 && selfServerPortUserInput[iC] != Config_c.CarMakerSetup.CarMakerPort && Config_c.CarMakerSetup.EnableEgoSimulink) {
								MsgClient_c.VehDataSend_um[actualClientSock[iC]].push_back(it.second);
							}
							// if is the CarMakerPort or only have one socket (both vehicle simualtor and XIL), send it
							else if (selfServerPortUserInput[iC] == Config_c.CarMakerSetup.CarMakerPort || actualClientSock.size() == 1) {
								MsgClient_c.VehDataSend_um[actualClientSock[iC]].push_back(it.second);
							}
						}
						else {
							MsgClient_c.VehDataSend_um[actualClientSock[iC]].push_back(it.second);
						}
					}
					for (auto it : MsgServer_c.TlsDataRecv_um) {
						// assign Tls data to each client
						// if all signal, then add it to send list
						if (Config_c.SocketPort2SubscriptionList_um[curPort].SignalList.subAllSignalFlag) {
							MsgClient_c.TlsDataSend_um[actualClientSock[iC]].push_back(it.second);
						}
						// else, only assign Tls that is needed for current port/socket
						else {
							// check if current signal light is included
							if (Config_c.SocketPort2SubscriptionList_um[curPort].SignalList.signalId_v.find(it.first) != Config_c.SocketPort2SubscriptionList_um[curPort].SignalList.signalId_v.end()) {
								MsgClient_c.TlsDataSend_um[actualClientSock[iC]].push_back(it.second);
							}
						}
						
					}
					for (auto it : MsgServer_c.DetDataRecv_um) {
						MsgClient_c.DetDataSend_um[actualClientSock[iC]].push_back(it.second);
					}

					if (ENABLE_VERBOSE) {
						printf("sending client at port %d\n", selfServerPortUserInput[iC]);
						printf("\tvehicle data size %d\n", (int)MsgClient_c.VehDataSend_um[actualClientSock[iC]].size());
						printf("\tsignal light data size %d\n", (int)MsgClient_c.TlsDataSend_um[actualClientSock[iC]].size());
						printf("\tdetector data size %d\n", (int)MsgClient_c.DetDataSend_um[actualClientSock[iC]].size());

						FILE* f = fopen(MasterLogName.c_str(), "a");
						//fprintf(f, "send complete\n");
						fprintf(f, "send client: %d\n", selfServerPortUserInput[iC]);
						if ((int)MsgClient_c.VehDataSend_um[actualClientSock[iC]].size() > 0) {
							fprintf(f, "\tveh: %d\n", (int)MsgClient_c.VehDataSend_um[actualClientSock[iC]].size());
						}
						if ((int)MsgClient_c.TlsDataSend_um[actualClientSock[iC]].size()) {
							fprintf(f, "\ttls: %d\n", (int)MsgClient_c.TlsDataSend_um[actualClientSock[iC]].size());
						}
						if ((int)MsgClient_c.DetDataSend_um[actualClientSock[iC]].size()) {
							fprintf(f, "\tdet: %d\n", (int)MsgClient_c.DetDataSend_um[actualClientSock[iC]].size());
						}
						fclose(f);

					}

					float simTimeSend = simTime;
					uint8_t simStateSend = 1;

					PERF_TOC("prepare_send");
					PERF_TIC("send_data");
#ifdef ENABLE_PERF_TIMING
					// Check send buffer status before send
					int sndBufSize = 0;
					int optLen = sizeof(sndBufSize);
					getsockopt(actualClientSock[iC], SOL_SOCKET, SO_SNDBUF, (char*)&sndBufSize, &optLen);

					// Calculate message size
					int msgSize = MsgClient_c.VehDataSend_um[actualClientSock[iC]].size();
					PERF_LOG("t=%.2f client=%d/%d sndBufSize=%d nVeh=%d nTls=%d nDet=%d\n",
						simTime, iC, (int)actualClientSock.size(), sndBufSize, msgSize,
						(int)MsgClient_c.TlsDataSend_um[actualClientSock[iC]].size(),
						(int)MsgClient_c.DetDataSend_um[actualClientSock[iC]].size());
#endif

					if (ENABLE_EXT_DYN) {
						if (isVeryFirstStep) {
							if (Sock_c.sendData(actualClientSock[iC], iC, simTimeSend, simStateSend, MsgClient_c) < 0) {
								printf("ERROR: send to client fails\n");
								g_shutdown.shutdownRequested = true;
								break;
							};
						}
						else {
							if (Sock_c.sendData(actualClientSock[iC], iC, simTimeSend, simStateSend, MsgClient_c) < 0) {
								printf("ERROR: send to client fails\n");
								g_shutdown.shutdownRequested = true;
								break;
							};
						}
					}
					else if (ENABLE_VEH_SIMULATOR) {
						//if (isVeryFirstStep && selfServerPortUserInput[iC] == Config_c.CarMakerSetup.CarMakerPort && Config_c.CarMakerSetup.EnableEgoSimulink) {
						//	continue;
						//}
						//else {
							if (Sock_c.sendData(actualClientSock[iC], iC, simTimeSend, simStateSend, MsgClient_c) < 0) {
								printf("ERROR: send to client fails\n");
								g_shutdown.shutdownRequested = true;
								break;
							};
						//}
					}
					else {
						if (Sock_c.sendData(actualClientSock[iC], iC, simTimeSend, simStateSend, MsgClient_c) < 0) {
							printf("ERROR: send to client fails\n");
							g_shutdown.shutdownRequested = true;
							break;
						};
					}

					PERF_TOC("send_data");

					if (ENABLE_VERBOSE) {
						printf("send complete\n");

						//FILE* f = fopen(MasterLogName.c_str(), "a");
						//fprintf(f, "send complete\n");
						//fclose(f);
					}

					// ---- #174 sequential: RECV this client's return, merge into B ----
					// (last-writer-wins per id; feeds the NEXT client's overlaid publish).
					int simStateRecvC; float simTimeRecvC;
					if (Sock_c.recvData(actualClientSock[iC], &simStateRecvC, &simTimeRecvC, MsgClient_c) < 0) {
						if (WSAGetLastError() != WSAEINTR && WSAGetLastError() != WSAEFAULT)
							printf("ERROR: receive from client fails\n");
						g_shutdown.shutdownRequested = true;
						break;
					}
					// drop client-returned ids SUMO doesn't know, EXCEPT Carla-owned ids
					// (injected via the send path, so absent from SUMO until first inject).
					if (!ENABLE_VEH_SIMULATOR) {
						vector<string> idToRemove;
						for (auto& rit : MsgClient_c.VehDataRecv_um) {
							if (MsgServer_c.VehDataRecv_um.find(rit.first) == MsgServer_c.VehDataRecv_um.end()) {
								if (ENABLE_CARLA && ENABLE_CARLA_EXTERNAL_CONTROL &&
									find(Config_c.CarlaSetup.InterestedIds.begin(), Config_c.CarlaSetup.InterestedIds.end(), rit.first) != Config_c.CarlaSetup.InterestedIds.end())
									continue;
								idToRemove.push_back(rit.first);
							}
						}
						for (auto& r : idToRemove) MsgClient_c.VehDataRecv_um.erase(r);
					}

				}
			}
			catch (const std::exception& e) {
				printf("ERROR: Exception in send to client: %s\n", e.what());
				g_shutdown.shutdownRequested = true;
				break;
			}
			catch (...) {
				printf("UNKNOWN ERROR: send to client fails\n");
				g_shutdown.shutdownRequested = true;
				break;
			}
		}

		// Check if shutdown was requested during client send
		if (g_shutdown.shutdownRequested) {
			break;
		}

		// #174 sequential clients: client RECV now happens INLINE in the publish loop
		// above (each client's publish overlays A with B-so-far, then recv+merge), so a
		// lower-port client's return feeds a higher-port client's publish the same tick.
		// The legacy broadcast-recv block that used to live here was removed.

		// Check if shutdown was requested during client recv
		if (g_shutdown.shutdownRequested) {
			break;
		}

		if (ENABLE_TIMING) {
			QueryPerformanceCounter(&StartingTime);
		}

		///****************************************************
		// VISSIM/SUMO <<<<<===== Traffic Layer
		// SEND to Traffic Simulator
		///****************************************************
		try {
			MsgServer_c.clearSendStorage();
			// this function distribute combined message to different server/clients
			if (ENABLE_CLIENT) {
				Traffic_c.parseSendMsg(MsgClient_c, MsgServer_c); //MsgClient_c recv storage => MsgServer_c send storage
			}
			else {
				Traffic_c.parseSendMsg(MsgServer_c, MsgServer_c);
			}

			if (ENABLE_REALSIM && !g_shutdown.trafficSimulatorClosed) {
				if (Traffic_c.sendToTrafficSimulator(simTime, MsgServer_c) < 0) {
					printf("ERROR: send to traffic simulator fails\n");
					g_shutdown.trafficSimulatorClosed = true;
					g_shutdown.shutdownRequested = true;
					break;
				};
			}


			if (ENABLE_VERBOSE) {
				for (auto& it : MsgServer_c.VehDataSend_um) {
					for (int i = 0; i < it.second.size(); i++) {
						MsgServer_c.printVehData(it.second[i]);

						MsgServer_c.printVehDataToFile(MasterLogName, it.second[i]);

					}
				}
			}
		}
		catch (const std::exception& e) {
			printf("ERROR: Exception in send to traffic simulator: %s\n", e.what());
			g_shutdown.shutdownRequested = true;
			break;
		}
		catch (...) {
			printf("UNKNOWN ERROR: send to traffic simulator fails\n");
			g_shutdown.shutdownRequested = true;
			break;
		}

		///****************************************************
		// go to next step
		///****************************************************
		if (isVeryFirstStep) {
			isVeryFirstStep = false;
		}

		ii = ii + 1;

		PERF_TOC("main_loop");

	}

	PERF_SHUTDOWN();

	// Perform graceful cleanup after main loop exits
	performCleanup(false);

	return 0;
}
