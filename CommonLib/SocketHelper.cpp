#include "SocketHelper.h"

using namespace std;

// The convention is that, Server always provide service to the Client. 
// i.e. Manager will be the Server to provide traffic data service to all possible clients, e.g. Controller, Communication, Vehicle. 
// The Controller can be another service provider to Vehicle to command desired speed
// The Communication will be another service provider to Controller to provide traffic information. 
// 
// [              ] - [Controller] - [      ]
// [Another Server] - [Controller] - [Client]
// [              ] - [Controller] - [      ]
//
// First, connect to other server
// Then, wait all clients connect
// Next, signal other server to start service
// Last, start own service => return to the caller of Socket initConnection

SocketHelper::SocketHelper() {

}

void SocketHelper::printSocketErrorMessage(int errorCode) {
	
#ifdef WIN32
	switch (errorCode) {
	case 10014:
		printf("Client is closed. Exiting...\n");
		break;
	case 10054:
		printf("WSAECONNRESET: Connection reset by peer. \n \
			An existing connection was forcibly closed by the remote host.This normally results if the peer application \
			on the remote host is suddenly stopped, the host is rebooted, the host or remote network interface \
			is disabled, or the remote host uses a hard close(see setsockopt for more information on the SO_LINGER option \
			on the remote socket).This error may also result if a connection was broken due to keep - \
			alive activity detecting a failure while one or more operations are in progress.Operations that were in progress fail with WSAENETRESET.Subsequent operations fail with WSAECONNRESET.\n\n");
		printf("Suggested Solutions: \n");
		printf("\t try to find who is using the ports using PowerShell:");
		printf("\t Get-Process -Id (Get-NetTCPConnection -LocalPort <PORT_NUMBER>).OwningProcess \n");

		break;
	default:
		printf("Error Code not supported yet\n");
	}
#else

#endif

}


void SocketHelper::socketSetup(vector <string> SERVERADDR_UserInput, vector <int> SERVERPORT_UserInput, vector <int> selfServerPortUserInput) {


	this->socketReset();

	for (size_t iS = 0; iS < SERVERADDR_UserInput.size(); iS++) {
		SERVERADDR.push_back(SERVERADDR_UserInput[iS]);
		SERVERPORT.push_back(SERVERPORT_UserInput[iS]);
	}

	for (size_t iC = 0; iC < selfServerPortUserInput.size(); iC++) {
		selfServerPort.push_back(selfServerPortUserInput[iC]);
	}

	NSERVER = SERVERADDR.size();
	NCLIENT = selfServerPortUserInput.size();

	if (NSERVER > 0) {
		ENABLE_SERVER = 1;
	}
	else {
		ENABLE_SERVER = 0;
	}

	if (NCLIENT > 0) {
		ENABLE_CLIENT = 1;
	}
	else {
		ENABLE_CLIENT = 0;
	}

	//!!! SHOULD READ A CONFIG TO GET THE FOLLOWING VARIABLES
	for (int iS = 0; iS < NSERVER; iS++) {
		serverSock.push_back(0);

		struct sockaddr_in addr;
		memset(&addr, 0, sizeof(addr));
		serverAddr.push_back(addr);

		serverAddrLen.push_back(0);

		sendServerByte.push_back(0);
	}

	for (int iC = 0; iC < NCLIENT; iC++) {
		clientSock.push_back(0);

		ClientConnected.push_back(0);

		selfServerSock.push_back(0);

		struct sockaddr_in addr;
		memset(&addr, 0, sizeof(addr));
		selfServerAddr.push_back(addr);
		clientAddr.push_back(addr);

		clientAddrLen.push_back(0);

		sendClientByte.push_back(0);
		recvClientMsgSize.push_back(0);

		ClientRequested.push_back(0);
	}


	// SANITY CHECK BEFORE EXITING 
	if (NCLIENT < 1) {
		ENABLE_WAIT_CLIENT_TRIGGER = 0;
		printf("WARNING: no client specified, turn off wait client trigger\n");
		//Sleep(1000);
	}
	if (NSERVER < 1) {
		ENABLE_SERVER_TRIGGER = 0;
		printf("WARNING: no server specified, turn off server trigger\n");
		//Sleep(1000);
	}

}

