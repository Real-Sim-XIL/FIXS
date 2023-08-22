/*
 * sfuntmpl_basic.c: Basic 'C' template for a level 2 S-function.
 *
 * Copyright 1990-2018 The MathWorks, Inc.
 */


/*
 * You must specify the S_FUNCTION_NAME as the name of your S-function
 * (i.e. replace sfuntmpl_basic with the name of your S-function).
 */

#define S_FUNCTION_NAME  RealSimSocket
#define S_FUNCTION_LEVEL 2

/*
 * input
 */


/*
 * Need to include simstruc.h for the definition of the SimStruct and
 * its associated macro definitions.
 */
#include "simstruc.h"

#include <iostream>
#include <fstream>
#include <vector>

#include <winsock2.h>
#include <ws2tcpip.h>
#include <mstcpip.h>
#include <windows.h>

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define RECVBUFFERSIZE 1024
#define SENDBUFFERSIZE 200


// s-function parameters
#define NPARAMS 3

#define SERVER_ADDR_IDX 0
#define SERVER_ADDR_PARAM(S) ssGetSFcnParam(S,SERVER_ADDR_IDX)

#define SERVER_PORT_IDX  1
#define SERVER_PORT_PARAM(S) ssGetSFcnParam(S,SERVER_PORT_IDX)

#define ENABLE_LOG_IDX  2
#define ENABLE_LOG_PARAM(S) ssGetSFcnParam(S,ENABLE_LOG_IDX)

using namespace std;


int enabledebug = 0;

// do not support multiple server yet
string SERVERADDR = "0.0.0.0";
int SERVERPORT = 0;
struct sockaddr_in serverAddr; /* Local a ddress */
int serverSock;

int serverConnected = 0;

// If nonblockFlag = 0, blocking is enabled;
// If nonblockFlag != 0, non-blocking mode is enabled.
//u_long nonblockFlag = 0;
int nonblockFlag = 0;

// this buffer will be held until new data is received
int gRecvSizeHold = 0;


double simTime = 0.0;
double simTimePrev = 0.0;

double connectionStartTime = 0.0;

static void socketShutdown(){
	
    // // socket close
    // shutdown(serverSock, SD_BOTH);
    // closesocket(serverSock);
    // WSACleanup();
    
    // reset local variables
    serverConnected = 0;
    nonblockFlag = 0;
    simTime = 0.0;
    simTimePrev = 0.0;   
    connectionStartTime = 0.0;
    gRecvSizeHold = (int) 0;
}


/* Error handling
 * --------------
 *
 * You should use the following technique to report errors encountered within
 * an S-function:
 *
 *       ssSetErrorStatus(S,"Error encountered due to ...");
 *       return;
 *
 * Note that the 2nd argument to ssSetErrorStatus must be persistent memory.
 * It cannot be a local variable. For example the following will cause
 * unpredictable errors:
 *
 *      mdlOutputs()
 *      {
 *         char msg[256];         {ILLEGAL: to fix use "static char msg[256];"}
 *         sprintf(msg,"Error due to %s", string);
 *         ssSetErrorStatus(S,msg);
 *         return;
 *      }
 *
 */

/*====================*
 * S-function methods *
 *====================*/

/* Function: mdlCheckParameters =============================================
 * Abstract:
 *    Validate our parameters to verify they are okay.
 */
