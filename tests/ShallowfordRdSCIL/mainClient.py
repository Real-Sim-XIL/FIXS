import socket
import sys

from CommonLib.SocketHelper import SocketHelper


# DEFINE SIGNAL CONTROLLER INTERSECTION ID, WHICH HAS TO BE THE ONE IN SUMO
TLS_SEL_ID = '2881'

# Create a TCP/IP socket
serverSock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Connect the socket to the port where the server is listening
server_address = ('localhost', 2444)
print('connecting to %s port %s' % server_address, file=sys.stderr)
serverSock.connect(server_address)

# # notify the SUMO server to start
# startServiceMsg = 'FFFF'
# serverSock.sendall(startServiceMsg.encode())

# create class of SocketHelper
Sock_c = SocketHelper()

# keep receiving and sending
while 1:

    # 1) retrieve detector data
    simStateRecv, simTimeRecv, VehDataRecv_v, TlsDataRecv_v, DetDataRecv_v = Sock_c.recvData(serverSock)
    # next(iDet for iDet in DetDataRecv_v if iDet["detId"] == "SB75_SB SB L3") # to search for a particular detector id
    #
    # print out detectors received
    print("time %.1f" % simTimeRecv)
    print("detector status table:")
    for iDet in DetDataRecv_v:
        print("\t %s,\t %d" % (iDet['id'], iDet['state']))
    
    print("\n")

    # 2) do something on the signal controller side
    # send detector to SC
    # process
    # recv detector from SC

    # 3) send signal to SUMO if new control exists
    # otherwise send 'empty' message (with only 9 bytes message header) to trigger next simulation step

    # if no new signal light decision, do this    
    tlsSendBuffer = b''
    # if there's new signal light decision, do this
    TlsDataSend = {}
    simTime = simTimeRecv - 39600
    if simTime < 20:
        TlsDataSend = {'id': TLS_SEL_ID, 'state': 'GGGGrrrrrrrr'} # this is a dummy state, check SUMO for index of each light
    elif simTime > 20 and simTime < 30:
        TlsDataSend = {'id': TLS_SEL_ID, 'state': 'rrrrGrrrrrrr'} 
    elif simTime > 30 and simTime < 40:
        TlsDataSend = {'id': TLS_SEL_ID, 'state': 'rrrrrGGrrrrr'} 
    elif simTime > 40 and simTime < 50:
        TlsDataSend = {'id': TLS_SEL_ID, 'state': 'rrrrrrrGGrrr'} 
    elif simTime > 50 and simTime < 60:
        TlsDataSend = {'id': TLS_SEL_ID, 'state': 'rrrrrrrrrGGG'} 
    
    if len(TlsDataSend) > 0:
        tlsSendBuffer = Sock_c.packTrafficLightData(TlsDataSend) # need to make sure TlsDataSend follow this dictionary format

    # empty vehicle and detector send buffer
    vehSendBuffer = b''
    detSendBuffer = b''

    simStateSend = simStateRecv # for now, keep this unchanged. 
    simTimeSend = simTimeRecv # this is the time where we want signal in SUMO to change. for now, set it same as current SUMO simulation time so that change signal light immediately
    
    sendBuffer = Sock_c.sendData(simStateSend, simTimeSend, vehSendBuffer, tlsSendBuffer, detSendBuffer)

    # send packed message
    sendStatus = serverSock.sendall(sendBuffer)

    if len(TlsDataSend) > 0:
        print("Send signal control:")
        print("\t %s:\t %s" % (TlsDataSend['id'], TlsDataSend['state']))
        print("\n")

    aa = 1