void SocketHelper::socketSetup(vector <string> SERVERADDR_UserInput, vector <int> SERVERPORT_UserInput) {

	this->socketReset();

	for (size_t iS = 0; iS < SERVERADDR_UserInput.size(); iS++) {
		SERVERADDR.push_back(SERVERADDR_UserInput[iS]);
		SERVERPORT.push_back(SERVERPORT_UserInput[iS]);
	}

	NSERVER = SERVERADDR.size();
	NCLIENT = 0;
	ENABLE_CLIENT = 0;
	ENABLE_WAIT_CLIENT_TRIGGER = 0;

	if (NSERVER > 0) {
		ENABLE_SERVER = 1;
	}
	else {
		ENABLE_SERVER = 0;
	}

	//!!! SHOULD READ A CONFIG TO GET THE FOLLOWING VARIABLES
	for (int iS = 0; iS < NSERVER; iS++) {
		serverSock.push_back(0);

		struct sockaddr_in addr;
		memset(&addr, 0, sizeof(addr));
		serverAddr.push_back(addr);

		serverAddrLen.push_back(0);

		sendServerByte.push_back(0);
	}
}

void SocketHelper::socketSetup(vector <int> selfServerPortUserInput) {

	this->socketReset();

	for (size_t iC = 0; iC < selfServerPortUserInput.size(); iC++) {
		selfServerPort.push_back(selfServerPortUserInput[iC]);
	}

	NSERVER = 0;
	ENABLE_SERVER = 0;
	ENABLE_SERVER_TRIGGER = 0;

	NCLIENT = selfServerPortUserInput.size();

	if (NCLIENT > 0) {
		ENABLE_CLIENT = 1;
	}
	else {
		ENABLE_CLIENT = 0;
	}

	for (int iC = 0; iC < NCLIENT; iC++) {
		clientSock.push_back(0);

		ClientConnected.push_back(0);

		selfServerSock.push_back(0);

		struct sockaddr_in addr;
		memset(&addr, 0, sizeof(addr));
		selfServerAddr.push_back(addr);
		clientAddr.push_back(addr);

		clientAddrLen.push_back(0);

		sendClientByte.push_back(0);
		recvClientMsgSize.push_back(0);

		ClientRequested.push_back(0);

	}
}

void SocketHelper::socketReset() {

	SERVERADDR.clear();
	SERVERPORT.clear();

	selfServerPort.clear();

	serverSock.clear();
	serverAddr.clear();
	serverAddrLen.clear();
	sendServerByte.clear();

	clientSock.clear();

	ClientConnected.clear();

	selfServerSock.clear();
	selfServerAddr.clear();
	clientAddr.clear();

	clientAddrLen.clear();

	sendClientByte.clear();
	recvClientMsgSize.clear();
	ClientRequested.clear();

	sockName_um.clear();

	AllClientConnected = 0;
	AllClientRequested = 0;
	ClientRecvStatus = 0;

	N_ACT_CLIENT = 0;

}

void SocketHelper::socketShutdown() {

	for (int iS = 0; iS < NCLIENT; iS++) {
#ifdef WIN32
		shutdown(selfServerSock[iS], SD_BOTH);
		closesocket(selfServerSock[iS]);
		shutdown(clientSock[iS], SD_BOTH);
		closesocket(clientSock[iS]);
#else
		shutdown(selfServerSock[iS], SHUT_RDWR);
		shutdown(clientSock[iS], SHUT_RDWR);
	#ifdef DSRTLX
		close(selfServerSock[iS]);
		close(clientSock[iS]);
	#endif
#endif
	}
	for (int iS = 0; iS < NSERVER; iS++) {
#ifdef WIN32
		shutdown(serverSock[iS], SD_BOTH);
		closesocket(serverSock[iS]);
#else
		shutdown(serverSock[iS], SHUT_RDWR);
	#ifdef DSRTLX
		close(serverSock[iS]);
	#endif
#endif	
	}

#ifdef WIN32
	WSACleanup();
#else

#endif

}


