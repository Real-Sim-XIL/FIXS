from ControllerHelper import *
from CommonLib.SocketHelper import SocketHelper
import time
import socket
import select
import sys
import re
import schedule

# Get real-time phase status (red/yellow/green)
def getPhaseStatusGroupInfo( ctrlr ):
    #phaseStatusGroupReds.1, phaseStatusGroupYellows.1, phaseStatusGroupGreens.1
    oids = ["1.3.6.1.4.1.1206.4.2.1.1.4.1.2.1", "1.3.6.1.4.1.1206.4.2.1.1.4.1.3.1", "1.3.6.1.4.1.1206.4.2.1.1.4.1.4.1"] 
    redYellowGreenInt = ctrlr.Connect.get( oids )
    print(redYellowGreenInt)
    return redYellowGreenInt

# Map phase status to SUMO ryg states of all movements
def mapPhaseGroup2SUMOsig( redYellowGreenInt ):
    # initiate redYellowGreenState string with all red
    rygState = 'rrrrrrrrrrrr' # question: how many digits should this be?  I have 11 now
    # SUMO link junc index (signal head #) to m60 phases
    phaseSig = [[7,8], [9, 10, 11], [], [0,1,2,3,4], [], [5,6], [], [], []] # phase 1 cooresponds to link junc indices 7 and 8 in SUMO      

    # get green phases
    G = format(redYellowGreenInt["1.3.6.1.4.1.1206.4.2.1.1.4.1.4.1"],'08b')[::-1] # reverse string so that first character represents first phase
    G_list = [c.start() for c in re.finditer('1', G)]
    sigG = []
    for i in range(len(G_list)):
        sigG += phaseSig[G_list[i]]
    sigG.sort()
    # update redYellowGreenState string with greens
    rygState = ''.join(['G' if idx in sigG else ele for idx, ele in enumerate(list(rygState))])

    # get yellow phases
    y = format(redYellowGreenInt["1.3.6.1.4.1.1206.4.2.1.1.4.1.3.1"],'08b')[::-1] # reverse string
    y_list = [c.start() for c in re.finditer('1', y)]
    sigy = []
    for i in range(len(y_list)):
        sigy += phaseSig[y_list[i]]
    sigy.sort()

    # update redYellowGreenState string with yellows
    rygState = ''.join(['y' if idx in sigy else ele for idx, ele in enumerate(list(rygState))])
    return rygState

# Get phase status group information and (if flag == 1) send to Real-Sim
def getSendPhaseStatus( ctrlr ):
    t = time.time()
    TlsDataSend = {}
    phaseStatus = getPhaseStatusGroupInfo( ctrlr )
    # print(phaseStatus)

    if not phaseStatus == {}:
        SUMOsigState = mapPhaseGroup2SUMOsig( phaseStatus )
        TlsDataSend = {'id': TLS_SEL_ID, 'state': SUMOsigState}
    # print(TlsDataSend)

    if flag == 1:
        sendToSUMO(TlsDataSend)

    # print(time.time()-t)


# Send phase state in SUMO format back to SUMO 
def sendToSUMO(TlsDataSend):
    if len(TlsDataSend) > 0:
        tlsSendBuffer = Sock_c.packTrafficLightData(TlsDataSend) # need to make sure TlsDataSend follow this dictionary format
    else:
        tlsSendBuffer = b''
        
    # empty vehicle and detector send buffer
    vehSendBuffer = b''
    detSendBuffer = b''

    simStateSend = 1 # for now, keep this unchanged. 
    simTimeSend = 0 # this is the time where we want signal in SUMO to change. for now, set it same as current SUMO simulation time so that change signal light immediately
    
    sendBuffer = Sock_c.sendData(simStateSend, simTimeSend, vehSendBuffer, tlsSendBuffer, detSendBuffer)

    # send packed message
    sendStatus = serverSock.sendall(sendBuffer)