#define MDL_CHECK_PARAMETERS   /* Change to #undef to remove function */
#if defined(MDL_CHECK_PARAMETERS) && defined(MATLAB_MEX_FILE)
static void mdlCheckParameters(SimStruct *S)
{
    /* Check 1st parameter: signs parameter */
    boolean_T illegalParam = 0;
    
    size_t    serverAddrLen;
    char_T    *serverAddrCharArray;
    
    serverAddrLen=mxGetNumberOfElements(SERVER_ADDR_PARAM(S));
    
    if (!mxIsChar(SERVER_ADDR_PARAM(S))) {
        illegalParam = 1;
    } else {
        if ((serverAddrCharArray=(char_T*)malloc(serverAddrLen+1)) == NULL) {
            ssSetErrorStatus(S,"Memory allocation error while parsing "
                    "1st parameters");
            return;
        }
        if (mxGetString(SERVER_ADDR_PARAM(S),serverAddrCharArray,serverAddrLen+1) != 0) {
            free(serverAddrCharArray);
            ssSetErrorStatus(S,"mxGetString error while parsing 1st "
                    "parameter");
            return;
        }else{


        }
//         free(serverAddrCharArray);
    }
    
    if (illegalParam) {
        ssSetErrorStatus(S,"1st parameter to S-function must be a "
                "string ");
        return;
    }
    
    /* Check second parameter: gain to be applied to the sum */
    if (!mxIsDouble(SERVER_PORT_PARAM(S)) ||
            mxGetNumberOfElements(SERVER_PORT_PARAM(S)) != 1) {
        ssSetErrorStatus(S,"Second parameter to S-function must be a "
                "scalar integer indicates port of the server to receive "
                "traffic message ");
        return;
    }
    
        /* Check second parameter: gain to be applied to the sum */
    if (!mxIsDouble(ENABLE_LOG_PARAM(S)) ||
            mxGetNumberOfElements(ENABLE_LOG_PARAM(S)) != 1) {
        ssSetErrorStatus(S,"Third parameter to S-function must be a "
                "scalar integer indicates whether or not enable log print");
        return;
    }
    
}
#endif /* MDL_CHECK_PARAMETERS */


/* Function: mdlInitializeSizes ===============================================
 * Abstract:
 *    The sizes information is used by Simulink to determine the S-function
 *    block's characteristics (number of inputs, outputs, states, etc.).
 */
static void mdlInitializeSizes(SimStruct *S)
{
    
    if (enabledebug){
        printf("begin init\n");
    }
    
    ssSetNumSFcnParams(S, NPARAMS);  /* Number of expected parameters */
#if defined(MATLAB_MEX_FILE)
    if (ssGetNumSFcnParams(S) == ssGetSFcnParamsCount(S)) {
        mdlCheckParameters(S);
        if (ssGetErrorStatus(S) != NULL) {
            printf("error get params\n");
            return;
        }
    } else {
        printf("number of params wrong\n");
        return; /* Parameter mismatch will be reported by Simulink */
    }
#endif
    
    ssSetSFcnParamTunable(S,SERVER_ADDR_IDX,false);
    ssSetSFcnParamTunable(S,SERVER_PORT_IDX,false);
    ssSetSFcnParamTunable(S,ENABLE_LOG_IDX,false);

    if (enabledebug){
        printf("get params\n");
    }
    
    size_t    serverAddrLen;
    char_T    *serverAddrCharArray;
    serverAddrLen=mxGetNumberOfElements(SERVER_ADDR_PARAM(S));
    serverAddrCharArray=(char_T*)malloc(serverAddrLen+1);
    mxGetString(SERVER_ADDR_PARAM(S),serverAddrCharArray,serverAddrLen+1);

    string serverAddrStr(serverAddrCharArray);
    SERVERADDR = serverAddrStr;
    SERVERPORT = (int) *mxGetPr(SERVER_PORT_PARAM(S));
    
    enabledebug = (int) *mxGetPr(ENABLE_LOG_PARAM(S));
    
    if (enabledebug || 1){
        printf("Parameters updated: server address %s, port %d, enabled log print %d\n", SERVERADDR, SERVERPORT, enabledebug);
    }
    
    ssSetNumContStates(S, 0);
    ssSetNumDiscStates(S, 0);
    
    if (!ssSetNumInputPorts(S, 5)) return;
    ssSetInputPortWidth(S, 0, 1);
    ssSetInputPortRequiredContiguous(S, 0, true); /*direct input signal access*/
    /*
     * Set direct feedthrough flag (1=yes, 0=no).
     * A port has direct feedthrough if the input is used in either
     * the mdlOutputs or mdlGetTimeOfNextVarHit functions.
     */
    ssSetInputPortDirectFeedThrough(S, 0, 1);
    
    ssSetInputPortWidth(S, 1, 1);
    ssSetInputPortRequiredContiguous(S, 1, true); /*direct input signal access*/
    ssSetInputPortDirectFeedThrough(S, 1, 1);
    
    ssSetInputPortWidth(S, 2, 1);
    ssSetInputPortRequiredContiguous(S, 2, true); /*direct input signal access*/
    ssSetInputPortDirectFeedThrough(S, 2, 1);
    ssSetInputPortDataType(S, 2, SS_BOOLEAN);

    ssSetInputPortWidth(S, 3, SENDBUFFERSIZE);
    ssSetInputPortRequiredContiguous(S, 3, true); /*direct input signal access*/
    ssSetInputPortDirectFeedThrough(S, 3, 0);
    ssSetInputPortDataType(S, 3, SS_UINT8);
    
    ssSetInputPortWidth(S, 4, 1);
    ssSetInputPortRequiredContiguous(S, 4, true); /*direct input signal access*/
    ssSetInputPortDirectFeedThrough(S, 4, 0);
    
    
    
    if (!ssSetNumOutputPorts(S, 3)) return;
    ssSetOutputPortWidth(S, 0, RECVBUFFERSIZE);
    ssSetOutputPortDataType(S, 0, SS_UINT8);
    ssSetOutputPortWidth(S, 1, 1);
    ssSetOutputPortWidth(S, 2, 1);
    ssSetOutputPortDataType(S, 2, SS_INT32);
    ssSetOutputPortOptimOpts(S, 0, SS_NOT_REUSABLE_AND_GLOBAL);

    ssSetNumSampleTimes(S, 1);
    ssSetNumRWork(S, 0);
    ssSetNumIWork(S, 0);
    ssSetNumPWork(S, 0); // reserve element in the pointers vector
    ssSetNumModes(S, 0); // to store a C++ object
    ssSetNumNonsampledZCs(S, 0);
    
    ssSetNumDWork(S, 1);
    ssSetDWorkWidth(S, 0, RECVBUFFERSIZE);
    ssSetDWorkDataType(S, 0, SS_INT8);
    ssSetDWorkName(S, 0, "recvDataHold");
    
    /* Specify the operating point save/restore compliance to be same as a
     * built-in block */
    ssSetOperatingPointCompliance(S, USE_DEFAULT_OPERATING_POINT);
    
    ssSetOptions(S, 0);
//     ssSetOptions(S,
//             SS_OPTION_SUPPORTS_ALIAS_DATA_TYPES | 
//             SS_OPTION_DISCRETE_VALUED_OUTPUT);
}