void SocketHelper::enableWaitClientTrigger() {
	// if enable, then need to receive ULONG_MAX signal to trigger start of service
	ENABLE_WAIT_CLIENT_TRIGGER = true;
}

void SocketHelper::disableWaitClientTrigger() {
	ENABLE_WAIT_CLIENT_TRIGGER = false;
}

void SocketHelper::enableServerTrigger() {
	// if enable, then will send ULONG_MAX signal to trigger start of service
	ENABLE_SERVER_TRIGGER = true;
}

void SocketHelper::disableServerTrigger() {
	ENABLE_SERVER_TRIGGER = false;
}

void SocketHelper::disableServer() {
	ENABLE_SERVER = false;
}

void SocketHelper::disableClient() {
	ENABLE_CLIENT = false;
	NCLIENT = 0;
}

int SocketHelper::initConnection(std::string errorLogName) {
	const int RECVCLIENTBUFSIZE = 2048;
	const int SENDCLIENTBUFSIZE = 8096;
	// const int RECVSERVERBUFSIZE = 8096;
	// const int SENDSERVERBUFSIZE = 2048;

	if (N_ACT_CLIENT < 1) {
		AllClientConnected = 1;
	}

#ifdef WIN32

	WSADATA wsaData;                 /* Structure for WinSock setup communication */

		/* Load Winsock 2.0 DLL */
	if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0) {
		fprintf(stderr, "WSAStartup() failed");

		if (!errorLogName.empty()) {
			fstream f(errorLogName, std::fstream::in | std::fstream::out | std::fstream::app);
			f << "WSAStartup() failed" << endl;
			f.close();
		}
		WSACleanup();
		return -1;
	}
