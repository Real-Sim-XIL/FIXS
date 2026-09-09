#pragma once

#include "yaml-cpp/yaml.h"
//#ifndef WINDOWS_INCLUDED
//#define WINDOWS_INCLUDED
//#define _WINSOCKAPI_
//#include <windows.h>
//#endif
//#include <tchar.h>
#include <unordered_map>
#include <unordered_set>
#include <fstream>

#include <iostream>
#include <string>
#include <exception>
#include <stdexcept>

//#include <fileapi.h>

//template <typename T> std::unordered_map<std::string, std::vector<T> > subAttMap;

// [ego][id]
// multiple ego/point/link -----> {attribute: list of value}, one for each client/port/connection

typedef typename std::unordered_map<std::string, std::vector<std::string> > SubAttMap_t;

// type, attribute, ip, port
// string, SubAttMap_t, vector <string>, vector <int>
typedef typename std::vector <std::tuple < std::string, SubAttMap_t, std::vector <std::string>, std::vector <int>> > Subscription_t;


struct SimulationSetup_t {
	bool EnableRealSim;

	bool EnableVerboseLog;

	// FIXS DriverModel "ego only" mode (issue #158 Stage B+).
	//   true  (default, legacy):  DriverModel does per-vehicle send/recv
	//                             inside the MOVE_DRIVER callback when the
	//                             vehicle is in the subscription list,
	//                             and skips per-tick I/O entirely when no
	//                             subscribed vehicle is present.
	//   false (CAV controller):   DriverModel does ONE send/recv per tick
	//                             at DRIVER_DATA_TIME, regardless of
	//                             subscription state. Required for the
	//                             TrafficLayer DSProxy + DriverModel relay
	//                             path where TL sends behavior cmds for
	//                             arbitrary CAV vehicles per tick.
	bool SubEgoOnly;

	double SimulationEndTime;

	// NEED to fix later
	// flag to set SUMO speed through setPreviousSpeed
	bool EnableExternalDynamics;

	std::vector<std::string> VehicleMessageField;

	std::string SelectedTrafficSimulator;

	std::string TrafficSimulatorIP;

	int TrafficSimulatorPort;

	// Warm-up (#86). While the warm-up is running the FIXS boundary is CLOSED:
	// the traffic simulator advances, but no message reaches any client and no
	// client is even accepted yet, so CarMaker/Carla start-up overlaps the
	// warm-up instead of queueing ahead of it.
	//
	//   WarmUpUntilEgoEntry - end the warm-up when the first subscribed ego is in
	//                         the network. The controller owns the entry time by
	//                         inserting the vehicle; no time is duplicated here.
	//   WarmUpTime          - end the warm-up at this ABSOLUTE simulation time.
	//                         Runs as ONE batch step, so nothing is observed in
	//                         between - which is why the two are exclusive.
	//
	// Both unset: no warm-up (sync from the first step). Both set: ego entry wins
	// and getConfig warns, because a batch step cannot also watch for an ego.
	bool WarmUpUntilEgoEntry;

	double WarmUpTime;

	// Client ports that must be SERVED THROUGH the warm-up instead of joining
	// when it ends. Empty (the default) = the original behaviour: the boundary is
	// closed to everyone.
	//
	// This has to be declared because it cannot be derived. Whether a client can
	// skip the warm-up is a property of ITS OWN state, and nothing FIXS can see
	// distinguishes the two kinds: a renderer and a signal-aware controller are
	// both just a subscription with a port. A controller that learns the signal
	// timing by watching it change (any actuated network -- SUMO's NEMA logics
	// expose no phase countdown at all) arrives blind if the boundary was closed
	// to it, and then plans the first approach on a nominal guess. A renderer or
	// an XIL box loses nothing by joining late, which is the whole point of the
	// warm-up. Only the scenario author knows which is which.
	//
	// A served client is accepted BEFORE the warm-up starts (so TrafficLayer
	// blocks for it, as it did before warm-ups existed); everyone else is
	// accepted when the warm-up ends.
	std::vector<int> WarmUpServePorts;

	std::string TrafficLayerIP;

	int TrafficLayerPort;
};

struct ApplicationSetup_t {
	bool EnableApplicationLayer;

	int NumberOfApplications;