/* Function: mdlInitializeSampleTimes =========================================
 * Abstract:
 *    This function is used to specify the sample time(s) for your
 *    S-function. You must register the same number of sample times as
 *    specified in ssSetNumSampleTimes.
 */
static void mdlInitializeSampleTimes(SimStruct *S)
{
    ssSetSampleTime(S, 0, INHERITED_SAMPLE_TIME);
	//ssSetSampleTime(S, 0, CONTINUOUS_SAMPLE_TIME);
    ssSetOffsetTime(S, 0, 0.0);
    
}



#define MDL_INITIALIZE_CONDITIONS   /* Change to #undef to remove function */
#if defined(MDL_INITIALIZE_CONDITIONS)
/* Function: mdlInitializeConditions ========================================
 * Abstract:
 *    In this function, you should initialize the continuous and discrete
 *    states for your S-function block.  The initial states are placed
 *    in the state vector, ssGetContStates(S) or ssGetRealDiscStates(S).
 *    You can also perform any other initialization activities that your
 *    S-function may require. Note, this routine will be called at the
 *    start of simulation and if it is present in an enabled subsystem
 *    configured to reset states, it will be call when the enabled subsystem
 *    restarts execution to reset the states.
 */
static void mdlInitializeConditions(SimStruct *S)
{
}
#endif /* MDL_INITIALIZE_CONDITIONS */



#define MDL_START  /* Change to #undef to remove function */
#if defined(MDL_START)
/* Function: mdlStart =======================================================
 * Abstract:
 *    This function is called once at start of model execution. If you
 *    have states that should be initialized once, this is the place
 *    to do it.
 */
