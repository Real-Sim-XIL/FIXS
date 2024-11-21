import VehDataMsgDefs
import struct
class MsgHelper:
    
    def __init__(self):
        self.VehicleMessageFieldDef = {
            'id': False,
            'type': False,
            'vehicleClass': False,
            'speed': False,
            'acceleration': False,
            'positionX': False,
            'positionY': False,
            'positionZ': False,
            'heading': False,
            'color': False,
            'linkId': False,
            'laneId': False,
            'distanceTravel': False,
            'speedDesired': False,
            'accelerationDesired': False,
            'hasPrecedingVehicle': False,
            'precedingVehicleId': False,
            'precedingVehicleDistance': False,
            'precedingVehicleSpeed': False,
            'signalLightId': False,
            'signalLightHeadId': False,
            'signalLightDistance': False,
            'signalLightColor': False,
            'speedLimit': False,
            'speedLimitNext': False,
            'speedLimitChangeDistance': False,
            'linkIdNext': False,
            'grade': False,
            'activeLaneChange': False
        }
        # DO NOT change. These are predetermined message header size
        # specified for Real-Sim
        self.msgHeaderSize = 9
        self.msgEachHeaderSize = 3
       # define data types identifier
        vehMsgIdentifer = 1;
        # debugging function
        enableDebug = 0;
        # vehicle data message
        self.VehicleDataEmpty = VehDataMsgDefs.VehFullData_t()
        # VehicleMessageFieldDefInputVec = zeros(1, 29); % Vector of 0,1 to select which message field will be transmitted
        
    import struct

    def depack_veh_data(self, VehData: dict, ByteData: bytes):
        iByte = 0  # Index in ByteData
        iByte += self.msgHeaderSize  # Skip the message header
        iByte += self.msgEachHeaderSize
        # Helper function to unpack a single float
        def unpack_float(data, index):
            value = struct.unpack('f', data[index:index+4])[0]
            return value, index + 4

        # Helper function to unpack a single int32
        def unpack_int32(data, index):
            value = struct.unpack('i', data[index:index+4])[0]
            return value, index + 4

        # Helper function to unpack a single uint32
        def unpack_uint32(data, index):
            value = struct.unpack('I', data[index:index+4])[0]
            return value, index + 4

        # Helper function to unpack a single int8
        def unpack_int8(data, index):
            value = struct.unpack('b', data[index:index+1])[0]
            return value, index + 1

        # Unpack fields based on VehicleMessageFieldDef
        if self.VehicleMessageFieldDef.get('id'):
            str_data, str_len, iByte, uint8Arr = MsgHelper.depack_string(ByteData, iByte)
            VehData['id'] = uint8Arr
            VehData['idLength'] = str_len

        if self.VehicleMessageFieldDef.get('type'):
            str_data, str_len, iByte, uint8Arr = MsgHelper.depack_string(ByteData, iByte)
            VehData['type'] = uint8Arr
            VehData['typeLength'] = str_len

        if self.VehicleMessageFieldDef.get('vehicleClass'):
            str_data, str_len, iByte, uint8Arr = MsgHelper.depack_string(ByteData, iByte)
            VehData['vehicleClass'] = uint8Arr
            VehData['vehicleClassLength'] = str_len

        if self.VehicleMessageFieldDef.get('speed'):
            VehData['speed'], iByte = unpack_float(ByteData, iByte)

        if self.VehicleMessageFieldDef.get('acceleration'):
            VehData['acceleration'], iByte = unpack_float(ByteData, iByte)

        if self.VehicleMessageFieldDef.get('positionX'):
            VehData['positionX'], iByte = unpack_float(ByteData, iByte)

        if self.VehicleMessageFieldDef.get('positionY'):
            VehData['positionY'], iByte = unpack_float(ByteData, iByte)

        if self.VehicleMessageFieldDef.get('positionZ'):
            VehData['positionZ'], iByte = unpack_float(ByteData, iByte)

        if self.VehicleMessageFieldDef.get('heading'):
            VehData['heading'], iByte = unpack_float(ByteData, iByte)

        if self.VehicleMessageFieldDef.get('color'):
            VehData['color'], iByte = unpack_uint32(ByteData, iByte)

        if self.VehicleMessageFieldDef.get('linkId'):
            str_data, str_len, iByte, uint8Arr = MsgHelper.depack_string(ByteData, iByte)
            VehData['linkId'] = uint8Arr
            VehData['linkIdLength'] = str_len

        if self.VehicleMessageFieldDef.get('laneId'):
            VehData['laneId'], iByte = unpack_int32(ByteData, iByte)

        if self.VehicleMessageFieldDef.get('distanceTravel'):
            VehData['distanceTravel'], iByte = unpack_float(ByteData, iByte)

        if self.VehicleMessageFieldDef.get('speedDesired'):
            VehData['speedDesired'], iByte = unpack_float(ByteData, iByte)

        if self.VehicleMessageFieldDef.get('accelerationDesired'):
            VehData['accelerationDesired'], iByte = unpack_float(ByteData, iByte)

        if self.VehicleMessageFieldDef.get('hasPrecedingVehicle'):
            VehData['hasPrecedingVehicle'], iByte = unpack_int8(ByteData, iByte)

        if self.VehicleMessageFieldDef.get('precedingVehicleId'):
            str_data, str_len, iByte, uint8Arr = MsgHelper.depack_string(ByteData, iByte)
            VehData['precedingVehicleId'] = uint8Arr
            VehData['precedingVehicleIdLength'] = str_len

        if self.VehicleMessageFieldDef.get('precedingVehicleDistance'):
            VehData['precedingVehicleDistance'], iByte = unpack_float(ByteData, iByte)

        if self.VehicleMessageFieldDef.get('precedingVehicleSpeed'):
            VehData['precedingVehicleSpeed'], iByte = unpack_float(ByteData, iByte)

        if self.VehicleMessageFieldDef.get('signalLightId'):
            str_data, str_len, iByte, uint8Arr = MsgHelper.depack_string(ByteData, iByte)
            VehData['signalLightId'] = uint8Arr
            VehData['signalLightIdLength'] = str_len

        if self.VehicleMessageFieldDef.get('signalLightHeadId'):
            VehData['signalLightHeadId'], iByte = unpack_int32(ByteData, iByte)

        if self.VehicleMessageFieldDef.get('signalLightDistance'):
            VehData['signalLightDistance'], iByte = unpack_float(ByteData, iByte)

        if self.VehicleMessageFieldDef.get('signalLightColor'):
            VehData['signalLightColor'], iByte = unpack_int8(ByteData, iByte)

        if self.VehicleMessageFieldDef.get('speedLimit'):
            VehData['speedLimit'], iByte = unpack_float(ByteData, iByte)

        if self.VehicleMessageFieldDef.get('speedLimitNext'):
            VehData['speedLimitNext'], iByte = unpack_float(ByteData, iByte)

        if self.VehicleMessageFieldDef.get('speedLimitChangeDistance'):
            VehData['speedLimitChangeDistance'], iByte = unpack_float(ByteData, iByte)

        if self.VehicleMessageFieldDef.get('linkIdNext'):
            str_data, str_len, iByte, uint8Arr = MsgHelper.depack_string(ByteData, iByte)
            VehData['linkIdNext'] = uint8Arr
            VehData['linkIdNextLength'] = str_len

        if self.VehicleMessageFieldDef.get('grade'):
            VehData['grade'], iByte = unpack_float(ByteData, iByte)

        if self.VehicleMessageFieldDef.get('activeLaneChange'):
            VehData['activeLaneChange'], iByte = unpack_int8(ByteData, iByte)

        return VehData
    import struct

    def pack_veh_data(self, simState: int, t: float, ByteData: bytearray, VehData: dict):
        # Calculate nMsgSize based on VehicleMessageFieldDef flags and VehData field lengths
        nMsgSize = (
            round(self.msgHeaderSize + self.msgEachHeaderSize
                  + self.VehicleMessageFieldDef.get('id', 0) * (1 + VehData.get('idLength', 0))
                  + self.VehicleMessageFieldDef.get('type', 0) * (1 + VehData.get('typeLength', 0))
                  + self.VehicleMessageFieldDef.get('vehicleClass', 0) * (1 + VehData.get('vehicleClassLength', 0))
                  + self.VehicleMessageFieldDef.get('speed', 0) * 4  # speed
                  + self.VehicleMessageFieldDef.get('acceleration', 0) * 4  # acceleration
                  + self.VehicleMessageFieldDef.get('positionX', 0) * 4  # positionX
                  + self.VehicleMessageFieldDef.get('positionY', 0) * 4  # positionY
                  + self.VehicleMessageFieldDef.get('positionZ', 0) * 4  # positionZ
                  + self.VehicleMessageFieldDef.get('heading', 0) * 4  # heading
                  + self.VehicleMessageFieldDef.get('color', 0) * 4  # color
                  + self.VehicleMessageFieldDef.get('linkId', 0) * (1 + VehData.get('linkIdLength', 0))  # linkId
                  + self.VehicleMessageFieldDef.get('laneId', 0) * 4  # laneId
                  + self.VehicleMessageFieldDef.get('distanceTravel', 0) * 4  # distanceTravel
                  + self.VehicleMessageFieldDef.get('speedDesired', 0) * 4  # speedDesired
                  + self.VehicleMessageFieldDef.get('accelerationDesired', 0) * 4  # accelerationDesired
                  + self.VehicleMessageFieldDef.get('hasPrecedingVehicle', 0) * 1  # hasPrecedingVehicle
                  + self.VehicleMessageFieldDef.get('precedingVehicleId', 0) * (1 + VehData.get('precedingVehicleIdLength', 0))  # precedingVehicleId
                  + self.VehicleMessageFieldDef.get('precedingVehicleDistance', 0) * 4  # precedingVehicleDistance
                  + self.VehicleMessageFieldDef.get('precedingVehicleSpeed', 0) * 4  # precedingVehicleSpeed
                  + self.VehicleMessageFieldDef.get('signalLightId', 0) * (1 + VehData.get('signalLightIdLength', 0))  # signalLightId
                  + self.VehicleMessageFieldDef.get('signalLightHeadId', 0) * 4  # signalLightHeadId
                  + self.VehicleMessageFieldDef.get('signalLightDistance', 0) * 4  # signalLightDistance
                  + self.VehicleMessageFieldDef.get('signalLightColor', 0) * 1  # signalLightColor
                  + self.VehicleMessageFieldDef.get('speedLimit', 0) * 4  # speedLimit
                  + self.VehicleMessageFieldDef.get('speedLimitNext', 0) * 4  # speedLimitNext
                  + self.VehicleMessageFieldDef.get('speedLimitChangeDistance', 0) * 4  # speedLimitChangeDistance
                  + self.VehicleMessageFieldDef.get('linkIdNext', 0) * (1 + VehData.get('linkIdNextLength', 0))  # linkIdNext
                  + self.VehicleMessageFieldDef.get('grade', 0) * 4  # grade
                  + self.VehicleMessageFieldDef.get('activeLaneChange', 0) * 1  # activeLaneChange
            )
        )
        nVehMsgSize = round(nMsgSize - self.msgHeaderSize)

        # Initialize iByte index for ByteData
        iByte = 0

        # Pack simState as uint8
        ByteData[iByte] = simState
        iByte += 1

        # Pack time t as a float (4 bytes)
        ByteData[iByte:iByte+4] = struct.pack('f', t)
        iByte += 4

        # Pack message size as uint32 (4 bytes)
        ByteData[iByte:iByte+4] = struct.pack('I', nMsgSize)
        iByte += 4

        # Pack nVehMsgSize as uint16 (2 bytes)
        ByteData[iByte:iByte+2] = struct.pack('H', nVehMsgSize)
        iByte += 2

        # Pack message type as uint8 (set to 1 as in MATLAB code)
        ByteData[iByte] = 1
        iByte += 1

        # Pack fields conditionally based on VehicleMessageFieldDef
        if self.VehicleMessageFieldDef.get('id'):
            ByteData, iByte = MsgHelper.pack_string(ByteData, iByte, VehData, 'id')

        if self.VehicleMessageFieldDef.get('type'):
            ByteData, iByte = MsgHelper.pack_string(ByteData, iByte, VehData, 'type')

        if self.VehicleMessageFieldDef.get('vehicleClass'):
            ByteData, iByte = MsgHelper.pack_string(ByteData, iByte, VehData, 'vehicleClass')

        if self.VehicleMessageFieldDef.get('speed'):
            ByteData[iByte:iByte+4] = struct.pack('f', VehData['speed'])
            iByte += 4

        if self.VehicleMessageFieldDef.get('acceleration'):
            ByteData[iByte:iByte+4] = struct.pack('f', VehData['acceleration'])
            iByte += 4

        if self.VehicleMessageFieldDef.get('positionX'):
            ByteData[iByte:iByte+4] = struct.pack('f', VehData['positionX'])
            iByte += 4

        if self.VehicleMessageFieldDef.get('positionY'):
            ByteData[iByte:iByte+4] = struct.pack('f', VehData['positionY'])
            iByte += 4

        if self.VehicleMessageFieldDef.get('positionZ'):
            ByteData[iByte:iByte+4] = struct.pack('f', VehData['positionZ'])
            iByte += 4

        if self.VehicleMessageFieldDef.get('heading'):
            ByteData[iByte:iByte+4] = struct.pack('f', VehData['heading'])
            iByte += 4

        if self.VehicleMessageFieldDef.get('color'):
            ByteData[iByte:iByte+4] = struct.pack('I', VehData['color'])
            iByte += 4

        if self.VehicleMessageFieldDef.get('linkId'):
            ByteData, iByte = MsgHelper.pack_string(ByteData, iByte, VehData, 'linkId')

        if self.VehicleMessageFieldDef.get('laneId'):
            ByteData[iByte:iByte+4] = struct.pack('i', VehData['laneId'])
            iByte += 4

        if self.VehicleMessageFieldDef.get('distanceTravel'):
            ByteData[iByte:iByte+4] = struct.pack('f', VehData['distanceTravel'])
            iByte += 4

        if self.VehicleMessageFieldDef.get('speedDesired'):
            ByteData[iByte:iByte+4] = struct.pack('f', VehData['speedDesired'])
            iByte += 4

        if self.VehicleMessageFieldDef.get('accelerationDesired'):
            ByteData[iByte:iByte+4] = struct.pack('f', VehData['accelerationDesired'])
            iByte += 4

        if self.VehicleMessageFieldDef.get('hasPrecedingVehicle'):
            ByteData[iByte] = VehData['hasPrecedingVehicle']
            iByte += 1

        if self.VehicleMessageFieldDef.get('precedingVehicleId'):
            ByteData, iByte = MsgHelper.pack_string(ByteData, iByte, VehData, 'precedingVehicleId')

        if self.VehicleMessageFieldDef.get('precedingVehicleDistance'):
            ByteData[iByte:iByte+4] = struct.pack('f', VehData['precedingVehicleDistance'])
            iByte += 4

        if self.VehicleMessageFieldDef.get('precedingVehicleSpeed'):
            ByteData[iByte:iByte+4] = struct.pack('f', VehData['precedingVehicleSpeed'])
            iByte += 4

        if self.VehicleMessageFieldDef.get('signalLightId'):
            ByteData, iByte = MsgHelper.pack_string(ByteData, iByte, VehData, 'signalLightId')

        if self.VehicleMessageFieldDef.get('signalLightHeadId'):
            ByteData[iByte:iByte+4] = struct.pack('i', VehData['signalLightHeadId'])
            iByte += 4

        if self.VehicleMessageFieldDef.get('signalLightDistance'):
            ByteData[iByte:iByte+4] = struct.pack('f', VehData['signalLightDistance'])
            iByte += 4

        if self.VehicleMessageFieldDef.get('signalLightColor'):
            ByteData[iByte] = VehData['signalLightColor']
            iByte += 1

        if self.VehicleMessageFieldDef.get('speedLimit'):
            ByteData[iByte:iByte+4] = struct.pack('f', VehData['speedLimit'])
            iByte += 4

        if self.VehicleMessageFieldDef.get('speedLimitNext'):
            ByteData[iByte:iByte+4] = struct.pack('f', VehData['speedLimitNext'])
            iByte += 4

        if self.VehicleMessageFieldDef.get('speedLimitChangeDistance'):
            ByteData[iByte:iByte+4] = struct.pack('f', VehData['speedLimitChangeDistance'])
            iByte += 4

        if self.VehicleMessageFieldDef.get('linkIdNext'):
            ByteData, iByte = MsgHelper.pack_string(ByteData, iByte, VehData, 'linkIdNext')

        if self.VehicleMessageFieldDef.get('grade'):
            ByteData[iByte:iByte+4] = struct.pack('f', VehData['grade'])
            iByte += 4

        if self.VehicleMessageFieldDef.get('activeLaneChange'):
            ByteData[iByte] = VehData['activeLaneChange']
            iByte += 1

        return ByteData, nMsgSize

    
    @ staticmethod
    def depack_string(ByteData: bytes, iByte: int):
    
        # Read the length of the string
        strLen = struct.unpack('B', ByteData[iByte:iByte+1])[0]
        if strLen > 50:
            raise ValueError("Max characters of id, linkId, type, precedingVehicleId, linkIdNext must be smaller than 50")

        iByte += 1

        # Initialize a string of size 50 and a uint8 array of size 50
        str_data = [''] * 50
        uint8Arr = [0] * 50

        # Read characters from ByteData according to strLen
        for i in range(strLen):
            byte_value = ByteData[iByte]
            str_data[i] = chr(byte_value)  # Convert byte to character
            uint8Arr[i] = byte_value
            iByte += 1

        # Convert list of characters to a single string
        str_data = ''.join(str_data[:strLen])

        return str_data, strLen, iByte, uint8Arr
    
    @ staticmethod
    def pack_string(ByteData: bytes, iByte: int, VehData: dict, messageField: str):
        # Get the length of the string field
        str_len = VehData.get(f"{messageField}Length", 0)

        # Pack the length of the string as a single byte
        ByteData[iByte] = str_len
        iByte += 1

        # Pack the string itself up to str_len
        ByteData[iByte:iByte+str_len] = VehData[messageField][:str_len]
        iByte += str_len

        return ByteData, iByte
    


def test_pack_simple_message():
    msg_helper = MsgHelper()

    # Set fields to be packed
    msg_helper.VehicleMessageFieldDef['id'] = True
    msg_helper.VehicleMessageFieldDef['speed'] = True

    # Sample VehData
    VehData = {
        'id': b'vehicle123',  # ID as bytes
        'idLength': len(b'vehicle123'),
        'speed': 88.5  # Speed as float
    }

    # Allocate ByteData buffer
    ByteData = bytearray(100)  # Pre-allocate buffer size

    # Pack data
    packed_data, nMsgSize = msg_helper.pack_veh_data(simState=1, t=123.456, ByteData=ByteData, VehData=VehData)

    print("Packed ByteData:", packed_data[:nMsgSize])
    print("Packed message size:", nMsgSize)

    VehData = {}
    unpacked_data = msg_helper.depack_veh_data(VehData, packed_data)

    print("Unpacked VehData:", unpacked_data)
    print(bytes(unpacked_data['id'][: unpacked_data['idLength']]).decode('utf-8'))

    

test_pack_simple_message()