# Mapping detector calls in SUMO to a 8-bit binary call string
def mapSUMOdet2vehcall( det ):
    vehcall = '00000000'
    call_idx = []
    if next(iDet for iDet in det if iDet["id"] == b"SB75_SB WB L1")['state'] == 1 or next(iDet for iDet in det if iDet["id"] == b"SB75_SB WB L2")['state'] == 1:
        call_idx.append(0) # phase 1 gets vehcall
    if next(iDet for iDet in det if iDet["id"] == b"SB75_SB EB L1")['state'] == 1 or next(iDet for iDet in det if iDet["id"] == b"SB75_SB EB L2")['state'] == 1:
        call_idx.append(1) # phase 2 gets vehcall
    if next(iDet for iDet in det if iDet["id"] == b"SB75_SB SB L1")['state'] == 1 or next(iDet for iDet in det if iDet["id"] == b"SB75_SB SB L2")['state'] == 1 or next(iDet for iDet in det if iDet["id"] == b"SB75_SB SB L3")['state'] == 1:
        call_idx.append(3) # phase 4 gets vehcall
    if next(iDet for iDet in det if iDet["id"] == b"SB75_SB WB L3")['state'] == 1 or next(iDet for iDet in det if iDet["id"] == b"SB75_SB WB L4")['state'] == 1:
        call_idx.append(5) # phase 6 gets vehcall
    
    vehcall = ''.join(['1' if idx in call_idx else ele for idx, ele in enumerate(list(vehcall))])[::-1] # reverse order for NTCIP veh call format

    return vehcall


## ------------------ Main program starts ----------------------

# use flag = 1 when connecting SUMO, use anything else when not
flag = 1

# DEFINE SIGNAL CONTROLLER INTERSECTION ID, WHICH HAS TO BE THE ONE IN SUMO
TLS_SEL_ID = '2881'

if flag == 1:
    # Create a TCP/IP socket
    serverSock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    # Connect the socket to the port where the server is listening
    server_address = ('127.0.0.1', 2444)
    print('connecting to %s port %s' % server_address, file=sys.stderr)
    serverSock.connect(server_address)

    # # notify the SUMO server to start
    # startServiceMsg = 'FFFF'
    # serverSock.sendall(startServiceMsg.encode())

    # create class of SocketHelper
    Sock_c = SocketHelper()

# Exit option on startup
userInput = input("Press Y/y to enter program, any other key to exit: ")
if userInput.upper() != "Y":
    exit()

# Inform how to quit
print ("Running program, press Q/q and Return to quit")

controllers = {}
controllers[0] = Controller(IPaddr = '192.168.140.140', port = '161') # Siemens M60's default NTCIP port is 161
controllers[0].Register()

while True:
    # getPhaseStatusGroupInfo(controllers[0])
    getSendPhaseStatus(controllers[0])
    if flag == 1:
        simStateRecv, simTimeRecv, VehDataRecv_v, TlsDataRecv_v, DetDataRecv_v = Sock_c.recvData(serverSock)
    else:
        DetDataRecv_v = [
            {'id': 'SB75_SB SB L1', 'state': 1}, #phase4
            {'id': 'SB75_SB SB L2', 'state': 0}, #phase4
            {'id': 'SB75_SB SB L3', 'state': 0}, #phase4
            {'id': 'SB75_SB WB L1', 'state': 0}, #phase1
            {'id': 'SB75_SB WB L2', 'state': 0}, #phase1
            {'id': 'SB75_SB WB L3', 'state': 0}, #phase6
            {'id': 'SB75_SB WB L4', 'state': 0}, #phase6
            {'id': 'SB75_SB EB L1', 'state': 0}, #phase2
            {'id': 'SB75_SB EB L2', 'state': 0}  #phase2
        ] 
        
    # generate ntcip vehicle call string from sumo detector status
    vcalls = mapSUMOdet2vehcall( DetDataRecv_v )
    print( "Vehicle calls: {}".format(vcalls) )
    # Send SUMO vehicle calls as if they are real-world detectors to the signal controller
    controllers[0].NewGroupVehCalls( 1, vcalls )

    # # # # watch for Q/q and Return; otherwise next scheduled is run    
    # # watch for Q/q and Return; otherwise next scheduled is run
    # inp = select.select([sys.stdin], [], [], 1)[0]
    # if inp:
    #     value = sys.stdin.readline().rstrip()
    #     if (value.upper() == "Q"):
    #         controllers[0].Deregister()
    #         print("Exiting program")
    #         sys.exit(0)
    # else:
    #     schedule.run_pending()
    #     time.sleep(0.1)