	std::vector <int> ApplicationPort;


	Subscription_t VehicleSubscription;

	Subscription_t DetectorSubscription;

	Subscription_t SignalSubscription;

};

struct XilSetup_t {
	bool EnableXil;

	bool AsServer;

	Subscription_t VehicleSubscription;

	Subscription_t SignalSubscription;

	Subscription_t DetectorSubscription;

};


// The ego, described once, for every backend (ORNL-Real-Sim/FIXS#305).
//
// CarMaker, an XIL plant and Carla are the same situation from the traffic
// simulator's side: a vehicle SUMO holds but does not drive. This is the one
// place that says which vehicle that is and who drives it. The per-backend keys
// it replaces are still PARSED as fallbacks (see the EgoSetup section in
// ConfigHelper.cpp) but no longer exist as fields.
struct EgoSetup_t {

	std::string Id;         // FIXS id of the ego
	std::string Type;       // vehicle type the TRAFFIC simulator knows the ego by,
	                        // used when it has to be injected. Not the virtual
	                        // environment's model -- that is CarlaSetup.EgoBlueprint.

	// WHAT COMPUTES THE EGO'S MOTION.
	//   traffic : the traffic simulator does
	//   virenv  : the virtual environment's physics does (Carla PhysX today);
	//             named for the role, not the backend
	//   xil     : an external plant does
	// "carla" is a deprecated alias for "virenv".
	std::string Dynamics;

	// WHO PRODUCES THE PEDALS AND STEER.
	//   simulator : the driver the environment brought (Carla TM, IPGDriver)
	//   fixs      : the driver FIXS ships (EgoDriver)
	//   user      : yours -- name a Controller below and it runs in-process once
	//               per environment step; leave it unset and the pedals come off
	//               the FIXS record at the 0.1 s feed, which is NOT equivalent
	//               (see FIXS#305: the feed carries the advisory in `speed`).
	// Deprecated aliases: carlaTM|internal|external|embedded.
	std::string ActuationSource;

	std::string Controller;   // user control law (.py); Python backend only

};


struct CarMakerSetup_t {
	bool EnableCosimulation;

	bool EnableEgoSimulink;

	std::string CarMakerIP;

	int CarMakerPort;

	double TrafficRefreshRate;

	bool SynchronizeTrafficSignal;

	int TrafficSignalPort;

};

struct CarlaSetup_t {
	bool EnableVerboseLog;

	bool EnableCosimulation;

	// Co-sim bridge selector, consumed by FIXS_Applications' run_cosim.py (NOT by
	// this engine): true -> the standalone run_synchronization.py bridge;
	// false -> TrafficLayer + this VirCarlaEnv. A bool, not a "py"/"cpp" string,
	// so a typo cannot silently select the wrong engine. Mirrored in
	// ConfigHelper.py; parsed here for schema parity. Default true.
	bool EnablePythonBackend;

	bool EnableExternalControl;

	bool UseVehicleTypeAsBlueprint;

	std::string CarlaServerIP;

	int CarlaServerPort;

	std::string CarlaClientIP;

	int CarlaClientPort;

	std::string CarlaMapName;

	std::string CenteredViewId;

	// Spectator (main-viewport) BEV follow of CenteredViewId. Rigid top-down snap
	// each tick (no low-pass -> no camera oscillation). Off -> the bridge never
	// touches the spectator, so you can free-fly the CARLA camera to inspect.
	bool   EnableSpectatorFollow;   // master on/off
	double SpectatorHeight;         // BEV camera height above the ego (m)
	bool   SpectatorAlignYaw;       // true: rotate view with ego heading; false: fixed north-up

	// Pace the bridge loop to real time (sleep out the remainder of each tick).
	// FALSE for XIL -- the RT component (CarMaker/dSPACE) already paces the loop
	// and an extra sleep would throttle it. TRUE for a STANDALONE viz demo (no RT
	// hardware) so a follow-cam renders smooth real-time motion instead of a
	// several-x fast-forward. This is what the native run_synchronization does.
	bool   RealtimePacing;

	double TrafficRefreshRate;