#endif

	/*==============================
	* FIRST, connect to other Servers
	==============================*/
	if (ENABLE_SERVER) {
#ifdef RS_DEBUG
		Log("RealSim start server task\n");	
#endif
		for (int iS = 0; iS < NSERVER; iS++) {
			memset(&serverAddr[iS], 0, sizeof(serverAddr[iS]));   /* Zero out structure */
			serverAddr[iS].sin_addr.s_addr = inet_addr(SERVERADDR[iS].c_str());
			serverAddr[iS].sin_family = AF_INET;
			serverAddr[iS].sin_port = htons(SERVERPORT[iS]);

			// Create a SOCKET for connecting to server
			serverSock[iS] = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
#ifdef WIN32
			if (serverSock[iS] == INVALID_SOCKET) {
				if (!errorLogName.empty()) {
					fstream f(errorLogName, std::fstream::in | std::fstream::out | std::fstream::app);
					f << "socket failed with error : " << WSAGetLastError() << endl;
					f.close();
				}
				printf("socket failed with error: %ld\n", WSAGetLastError());
				closesocket(serverSock[iS]);
				WSACleanup();
				return -1;
			}
#else
			if (serverSock[iS] == -1) {
				if (!errorLogName.empty()) {
					fstream f(errorLogName, std::fstream::in | std::fstream::out | std::fstream::app);
					f << "socket failed  " << endl;
					f.close();
				}
				printf("socket failed \n");
				//close(serverSock[iS]);
				return -1;
			}
#endif

			int nodelayflag = 1; 	
			int resultt = setsockopt(serverSock[iS], IPPROTO_TCP, TCP_NODELAY, (char*)&nodelayflag, sizeof(int));
			if (resultt){
				// placeholder
			}
#ifdef RS_DEBUG
			Log("RealSim start server connection\n");	
#endif
			// Connect to server.
			if (connect(serverSock[iS], (struct sockaddr*)&serverAddr[iS], sizeof(serverAddr[iS])) < 0) {
				if (!errorLogName.empty()) {
					fstream f(errorLogName, std::fstream::in | std::fstream::out | std::fstream::app);
#ifdef WIN32
					f << "Unable to connect to server! error: " << WSAGetLastError() << endl;
#else
					f << "Unable to connect to server! error: " <<  endl;
#endif
					f.close();
				}
#ifdef WIN32
				printf("Unable to connect to server! error: %ld\n", WSAGetLastError());
				closesocket(serverSock[iS]);
				WSACleanup();
				printSocketErrorMessage(WSAGetLastError());
#else
				printf("Unable to connect to server! error\n");
			#ifdef DSRTLX
				close(serverSock[iS]);
			#endif
#endif
				return -1;
			}
#ifdef RS_DEBUG
			Log("RealSim server connected\n");	
#endif

		}
	}

	// ===========================================================================
	// 			SECOND, wait for Clients to connect
	// ===========================================================================
	// -------------------
	//  Initialize socket for clients
	// -------------------
	if (ENABLE_CLIENT) {
		for (int iS = 0; iS < NCLIENT; iS++) {
			/* Create socket for incoming connections */
			if ((selfServerSock[iS] = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
				if (!errorLogName.empty()) {
					fstream f(errorLogName, std::fstream::in | std::fstream::out | std::fstream::app);
#ifdef WIN32
					f << "self server socket() failed! error: " << WSAGetLastError() << endl;
#else
					f << "self server socket() failed! error: " << endl;
#endif
					f.close();
				}
#ifdef WIN32
				fprintf(stderr, "%s: %d\n", "socket() failed", WSAGetLastError());
				closesocket(selfServerSock[iS]);
				WSACleanup();
#else
				fprintf(stderr, "%s: \n", "socket() failed");
			#ifdef DSRTLX
				close(selfServerSock[iS]);
			#endif
#endif
				return -1;
			}

			/* Construct local address structure */
			memset(&selfServerAddr[iS], 0, sizeof(selfServerAddr[iS]));   /* Zero out structure */
			selfServerAddr[iS].sin_family = AF_INET;                /* Internet address family */
			selfServerAddr[iS].sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
			selfServerAddr[iS].sin_port = htons(selfServerPort[iS]);      /* Local port */

			/* Bind to the local address */
			if (::bind(selfServerSock[iS], (struct sockaddr*)&selfServerAddr[iS], sizeof(selfServerAddr[iS])) < 0) {
#ifdef WIN32
				fprintf(stderr, "%s: %d\n", "bind() failed", WSAGetLastError());
				if (!errorLogName.empty()) {
					fstream f(errorLogName, std::fstream::in | std::fstream::out | std::fstream::app);
					f << "bind() failed! error: " << WSAGetLastError() << endl;
					f.close();
				}
				closesocket(selfServerSock[iS]);
				WSACleanup();
#else
				fprintf(stderr, "%s: \n", "bind() failed");
				if (!errorLogName.empty()) {
					fstream f(errorLogName, std::fstream::in | std::fstream::out | std::fstream::app);
					f << "bind() failed! error: " << endl;
					f.close();
			}
			#ifdef DSRTLX
				close(selfServerSock[iS]);
			#endif
#endif
				return -1;
			}

			/* Mark the socket so it will listen for incoming connections */
			if (listen(selfServerSock[iS], MAXPENDING) < 0) {
#ifdef WIN32
				if (!errorLogName.empty()) {
					fstream f(errorLogName, std::fstream::in | std::fstream::out | std::fstream::app);
					f << "listen() failed! error: " << WSAGetLastError() << endl;
					f.close();
				}
				fprintf(stderr, "%s: %d\n", "listen() failed", WSAGetLastError());
				closesocket(selfServerSock[iS]);
				WSACleanup();
#else
				if (!errorLogName.empty()) {
					fstream f(errorLogName, std::fstream::in | std::fstream::out | std::fstream::app);
					f << "listen() failed! error: " << endl;
					f.close();
				}
				fprintf(stderr, "%s:\n", "listen() failed");
			#ifdef DSRTLX
				close(selfServerSock[iS]);
			#endif
#endif
				return -1;
			}
		}
	}

	//+++++++++
	// Wait for clients to connections
	//+++++++++
	if (ENABLE_CLIENT) {
		//set of socket descriptors  
		fd_set readfds;

		int activity;

		int max_sd = selfServerSock[0];

		cout << "Waiting for all clients to connect...." << endl;

		while (!AllClientConnected) {
			//clear the socket set  
			FD_ZERO(&readfds);

			//add master socket to set  

			for (int iS = 0; iS < N_ACT_CLIENT; iS++) {
				FD_SET(selfServerSock[iS], &readfds);
				if (selfServerSock[iS] > max_sd) {
					max_sd = selfServerSock[iS];
				}

				if (clientSock[iS] > 0)
					FD_SET(clientSock[iS], &readfds);
				if (clientSock[iS] > max_sd) {
					max_sd = clientSock[iS];
				}
			}

			activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

			if ((activity < 0) && (errno != EINTR))
			{
#ifdef WIN32
				if (!errorLogName.empty()) {
					fstream f(errorLogName, std::fstream::in | std::fstream::out | std::fstream::app);
					f << "select error! error: " << WSAGetLastError() << endl;
					f.close();
				}
				fprintf(stderr, "%s: %d\n", "select error", WSAGetLastError());
#else
				if (!errorLogName.empty()) {
					fstream f(errorLogName, std::fstream::in | std::fstream::out | std::fstream::app);
					f << "select error! error: " << endl;
					f.close();
			}
				fprintf(stderr, "%s:\n", "select error");
#endif
			}

			//If something happened on the master socket ,  
		   //then its an incoming connection  
			for (int iS = 0; iS < N_ACT_CLIENT; iS++) {
				if (FD_ISSET(selfServerSock[iS], &readfds))
				{
					clientAddrLen[iS] = sizeof(clientAddr[iS]);

					if ((clientSock[iS] = accept(selfServerSock[iS], (struct sockaddr*)&clientAddr[iS], &clientAddrLen[iS])) < 0) {
#ifdef WIN32
						if (!errorLogName.empty()) {
							fstream f(errorLogName, std::fstream::in | std::fstream::out | std::fstream::app);
							f << "accept() failed! error: " << WSAGetLastError() << endl;
							f.close();
						}
						fprintf(stderr, "%s: %d\n", "accept() failed", WSAGetLastError());
						closesocket(clientSock[iS]);
						WSACleanup();
#else
						if (!errorLogName.empty()) {
							fstream f(errorLogName, std::fstream::in | std::fstream::out | std::fstream::app);
							f << "accept() failed! error: " << endl;
							f.close();
					}
						fprintf(stderr, "%s: \n", "accept() failed");
					#ifdef DSRTLX
						close(clientSock[iS]);
					#endif
#endif
						return -1;
					}

					int nodelayflag = 1;
					int resultt = setsockopt(clientSock[iS], IPPROTO_TCP, TCP_NODELAY, (char*)&nodelayflag, sizeof(int));
					if (resultt){
						// placeholder
					}

					/* clientSock is connected to a client! */
					printf("Handling client #%d %s\n", iS + 1, inet_ntoa(clientAddr[iS].sin_addr));

					ClientConnected[iS] = 1;
				}
			}

			AllClientConnected = 1;
			for (int iS = 0; iS < N_ACT_CLIENT; iS++) {
				if (!ClientConnected[iS]) {
					AllClientConnected = 0;
					break;
				}
			}

		}

		// -------------------
		//  after connecting to all clients, then connect to VISSIM, if selected as VISSIM
		// -------------------
		if ((int)selfServerSock.size() > N_ACT_CLIENT) {
			int AllVissimConnected = 0;

			cout << "Waiting VISSIM to start...." << endl;

			while (!AllVissimConnected) {
				//clear the socket set  
				FD_ZERO(&readfds);

				//add master socket to set  
				for (size_t iS = N_ACT_CLIENT; iS < selfServerSock.size(); iS++) {
					FD_SET(selfServerSock[iS], &readfds);
					if (selfServerSock[iS] > max_sd) {
						max_sd = selfServerSock[iS];
					}

					if (clientSock[iS] > 0)
						FD_SET(clientSock[iS], &readfds);
					if (clientSock[iS] > max_sd) {
						max_sd = clientSock[iS];
					}
				}

				activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

				if ((activity < 0) && (errno != EINTR))
				{
#ifdef WIN32
					if (!errorLogName.empty()) {
						fstream f(errorLogName, std::fstream::in | std::fstream::out | std::fstream::app);
						f << "select error! error: " << WSAGetLastError() << endl;
						f.close();
					}
#else
					if (!errorLogName.empty()) {
						fstream f(errorLogName, std::fstream::in | std::fstream::out | std::fstream::app);
						f << "select error! error: " << endl;
						f.close();
				}
#endif
					printf("select error");
				}

				//If something happened on the master socket ,  
			   //then its an incoming connection  
				for (size_t iS = N_ACT_CLIENT; iS < selfServerSock.size(); iS++) {
					if (FD_ISSET(selfServerSock[iS], &readfds))
					{
						clientAddrLen[iS] = sizeof(clientAddr[iS]);

						if ((clientSock[iS] = accept(selfServerSock[iS], (struct sockaddr*)&clientAddr[iS], &clientAddrLen[iS])) < 0) {
#ifdef WIN32
							if (!errorLogName.empty()) {
								fstream f(errorLogName, std::fstream::in | std::fstream::out | std::fstream::app);
								f << "accept() failed! error: " << WSAGetLastError() << endl;
								f.close();
							}
							fprintf(stderr, "%s: %d\n", "accept() failed", WSAGetLastError());
							//exit(1);
							closesocket(clientSock[iS]);
							WSACleanup();

#else
							if (!errorLogName.empty()) {
								fstream f(errorLogName, std::fstream::in | std::fstream::out | std::fstream::app);
								f << "accept() failed! error: "  << endl;
								f.close();
							}
							fprintf(stderr, "%s: \n", "accept() failed");
							//exit(1);
						#ifdef DSRTLX
							close(clientSock[iS]);
						#endif
#endif
							return -1;
						}

						int nodelayflag = 1;
						int resultt = setsockopt(clientSock[iS], IPPROTO_TCP, TCP_NODELAY, (char*)&nodelayflag, sizeof(int));
						if (resultt){
							// placeholder
						}

						/* clientSock is connected to a client! */
						printf("Handling VISSIM client #%d %s\n", iS + 1 - N_ACT_CLIENT, inet_ntoa(clientAddr[iS].sin_addr));

						ClientConnected[iS] = 1;
					}
				}

				AllVissimConnected = 1;
				for (size_t iS = N_ACT_CLIENT; iS < selfServerSock.size(); iS++) {
					if (!ClientConnected[iS]) {
						AllVissimConnected = 0;
						break;
					}
				}

			}
		}

	}

	cout << "All Clients Connected!" << endl;

	/*==============================
	* THIRD, signal server to start service
	==============================*/
	if (ENABLE_CLIENT && ENABLE_WAIT_CLIENT_TRIGGER) {
		// check if has been signaled to start service
		while (!AllClientRequested) {
			AllClientRequested = 1;
			for (int iS = 0; iS < NCLIENT; iS++) {
				char recvClientBuffer[RECVCLIENTBUFSIZE];
				if ((recvClientMsgSize[iS] = recv(clientSock[iS], recvClientBuffer, sizeof(unsigned long), 0)) == SOCKET_ERROR) {
#ifdef WIN32
					if (!errorLogName.empty()) {
						fstream f(errorLogName, std::fstream::in | std::fstream::out | std::fstream::app);
						f << "recv() client trigger failed! error: " << WSAGetLastError() << endl;
						f.close();
					}
					printf("recv() failed with error code : %d", WSAGetLastError());
					//exit(EXIT_FAILURE);
					closesocket(clientSock[iS]);
					WSACleanup();
#else
					if (!errorLogName.empty()) {
						fstream f(errorLogName, std::fstream::in | std::fstream::out | std::fstream::app);
						f << "recv() client trigger failed! error: " << endl;
						f.close();
					}
					printf("recv() failed with error code ");
					//exit(EXIT_FAILURE);
					#ifdef DSRTLX
						close(clientSock[iS]);
					#endif
#endif
					return -1;
				}
				unsigned long tempUl;
				memcpy(&tempUl, recvClientBuffer, sizeof(unsigned long));
				if (tempUl == ULONG_MAX); {
					ClientRequested[iS] = 1;
				}
			}
			for (int iS = 0; iS < NCLIENT; iS++) {
				if (!ClientRequested[iS]) {
					AllClientRequested = 0;
					break;
				}
			}
		}

		cout << "Client service started!!" << endl;
	}

	// SENDING FFFF, four bytes signal
	if (ENABLE_SERVER && ENABLE_SERVER_TRIGGER) {
		cout << "Request Server to start service......." << endl;

		for (int iS = 0; iS < NSERVER; iS++) {
			unsigned long StartServiceMsg = ULONG_MAX;
			char sendClientBuffer[SENDCLIENTBUFSIZE];
			memcpy(sendClientBuffer, (char*)&StartServiceMsg, sizeof(StartServiceMsg));
			int sendMsgSize = sizeof(StartServiceMsg);
			if (send(serverSock[iS], sendClientBuffer, sendMsgSize, 0) < 0)
			{
#ifdef WIN32
				if (!errorLogName.empty()) {
					fstream f(errorLogName, std::fstream::in | std::fstream::out | std::fstream::app);
					f << "send() server trigger failed! error: " << WSAGetLastError() << endl;
					f.close();
				}
				printf("send failed with error: %d\n", WSAGetLastError());
				closesocket(serverSock[iS]);
				WSACleanup();
#else
				if (!errorLogName.empty()) {
					fstream f(errorLogName, std::fstream::in | std::fstream::out | std::fstream::app);
					f << "send() server trigger failed! error: " << endl;
					f.close();
				}
				printf("send failed with error:\n");
				#ifdef DSRTLX
					close(serverSock[iS]);
				#endif
#endif
				return -1;
			}
		}
	}

	cout << "RealSim Initialized" << endl;
#ifdef RS_DEBUG
	Log("RealSim Initialized\n");	
#endif 

	return 0;
}