static void mdlStart(SimStruct* S)
{

	if (enabledebug) {
		printf("enter mdlStart\n");
	
		printf("server %s, port %d\n", SERVERADDR.c_str(), SERVERPORT);
	}

    
    // reset local variables
    serverConnected = 0;
    nonblockFlag = 0;
    simTime = 0.0;
    simTimePrev = 0.0;   
    gRecvSizeHold = (int) 0;
    
    
    real_T            *recvDataHold       = (real_T*) ssGetDWork(S, 0);
    memset(recvDataHold, 0, sizeof(recvDataHold));

	serverSock = 0;

	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	serverAddr = addr;


	WSADATA wsaData;                 /* Structure for WinSock setup communication */

	/* Load Winsock 2.0 DLL */
	if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0) {
		fprintf(stderr, "WSAStartup() failed");
		socketShutdown();
        ssSetErrorStatus(S, "ERROR: WSAStartup() failed");
        return;
	}

	/*==============================
	 * FIRST, connect to other Servers
	 * ==============================*/
	memset(&serverAddr, 0, sizeof(serverAddr));   /* Zero out structure */
	serverAddr.sin_addr.s_addr = inet_addr(SERVERADDR.c_str());
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVERPORT);

	// Create a SOCKET for connecting to server
	serverSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (serverSock == INVALID_SOCKET) {
		printf("socket failed with error: %ld\n", WSAGetLastError());
        socketShutdown();
        static char errmsg[100];
        sprintf(errmsg, "ERROR: socket failed with error: %ld\n", WSAGetLastError());
        ssSetErrorStatus(S, errmsg);
        return;
	}
			
//     int nodelayflag = 1; 	
// 	int resultt = setsockopt(serverSock, IPPROTO_TCP, TCP_NODELAY, (char*)&nodelayflag, sizeof(int));
            
	//-------------------------
	// Set the socket I/O mode: In this case FIONBIO
	// enables or disables the blocking mode for the
	// socket based on the numerical value of nonblockFlag.
	// If nonblockFlag = 0, blocking is enabled;
	// If nonblockFlag != 0, non-blocking mode is enabled.


// 	if (connect(serverSock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
// 		printf("Unable to connect to server! error: %d\n", WSAGetLastError());
//         socketShutdown();
//         static char errmsg[100];
//         sprintf(errmsg, "ERROR: Unable to connect to server! error: %ld\n", WSAGetLastError());
//         ssSetErrorStatus(S, errmsg);
//         return;
// 	}
// 	else {
// 		serverConnected = 3;
// 		printf("Server connected!");
// 	}    
    
	// //     }

    // send client trigger 
//     unsigned long StartServiceMsg = ULONG_MAX;
//     char sendBuf[10];
// 	memcpy(sendBuf, (char*)&StartServiceMsg, sizeof(StartServiceMsg));  
//     if (send(serverSock, sendBuf, sizeof(StartServiceMsg), 0) != sizeof(StartServiceMsg)) {
//         printf("send out error!!\n");
//         serverConnected = false;
//         socketShutdown();
//         ssSetLocalErrorStatus(S, "ERROR: send to TrafficLayer failed");
//         return;
//     }
    
	if (enabledebug) {
		printf("finish mdlStart\n");
	}
}
#endif /*  MDL_START */



/* Function: mdlOutputs =======================================================
 * Abstract:
 *    In this function, you compute the outputs of your S-function
 *    block.
 */
static void mdlOutputs(SimStruct *S, int_T tid)
{
    try{
        if (enabledebug){
            printf("start get output\n");
        }
        
        real_T       *ByteData = ssGetOutputPortRealSignal(S,0);
        real_T       *nByte = ssGetOutputPortRealSignal(S,1);
        real_T       *pServerConnectedFlag = ssGetOutputPortRealSignal(S,2);
        
                
        memcpy(pServerConnectedFlag, &serverConnected, sizeof(pServerConnectedFlag));
        if (enabledebug){
            printf("mdlOutputs serverConnected %d\n", (int) serverConnected);
        }

        *nByte = gRecvSizeHold;

        real_T            *recvDataHold       = (real_T            *) ssGetDWork(S, 0);
        memcpy(ByteData, recvDataHold, gRecvSizeHold);

        if (enabledebug){
            printf("end mdlOutputs\n");
        }
    }catch (const std::exception& e) {
        ssSetLocalErrorStatus(S, e.what());
        socketShutdown();
        return;
    }
    catch (...) {
        ssSetLocalErrorStatus(S, "UNKNOWN ERROR: calling mdlOutputs failed");
        socketShutdown();
        return;
    }

}



#define MDL_UPDATE  /* Change to #undef to remove function */
#if defined(MDL_UPDATE)
/* Function: mdlUpdate ======================================================
 * Abstract:
 *    This function is called once for every major integration time step.
 *    Discrete states are typically updated here, but this function is useful
 *    for performing any tasks that should only take place once per
 *    integration step.
 */
