
import socket
import sys, os

sys.path.append(os.path.dirname((os.path.dirname(os.path.dirname(os.path.abspath(__file__))))))

from CommonLib.SocketHelper import SocketHelper


# DEFINE SIGNAL CONTROLLER INTERSECTION ID, WHICH HAS TO BE THE ONE IN SUMO
TLS_SEL_ID = '2881'

hostList = [7331, 2444]

N_HOST = len(hostList)
serverSock = N_HOST*[0]
for i in range(0,N_HOST):
    # Create a TCP/IP socket
    serverSock[i] = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    # Connect the socket to the port where the server is listening
    server_address = ('localhost', hostList[i])
    print('connecting to %s port %s' % server_address, file=sys.stderr)
    serverSock[i].connect(server_address)

# # notify the SUMO server to start
# startServiceMsg = 'FFFF'
# serverSock.sendall(startServiceMsg.encode())

# create class of SocketHelper
Sock_c = SocketHelper()

# keep receiving and sending
while 1:

    # 1) retrieve data
    for i in range(0,N_HOST): 
        simStateRecv, simTimeRecv, VehDataRecv_v, TlsDataRecv_v, DetDataRecv_v = Sock_c.recvData(serverSock[i])

    print("time %.1f" % simTimeRecv)

    # 2) do something on the signal controller side
    # send detector to SC
    # process
    # recv detector from SC

    # 3) send back to SUMO 
    for i in range(0,N_HOST): 
        # if no new signal light decision, do this    
        tlsSendBuffer = b''

        # empty vehicle and detector send buffer
        vehSendBuffer = b''
        detSendBuffer = b''

        simStateSend = simStateRecv # for now, keep this unchanged. 
        simTimeSend = simTimeRecv # this is the time where we want signal in SUMO to change. for now, set it same as current SUMO simulation time so that change signal light immediately
        
        sendBuffer = Sock_c.sendData(simStateSend, simTimeSend, vehSendBuffer, tlsSendBuffer, detSendBuffer)

        # send packed message
        sendStatus = serverSock[i].sendall(sendBuffer)

    pass