int SocketHelper::recvData(int sock, int* simState, float* simTime, MsgHelper& Msg_c) {
	// this function will receive all possible data

	//int recvStatus = 0;

	char recvBuffer[200];

	int recvSize;

	if ((recvSize = recv(sock, recvBuffer, Msg_c.msgHeaderSize, 0)) == SOCKET_ERROR) {
#ifdef WIN32
		if (WSAGetLastError() == WSAEINTR) {
			return -1;
		}
		else if (WSAGetLastError() == WSAEFAULT) {
			printSocketErrorMessage(10014);
			return -1;
		}
		printf("recv() failed with code : %d\n", WSAGetLastError());
#else
		printf("recv() failed with code :\n");
#endif
		return -1;
	}
	//+++++++++
	// Parser received message
	//+++++++++

	uint32_t msgProcessed = Msg_c.msgHeaderSize;
	uint8_t simStateRecv;
	uint32_t totalMsgSizeRecv;
	float simTimeRecv;

	Msg_c.depackHeader(recvBuffer, &simStateRecv, &simTimeRecv, &totalMsgSizeRecv);
	*simState = simStateRecv;
	*simTime = simTimeRecv;


	while (msgProcessed < totalMsgSizeRecv) {
		VehFullData_t iVehData;
		TrafficLightData_t iTlsData;
		TlsDetector_t iDetData;

		uint16_t iMsgSizeRecv=0;
		uint8_t iMsgTypeRecv = 0;

		char recvVehDataHeaderBuf[256];
		if ((recvSize = recv(sock, recvVehDataHeaderBuf, Msg_c.msgEachHeaderSize, 0)) == SOCKET_ERROR) {
#ifdef WIN32
			if (WSAGetLastError() == WSAEFAULT) {
				printSocketErrorMessage(10014);
				return -1;
			}
			printf("recvfrom() failed with error code : %d", WSAGetLastError());
#else
			printf("recvfrom() failed with error code :");
#endif
			return -1;
		}
		Msg_c.depackMsgType(recvVehDataHeaderBuf, &iMsgSizeRecv, &iMsgTypeRecv);

		char recvEachDataBuf[1024];
		// this recvSize is purely the message body, without the Msg_c.msgEachHeaderSize
		if ((recvSize = recv(sock, recvEachDataBuf, iMsgSizeRecv - Msg_c.msgEachHeaderSize, 0)) == SOCKET_ERROR) {
#ifdef WIN32
			if (WSAGetLastError() == WSAEFAULT) {
				printSocketErrorMessage(10014);
				return -1;
			}
			printf("recvfrom() failed with error code : %d", WSAGetLastError());
#else
			printf("recvfrom() failed with error code :");
#endif
			return -1;
		}


		string id;
		switch (iMsgTypeRecv) {
		case 1:
			Msg_c.depackVehData(recvEachDataBuf, iVehData);
			id = iVehData.id;
			Msg_c.VehDataRecv_um[id] = iVehData;
			break;
		case 2:
			Msg_c.depackTrafficLightData(recvEachDataBuf, &iTlsData);
			Msg_c.TlsDataRecv_um[iTlsData.name] = iTlsData;
			break;
		case 3:
			Msg_c.depackDetectorData(recvEachDataBuf, recvSize, &iDetData);
			Msg_c.DetDataRecv_um[get<1>(iDetData)] = iDetData;
			break;
		}

		msgProcessed += iMsgSizeRecv;
	}

	return 0;
}