	// Carla render/tick sub-step. The FIXS feed is 0.1 s; ticking Carla finer
	// (e.g. 0.05 s) and interpolating the feed gives smoother motion -- the same
	// trick the CarMaker side uses. Default 0 -> the bridge uses TrafficRefreshRate
	// (1:1, no interpolation). Must evenly divide the 0.1 s feed (0.05/0.025/0.02).
	double CarlaTimeStep;

	std::vector<std::string> InterestedIds;

	// #174 ego driving-mode ladder (integer -- modular, GUI-mappable):
	//   0 = SumoDriver  : SUMO drives the ego; Carla teleports it (default, today)
	//   1 = CarlaDriver : L0 -- Carla TM drives the ego (physics ON + autopilot);
	//                     its state is read back and injected into SUMO each feed
	//   2 = Advisory    : L2 -- as 1, plus external desired-speed advisory
	//                     through FIXS (TM keeps steering)          [reserved]
	//   3 = Control     : L4 -- external throttle/brake/steer through FIXS
	//                     (full PhysX dynamics, external steers)    [reserved]
	int EgoMode;
	// Which L0 driver actuates the ego when EgoMode >= 1:
	//   "TM"      -> native Carla Traffic Manager autopilot (needs a routable map)
	//   "Pursuit" -> the SDK-free EgoDriver module (map-agnostic fallback)
	std::string EgoL0Driver;

	// #305 THE CONFIG SURFACE. EgoMode and EgoL0Driver above stay as the internal
	// representation; these two keys are what a scenario writes, and when present
	// they derive EgoMode, EgoL0Driver AND EnableExternalControl together -- so the
	// three can no longer contradict one another, which is exactly what they did.
	//
	// They answer two genuinely independent questions:
	//
	//   EgoDynamics -- WHAT COMPUTES THE EGO'S MOTION
	//     "traffic" : the traffic simulator does; Carla teleports the ego in
	//     "carla"   : Carla PhysX does
	//     "xil"     : an XIL plant does; Carla teleports the ego in  [not implemented]
	//
	//   EgoActuationSource -- WHO PRODUCES THE PEDALS/STEER IT RUNS ON
	//     "carlaTM"  : Carla's Traffic Manager, in-process
	//     "internal" : the built-in EgoDriver module (pure pursuit on EgoRoutePoints)
	//     "external" : taken off the ego's FIXS record, i.e. from a controller client
	//     "embedded" : a user controller named by EgoController, called in-process
	//                  once per Carla step (Python backend only; see FIXS#325)
	//
	// "external" and "embedded" are the same controller in two places, and the
	// difference is not stylistic. An external client is served at the 0.1 s feed,
	// so it sees the ego record as the traffic simulator left it -- including
	// `speed`, which under L2 carries the eco advisory rather than the measured
	// speed. A controller closing a speed loop there reads back its own setpoint,
	// its error is ~0 by construction, and it never corrects. Embedded, the record
	// is refreshed from the backend before every call, so the loop closes on the
	// plant.
	//
	// L0 vs L2 is deliberately NOT a value here. Both are "carla" plus a driver;
	// they differ only in whether a controller is wired in upstream on a lower
	// port. That is topology, and the config has no business claiming to know it.
	std::string EgoBlueprint;          // Carla blueprint for the ego actor
	std::vector<double> EgoSpawnPose;  // [x, y, z, headingDeg] FIXS frame (mode >= 1)
	int TrafficManagerPort;            // Carla TM port (client-side instance)

	// Ego route for the Carla TM (the CarMaker-Route analog): [[x,y], ...] FIXS
	// frame waypoints injected via TM SetCustomPath. REQUIRED in practice for
	// generated OpenDRIVE worlds -- TM default lane-following drives straight off
	// road ends there. EgoRouteRepeat: laps of the list to queue (loop scenarios).
	// #174 NOTE: EgoRoutePoints / EgoRouteRepeat / EgoTargetSpeed are DRIVER behavior,
	// not simulation wiring. They live in the sim config only because the in-Carla
	// drivers (native TM, Pursuit fallback) run inside VirCarlaEnv and read them here.
	// The unified EgoDriver client (EgoL0Driver: Actuation) already owns these on the
	// application side (like the XIL side's RealSimPara.speedInit / RealSimInterpSpeed).
	// LEGACY: retire the in-Carla drivers -> these move to the driver and leave the sim
	// config (and this struct) entirely.
	std::vector<std::pair<double, double>> EgoRoutePoints;
	int EgoRouteRepeat;
	double EgoTargetSpeed;             // driver cruise speed (m/s); also the L2 fallback
	                                   // target until an external advisory arrives

};

