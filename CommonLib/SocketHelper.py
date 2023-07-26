from struct import unpack, pack

class SocketHelper:
    MSG_HEADER_SIZE = 9
    MSG_EACH_HEADER_SIZE = 3

    MSG_CODER = 'utf-8'

    def __init__(self):
        # empty constructer
        aa = 1

    def packHeader(self, simState, simTime, totalMsgSize):
        buffer = pack('<BfI', simState, simTime, totalMsgSize)

        return buffer

    def depackHeader(self, buffer):
        temp = unpack('<BfI', buffer)
        simState = temp[0]
        simTime = temp[1]
        totalMsgSize = temp[2]

        return simState, simTime, totalMsgSize

    def depackMsgType(self, buffer):
        temp = unpack('<HB', buffer)
        msgSize = temp[0]
        msgType = temp[1]

        return msgSize, msgType

    def packTrafficLightData(self, TrafficLightData):
        # need to skip the size part and add after all have been written
        buffer = b''

        # identifier
        buffer = buffer + pack('<B', 2)

        # message itself
        idLen = len(TrafficLightData['id'])
        buffer = buffer + pack('<B', idLen)
        buffer = buffer + pack('<'+str(idLen)+'s', bytes(TrafficLightData['id'], self.MSG_CODER))

        # sig id
        buffer = buffer + pack('<H', 0)

        idLen = len(TrafficLightData['state'])
        buffer = buffer + pack('<B', idLen)
        buffer = buffer + pack('<'+str(idLen)+'s', bytes(TrafficLightData['state'], self.MSG_CODER))

        buffer = pack('<H', len(buffer)+2) + buffer

        return buffer


    def depackDetectorData(self,buffer):
        nByte = len(buffer)
        sigLen = buffer[0]
        sigName = unpack('<{}s'.format(sigLen), buffer[1:(1+sigLen)])[0]
        sigId = unpack('<H', buffer[(1+sigLen):(1+sigLen+2)])[0]

        iByte = (1+sigLen+2)
        DetDataRecv_v = []
        while iByte<nByte:
            detIdLen = unpack('<B', buffer[iByte:iByte+1])[0]
            iByte = iByte + 1
            detId = unpack('<{}s'.format(detIdLen), buffer[iByte:iByte+detIdLen])[0]
            iByte = iByte + detIdLen
            
            iByte = iByte + 1
            detState = unpack('<B', buffer[iByte:iByte+1])[0]
            iByte = iByte + 1

            DetData_d = {'id': detId, 'state': detState}

            DetDataRecv_v.append(DetData_d)

        return DetDataRecv_v

    def recvData(self, sock):
        # initialize return lists
        VehDataRecv_v = []
        TlsDataRecv_v = []
        DetDataRecv_v = []

        # get header for entire message
        recvBuffer = sock.recv(self.MSG_HEADER_SIZE)
        simState, simTime, totalMsgSizeRecv = self.depackHeader(recvBuffer)

        msgProcessed = self.MSG_HEADER_SIZE

        while (msgProcessed < totalMsgSizeRecv):
            # get message type header
            recvBuffer = sock.recv(self.MSG_EACH_HEADER_SIZE)
            iMsgSizeRecv, iMsgTypeRecv = self.depackMsgType(recvBuffer)

            # get message it self
            recvBuffer = sock.recv(iMsgSizeRecv - self.MSG_EACH_HEADER_SIZE)
            
            # unpack message based on type identifier
            if iMsgTypeRecv == 1:
                aa = 1

            elif iMsgTypeRecv == 2:               
                aa = 1

            elif iMsgTypeRecv == 3:
                DetDataRecv_v = self.depackDetectorData(recvBuffer) 
            else:
                aa = 1

            msgProcessed = msgProcessed + iMsgSizeRecv
		    #msgProcessed = msgProcessed + iMsgSizeRecv

        return simState, simTime, VehDataRecv_v, TlsDataRecv_v, DetDataRecv_v


    def sendData(self, simState, simTime, vehSendBuffer, tlsSendBuffer, detSendBuffer):
        sendMsgSize = self.MSG_HEADER_SIZE + len(vehSendBuffer) + len(tlsSendBuffer) + len(detSendBuffer)
        headerSendBuffer = self.packHeader(simState, simTime, sendMsgSize)
            
        buffer = headerSendBuffer + vehSendBuffer + tlsSendBuffer + detSendBuffer

        return buffer