static void mdlUpdate(SimStruct* S, int_T tid)
{
    
    try{
   
        const real_T* u = (const real_T*)ssGetInputPortSignal(S, 0);
        //     real_T       *ByteData = ssGetOutputPortRealSignal(S,0);
        //     real_T       *nByte = ssGetOutputPortRealSignal(S,1);
        //     int_T             yWidth = ssGetOutputPortWidth(S,0);
        
        const real_T* u2 = (const real_T*)ssGetInputPortSignal(S, 1);
        double dt = u2[0];
        
        const boolean_T* pEnableConnection = (const boolean_T*)ssGetInputPortSignal(S, 2);
        bool enableConnection = (bool)pEnableConnection[0];
//         bool enableConnection = 1;
        
        const real_T* ByteOut = (const real_T*)ssGetInputPortSignal(S, 3);
        const real_T* nByteOut = (const real_T*)ssGetInputPortSignal(S, 4);
        
        if (enabledebug) {
            printf("\n========================\n");
            printf("simulation time: %f\n", u[0]);
            printf("start update\n");
        }
     
        if (enabledebug) {
            printf("input serverConnected %d,", serverConnected);
            printf("enableConnection %d\n", enableConnection);
            //     printf("pEnableConnection[0] \d\n", pEnableConnection[0]);
        }
        
        if (!serverConnected && enableConnection) {
            // Connect to server.
            if (connect(serverSock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
                printf("Unable to connect to server! error: %d\n", WSAGetLastError());
                socketShutdown();
                static char errmsg[100];
                sprintf(errmsg, "ERROR: Unable to connect to server! error: %ld\n", WSAGetLastError());
                ssSetErrorStatus(S, errmsg);
                return;
            }
            else {
                serverConnected = 3;
                printf("Server connected!");
            } 
            
            connectionStartTime = u[0];
        }
        
        if (serverConnected) {
            if (enabledebug) {
                printf("enter receive step, u[0] %f, simTimePrev %f, dt %f\n", u[0], simTimePrev, dt);
            }
            
            // only do this for the very first step
            if (abs(u[0] - connectionStartTime) <= 1e-5){
                int recvSize = 0;
                char recvBuf[RECVBUFFERSIZE];
                recvSize = recv(serverSock, recvBuf, sizeof(recvBuf), 0);
                if (recvSize == SOCKET_ERROR || recvSize < 1) {
                    serverConnected = 0;
                    socketShutdown();
                    ssSetStopRequested(S, 1);
                    //ssSetLocalErrorStatus(S, "\n\nreceive from TrafficLayer failed. This may not be a real error but part of the shutdown process of RealSim Interface, TrafficLayer might have closed, then please stop VISSIM/SUMO manually.");
                    return;
                };
            }
            
            
            // Begin of 0.1 cycle loop
            if (u[0] - simTimePrev + 1e-5 >= dt || abs(u[0] - connectionStartTime) <= 1e-5) {
                if (enabledebug) {
                    printf("\n=============================\n");
                    printf("0.1 trigger at %f\n", u[0]);
                }
                
                char sendBuf[SENDBUFFERSIZE];
                memcpy(sendBuf, ByteOut, *nByteOut);
                
                if (send(serverSock, sendBuf, *nByteOut, 0) != *nByteOut) {
                    printf("send out error!!\n");
                    serverConnected = 0;
                    //exit(EXIT_FAILURE);
                    socketShutdown();                   
                    //ssSetLocalErrorStatus(S, "ERROR: send to TrafficLayer failed");
                    ssSetStopRequested(S, 1);
                    return;
                    
                }
                
                if (enabledebug) {
                    printf("send out %d bytes\n", (int) *nByteOut);
                }
                
//                 simTimePrev = u[0];
            }
            
            
            int recvSize;
            // !! simple recv non-blocking
            char recvBuf[RECVBUFFERSIZE]={0};
            
            //memset(recvBuf, 0, RECVBUFFERSIZE);
            
            // keep receive if it is nonblocking (nonblocking for real-time control system)
            if (nonblockFlag && 0){
                if (enabledebug) {
                    printf("enter nonblock receive\n");
                }
                            
                recvSize = recv(serverSock, recvBuf, sizeof(recvBuf), 0);
                if (recvSize < 1) {
                    //             printf("nonblocking no data\n");
                    if (enabledebug) {
                        printf("nonblock did not receive\n");
                    }
                }
                else {
                    if (enabledebug) {
                        printf("nonblock receive\n");
                    }
                    real_T            *recvDataHold       = (real_T*) ssGetDWork(S, 0);                           
                    memset(recvDataHold, 0, sizeof(recvDataHold));
                    memcpy(recvDataHold, recvBuf, recvSize);
                    gRecvSizeHold = recvSize;
                }
            }
            // if blocking, then only receive every 0.1 seconds
            else if (u[0] - simTimePrev + 1e-5 >= dt  || abs(u[0] - connectionStartTime) <= 1e-5) {
                if (enabledebug) {
                    printf("enter blocking receive\n");
                }
                            
                recvSize = recv(serverSock, recvBuf, sizeof(recvBuf), 0);
                if (recvSize == SOCKET_ERROR || recvSize < 1) {
                    //printf("recv() failed with error code : %d", WSAGetLastError());
                    serverConnected = 0;
                    //exit(EXIT_FAILURE);
                    socketShutdown();
                    //ssSetLocalErrorStatus(S, "\n\nreceive from TrafficLayer failed. This may not be a real error but part of the shutdown process of RealSim Interface, TrafficLayer might have closed, then please stop VISSIM/SUMO manually.");
                    ssSetStopRequested(S, 1);
                    return;
                };
                
                if (enabledebug) {
                    printf("received %d bytes\n", recvSize);
                }

                real_T            *recvDataHold       = (real_T*) ssGetDWork(S, 0);                           
                memset(recvDataHold, 0, sizeof(recvDataHold));
                memcpy(recvDataHold, recvBuf, recvSize);
                gRecvSizeHold = recvSize;
                
            }

            if (u[0] - simTimePrev + 1e-5 >= dt){
                simTimePrev = u[0];
            }// END of 0.1 cycle loop
            
        }
            
        if (enabledebug) {
            printf("end update\n");
        }
        
    }catch (const std::exception& e) {
        ssSetLocalErrorStatus(S, e.what());
        socketShutdown();
        return;
    }
    catch (...) {
        ssSetLocalErrorStatus(S, "UNKNOWN ERROR: calling mdlUpdate failed");
        socketShutdown();
        return;
    }

}
#endif /* MDL_UPDATE */



#define MDL_DERIVATIVES  /* Change to #undef to remove function */
#if defined(MDL_DERIVATIVES)
/* Function: mdlDerivatives =================================================
 * Abstract:
 *    In this function, you compute the S-function block's derivatives.
 *    The derivatives are placed in the derivative vector, ssGetdX(S).
 */
static void mdlDerivatives(SimStruct *S)
{
}
#endif /* MDL_DERIVATIVES */



/* Function: mdlTerminate =====================================================
 * Abstract:
 *    In this function, you should perform any actions that are necessary
 *    at the termination of a simulation.  For example, if memory was
 *    allocated in mdlStart, this is the place to free it.
 */
static void mdlTerminate(SimStruct *S)
{

    try{
        // try to shutdown gracefully
        shutdown(serverSock, SD_BOTH);
        
        // // Receive until the peer closes the connection
        // int recvSize = 1;
        // char recvBuf[RECVBUFFERSIZE];
        // do {
            // recvSize = recv(serverSock, recvBuf, sizeof(recvBuf), 0);
            // if ( recvSize > 0 )
                // printf("Bytes received after close operation: %d\n", recvSize);
            // else if ( recvSize == 0 )
                // printf("Connection closed\n");
            // else
                // printf("recv failed with error: %d\n", WSAGetLastError());

        // } while( recvSize > 0 );

        // cleanup again
        closesocket(serverSock);
        WSACleanup();
    
    }catch (const std::exception& e) {
        ssSetLocalErrorStatus(S, e.what());
        socketShutdown();
        return;
    }
    catch (...) {
        ssSetLocalErrorStatus(S, "UNKNOWN ERROR: calling mdlTerminate failed");
        socketShutdown();
        return;
    }
    
    printf("stops\n");
}


/*=============================*
 * Required S-function trailer *
 *=============================*/

#ifdef  MATLAB_MEX_FILE    /* Is this file being compiled as a MEX-file? */
#include "simulink.c"      /* MEX-file interface mechanism */
#else
#include "cg_sfun.h"       /* Code generation registration function */
#endif