int SocketHelper::sendData(int sock, int iClient, float simTimeSend, uint8_t simStateSend, MsgHelper Msg_c) {
	// for each socket, send only the message relevant to that socket

	int iByte = 0;

	// initialize send data buffer
	//char tempVehDataBuffer[8096];
	char* tempVehDataBuffer = new char[65536];
	int tempVehDataByte = 0;

	char* tempTlsDataBuffer = new char[8096];
	int tempTlsDataByte = 0;

	char* tempDetDataBuffer = new char[8096];
	int tempDetDataByte = 0;

	int sendMsgSize = 0;
	char* sendBuffer = new char[81728];

	// Pack vehicle and detector data
	if (Msg_c.VehDataSend_um.size() > 0) {
		for (size_t iV = 0; iV < Msg_c.VehDataSend_um[sock].size(); iV++) {
			Msg_c.packVehData(Msg_c.VehDataSend_um[sock][iV], tempVehDataBuffer, &tempVehDataByte);
		}
	}

	if (Msg_c.TlsDataSend_um.size() > 0) {
		for (size_t iT = 0; iT < Msg_c.TlsDataSend_um[sock].size(); iT++) {
			Msg_c.packTrafficLightData(Msg_c.TlsDataSend_um[sock][iT], tempTlsDataBuffer, &tempTlsDataByte);
		}
	}

	if (Msg_c.DetDataSend_um.size() > 0) {
		for (size_t iD = 0; iD < Msg_c.DetDataSend_um[sock].size(); iD++) {
			Msg_c.packDetectorData(Msg_c.DetDataSend_um[sock][iD], tempDetDataBuffer, &tempDetDataByte);
		}
	}

	// pack header
	Msg_c.packHeader(simStateSend, simTimeSend, MSG_HEADER_SIZE + tempVehDataByte + tempTlsDataByte + tempDetDataByte, sendBuffer, &iByte);
	sendMsgSize = MSG_HEADER_SIZE;

	// pack vehicle data
	memcpy(sendBuffer + sendMsgSize, tempVehDataBuffer, tempVehDataByte);
	sendMsgSize += tempVehDataByte;

	// pack traffic light data
	memcpy(sendBuffer + sendMsgSize, tempTlsDataBuffer, tempTlsDataByte);
	sendMsgSize += tempTlsDataByte;

	// pack detector data
	memcpy(sendBuffer + sendMsgSize, tempDetDataBuffer, tempDetDataByte);
	sendMsgSize += tempDetDataByte;

	// send all vehicle and detector data
	if (send(sock, sendBuffer, sendMsgSize, 0) != sendMsgSize) {
#ifdef WIN32
		fprintf(stderr, "%s: %d\n", "send() failed", WSAGetLastError());
#else
		fprintf(stderr, "%s:\n", "send() failed");
#endif
		//exit(1);
		return -1;
	}


	//if (tempVehDataByte[iEgo] > 0) {
	if (tempVehDataByte > 0) {
		//printf(" ====> sending: time %.1f #vehicles %d packet size %d\n", simTimeSend, (int)Msg_c.VehIdSend_v[iC].size(), sendMsgSize);
	}
	if (tempDetDataByte > 0) {

	}

	return 0;

}