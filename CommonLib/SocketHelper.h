#pragma once

// will use event driven socket in the future
//#include <event2/event.h>

//#include <event2/bufferevent.h>
//#include <event2/util.h>
//#include <event2/event.h>

#include "MsgHelper.h"

#ifndef WIN32
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <netinet/tcp.h>
	#include <arpa/inet.h>
	
	#include <ostream>
	#include <fstream>

	#include <stdio.h>
	#include <stdlib.h>
	#include <unistd.h>
	#include <string.h>
	#include <fcntl.h>
	#include <cstring>
	#include <string>

	#include <sys/select.h>
	
	#include <cmath>
	
	#include <cstdio>
	#include <cstdlib>
	
	#include <climits>

	#define SOCKET_ERROR (-1)
//extern int close(int __fildes);

	#include <CarMaker.h>

#else
	#include <winsock2.h>
	#include <ws2tcpip.h>
	#include <windows.h>
	#include <tchar.h>

	#pragma comment (lib, "Ws2_32.lib")
	#pragma comment (lib, "Mswsock.lib")
	#pragma comment (lib, "AdvApi32.lib")

#ifdef RS_DEBUG
#include <CarMaker.h>
#endif

#endif

#include <iostream>
#include <fstream>

#include <unordered_map>

#define MAXPENDING 5    /* Maximum outstanding connection requests */

// RealSim header size and each message header size
#define MSG_HEADER_SIZE 9
#define MSG_EACH_HEADER_SIZE 3


class SocketHelper
{

public:
    // constructor
    SocketHelper();

	// different ways to initialize socket
	void socketSetup(std::vector <std::string> SERVERADDR_UserInput, std::vector <int> SERVERPORT_UserInput);
	void socketSetup(std::vector <int> selfServerPortUserInput);
	void socketSetup(std::vector <std::string> SERVERADDR_UserInput, std::vector <int> SERVERPORT_UserInput, std::vector <int> selfServerPortUserInput);

	void socketReset();

	void enableWaitClientTrigger();
	void disableWaitClientTrigger();
	void enableServerTrigger();
	void disableServerTrigger();
	void disableServer();
	void disableClient();

	int initConnection(std::string errorLogName="");

	void socketShutdown();

	// recv data 
	int recvData(int sock, int* simState, float* simTime, MsgHelper& Msg_c);
		
	// send data
	int sendData(int sock, int iClient, float simTimeSend, uint8_t simStateSend, MsgHelper Msg_c);


	void printSocketErrorMessage(int errorCode);

// below should be converted to private in the future
//private:

	int NSERVER = 1;
	int NCLIENT = 1;

	int N_ACT_CLIENT = 0; // actual clients exculde VISSIM

	bool ENABLE_CLIENT = 0;
	bool ENABLE_SERVER = 1;

	bool ENABLE_WAIT_CLIENT_TRIGGER = 1;
	bool ENABLE_SERVER_TRIGGER = 1;

	std::vector <std::string> SERVERADDR;
	std::vector <int> SERVERPORT;

	std::vector <int> serverSock;
	std::vector <int> clientSock;

	std::vector <unsigned short> selfServerPort;

	// Server socket initialize
	std::vector <struct sockaddr_in> serverAddr;
	std::vector <int> serverAddrLen;
	std::vector <int> sendServerByte;

	// Client socket initialize
	std::vector <int> ClientConnected;
	int AllClientConnected = 0;

	std::vector <int> ClientRequested;
	int AllClientRequested = 0;


	int ClientRecvStatus = 0;

	//
	std::vector <int> selfServerSock;                    /* Socket descriptor for server */
	std::unordered_map <int, std::string> sockName_um;

	std::vector <struct sockaddr_in> selfServerAddr; /* Local a ddress */
	std::vector <struct sockaddr_in> clientAddr; /* Client address */
#ifndef WIN32
	std::vector <size_t> clientAddrLen;            /* Length of client address data structure */
#else
	std::vector <int> clientAddrLen;            /* Length of client address data structure */
#endif
	std::vector <int> sendClientByte;
	std::vector <int> recvClientMsgSize;                    /* Size of received message */
	 

};