struct SumoSetup_t {

	int SpeedMode;
	int ExecutionOrder;

	// How far ahead (metres) to look for a preceding vehicle when filling the
	// precedingVehicle* fields -- i.e. the car-following horizon the application
	// sees. Too short and a controller is blind to a slower vehicle further ahead;
	// too long and it may react to one that turns off before it matters. Default
	// 1000 preserves the previously hard-coded behaviour.
	double PrecedingVehicleLookahead;

	// keepRoute bitmask for the moveToXY that mirrors an externally-driven ego,
	// for EVERY owner now -- the CarMaker call site used to hardcode 6 (#305).
	// A choice of failure mode: bit 0 pins the ego to its own route and makes
	// SUMO raise; bit 1 places it exactly and lets it leave the network silently.
	// https://sumo.dlr.de/docs/TraCI/Change_Vehicle_State.html#move_to_xy
	int EgoKeepRoute;

	// Auto-launch SUMO configuration
	bool EnableAutoLaunch;
	std::string SumoConfigFile;
	int NumClients;
	std::string RuntimeLibraryPath;
};

// VISSIM DrivingSimulatorProxy.dll coupling (issue #158, Stage A).
// When Enable: true, TrafficLayer drives VISSIM via the DSProxy DLL instead
// of the COM path. See doc/156_drivingsim_dll_design_proposal.md.
struct VissimSetup_t {
	// Enable the VISSIM DrivingSimulatorProxy.dll code path (TrafficLayer
	// drives VISSIM via DSProxy instead of the legacy COM path). Sibling
	// flags like EnableDriverModelRelay (Stage B+) live alongside, since a
	// single VISSIM run can have DSProxy on AND a DriverModel attached.
	bool EnableDSProxy;

	std::string NetworkFile;        // .inpx path passed to VISSIM_Connect
	int    VissimVersion;           // 2022 | 2026 (selects versionNo 2200/2600 + default DLL path)
	std::string DllPath;            // optional explicit DSProxy DLL path; empty -> derive from VissimVersion

	int    SimulatorFrequency;      // Hz (sub-frame interpolation if > VISSIM internal step)
	double VisibilityRadius;        // meters; -1 = unlimited
	int    MaxSimulatorVeh;         // ceiling on simultaneous DS-controlled vehicles
	int    MaxSimulatorPed;
	int    MaxSimulatorDet;
	int    MaxTotalVeh;
	int    MaxVissimPed;
	int    MaxVissimSigGrp;

	// Stage B+ (issue #158). When true, TrafficLayer also opens a server
	// socket on SimulationSetup.TrafficSimulatorPort for a FIXS DriverModel
	// callback. Per tick the loop:
	//   - drains DriverModel's per-tick state messages (DSProxy is the
	//     canonical source for vehicle state, so DriverModel uploads are
	//     received and discarded — just to keep its socket buffer clear)
	//   - relays any non-ego VehFullData_t received from app clients down
	//     to DriverModel as behavior commands (desired speed / acceleration
	//     / lane change for Wiedemann-integrated CAVs)
	// Unlocks scenario 3a — Python CAV controller modulating background
	// vehicles while DSProxy drives the ego.
	bool EnableDriverModelRelay;
};

typedef struct SubscriptionVehicleList_t {
	std::unordered_set <std::string> edgeSubscribeId_v;

	// vehicle id -> radius
	std::unordered_map <std::string, double > vehicleSubscribeId_v;

	std::pair <bool, double> subscribeAllVehicle = { std::make_pair(false, 0) };

	// tuple x, y, z, radius
	// name of the point -> x, y, z, r
	std::unordered_map <std::string, std::tuple<double, double, double, double> > pointSubscribeId_v;
	//std::vector <std::string> pointNamePoi_v;

	std::unordered_map <std::string, double> vehicleTypeSubscribedId_v;
};

typedef struct SubscriptionSignalList_t {
	std::unordered_set <std::string> signalId_v;

	// Default-initialised: this was previously an indeterminate value until the
	// CarMaker branch assigned it. Mirrors SubscriptionVehicleList_t's
	// subscribeAllVehicle, which is default-initialised to {false, 0}.
	bool subAllSignalFlag = false;
};

typedef struct SubscriptionDetectorList_t {
	std::unordered_set <std::string> pattern_v;;
};

typedef struct SubscriptionAllList_t {
	SubscriptionVehicleList_t VehicleList;
	SubscriptionSignalList_t SignalList = { {}, false };
	SubscriptionDetectorList_t DetectorList;

};

// Generic FIXS infrastructure logging (see CommonLib/DataLogger). Config-driven,
// backend-agnostic: records the FIXS vehicle-data stream in the SUMO/VISSIM wire
// convention. Analysis/plotting of the output lives per-test, not here.
struct DataLogSetup_t {
	bool EnableDataLog = false;
	std::string DataLogPath = "auto";           // "auto" -> a component default path
	std::vector<std::string> DataLogWho;         // vehicle ids to log; empty = all seen
	std::vector<std::string> DataLogFields;      // VehFullData fields; empty = default core set
};

class ConfigHelper
{
public:
	ConfigHelper();

	int getConfig(std::string configName);

	// Flag to suppress "Will use X as default" messages during config parsing
	bool SuppressDefaultMessages = true;

	bool parserFlag(YAML::Node node, std::string name);
	std::string parserString(YAML::Node node, std::string name);
	double parserDouble(YAML::Node node, std::string name);
	int parserInteger(YAML::Node node, std::string name);
	void parserIntegerVector(YAML::Node node, std::string name, std::vector<int>& outIntegerVector);
	void parserStringVector(YAML::Node node, std::string name, std::vector<std::string>& outStringVector);

	void parserSubscription(YAML::Node node, std::string name, Subscription_t& subscription);


	void getVehSubscriptionList(Subscription_t VehSub, std::unordered_set <std::string>& edgeSubscribeId_v, std::unordered_map <std::string, double >& vehicleSubscribeId_v, std::pair <bool, double>& subscribeAllVehicle, std::unordered_map <std::string, std::tuple<double, double, double, double> >& pointSubscribeId_v, std::unordered_map <std::string, double>& vehicleTypeSubscribedId_v);
	
	void getSigSubscriptionList(Subscription_t SigSub);
	void getDetSubscriptionList(Subscription_t DetSub);

	// Ids the warm-up watches for when WarmUpUntilEgoEntry is set (#86): the UNION
	// of the by-id vehicle subscriptions in ApplicationSetup and XilSetup.
	//
	// Deliberately NOT SubscriptionVehicleList.vehicleSubscribeId_v, which is
	// either/or (XIL only when the application layer is off) and also drives what
	// TrafficLayer subscribes to in SUMO and forwards downstream. Widening that
	// would change the data path; this list only decides when the warm-up ends,
	// so it can see both layers' egos without touching message routing.
	std::unordered_set <std::string> WarmUpEgoIds;

	// subscription abstraction, these variables are raw configuration specs
	// users can use these raw specs to create their own subscription containers for different usages
	SimulationSetup_t SimulationSetup;
	ApplicationSetup_t ApplicationSetup;
	XilSetup_t XilSetup;
	EgoSetup_t EgoSetup;
	CarMakerSetup_t CarMakerSetup;
	SumoSetup_t SumoSetup;
	CarlaSetup_t CarlaSetup;
	VissimSetup_t VissimSetup;
	DataLogSetup_t DataLogSetup;




	// default containers to facilitate loop over each subscribed element, e.g., each vehicle id

	// provide containers
	struct FlagSetup{

	}FlagSetup;


	SubscriptionVehicleList_t SubscriptionVehicleList;

	SubscriptionSignalList_t SubscriptionSignalList;

	SubscriptionDetectorList_t SubscriptionDetectorList;


	// creat maps to hold socket port -> subscription list so that can use it to distribute messages

	std::unordered_map <int, SubscriptionAllList_t> SocketPort2SubscriptionList_um;

	void resetConfig();

private:
	void popErrorMessageBox(std::string message, std::string title);

	void extractSubscriptionAttributes(YAML::Node attnode, std::string type, std::string att, SubAttMap_t& attMap);

};

