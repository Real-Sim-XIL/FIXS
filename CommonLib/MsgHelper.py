from re import M
from typing import List
from CommonLib.VehDataMsgDefs import VehData
import struct


class MessageType:
    vehicle_data = 1
    traffic_light_data = 2
    detector_data = 3



class MsgHelper:
    
    def __init__(self):
        self.vehicle_msg_field_valid = {
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
        self.msg_header_size = 9
        self.msg_each_header_size = 3
        # debugging function
        # vehicle data message
        # self.vehicle_data_empty = VehDataMsgDefs.VehData()
        # Vehiclemessage_fieldDefInputVec = zeros(1, 29); % Vector of 0,1 to select which message field will be transmitted
        
    
    def sef_vehicle_message_field(self, vehicle_msg_field: List[str]):
        for field in vehicle_msg_field:
            self.vehicle_msg_field_valid[field] = True

    @ staticmethod
    def unpack_float(data, index):
            value = struct.unpack('f', data[index:index+4])[0]
            return value, index + 4
    # Helper function to unpack a single int32
    @ staticmethod
    def unpack_int32(data, index):
        value = struct.unpack('i', data[index:index+4])[0]
        return value, index + 4
    # Helper function to unpack a single uint32
    @ staticmethod
    def unpack_uint32(data, index):
        value = struct.unpack('I', data[index:index+4])[0]
        return value, index + 4
    @ staticmethod
    def unpack_uint16(data, index):
        value = struct.unpack('H', data[index:index+2])[0]
        return value, index + 2
    @ staticmethod
    def unpack_uint8(data, index):
        value = struct.unpack('B', data[index:index+1])[0]
        return value, index + 1
    # Helper function to unpack a single int8
    @ staticmethod
    def unpack_int8(data, index):
        value = struct.unpack('b', data[index:index+1])[0]
        return value, index + 1
    
    def depack_veh_data(self, byte_data: bytes)-> VehData:
        veh_data = VehData()
        byte_index = 0  # Index in byte_data
        # byte_index += self.msg_header_size  # Skip the message header
        # byte_index += self.msg_each_header_size  # Skip the message type header
        # Helper function to unpack a single float
        
        # Unpack fields based on vehicle_msg_field_valid
        if self.vehicle_msg_field_valid.get('id'):
            str_data, str_len, byte_index, uint8Arr = MsgHelper.depack_string(byte_data, byte_index)
            veh_data.id = uint8Arr
            veh_data.idLength = str_len

        if self.vehicle_msg_field_valid.get('type'):
            str_data, str_len, byte_index, uint8Arr = MsgHelper.depack_string(byte_data, byte_index)
            veh_data.type = uint8Arr
            veh_data.typeLength = str_len

        if self.vehicle_msg_field_valid.get('vehicleClass'):
            str_data, str_len, byte_index, uint8Arr = MsgHelper.depack_string(byte_data, byte_index)
            veh_data.vehicleClass = uint8Arr
            veh_data.vehicleClassLength = str_len

        if self.vehicle_msg_field_valid.get('speed'):
            veh_data.speed, byte_index = MsgHelper.unpack_float(byte_data, byte_index)

        if self.vehicle_msg_field_valid.get('acceleration'):
            veh_data.acceleration, byte_index = MsgHelper.unpack_float(byte_data, byte_index)

        if self.vehicle_msg_field_valid.get('positionX'):
            veh_data.positionX, byte_index = MsgHelper.unpack_float(byte_data, byte_index)

        if self.vehicle_msg_field_valid.get('positionY'):
            veh_data.positionY, byte_index = MsgHelper.unpack_float(byte_data, byte_index)

        if self.vehicle_msg_field_valid.get('positionZ'):
            veh_data.positionZ, byte_index = MsgHelper.unpack_float(byte_data, byte_index)

        if self.vehicle_msg_field_valid.get('heading'):
            veh_data.heading, byte_index = MsgHelper.unpack_float(byte_data, byte_index)

        if self.vehicle_msg_field_valid.get('color'):
            veh_data.color, byte_index = MsgHelper.unpack_uint32(byte_data, byte_index)

        if self.vehicle_msg_field_valid.get('linkId'):
            str_data, str_len, byte_index, uint8Arr = MsgHelper.depack_string(byte_data, byte_index)
            veh_data.linkId = uint8Arr
            veh_data.linkIdLength = str_len

        if self.vehicle_msg_field_valid.get('laneId'):
            veh_data.laneId, byte_index = MsgHelper.unpack_int32(byte_data, byte_index)

        if self.vehicle_msg_field_valid.get('distanceTravel'):
            veh_data.distanceTravel, byte_index = MsgHelper.unpack_float(byte_data, byte_index)

        if self.vehicle_msg_field_valid.get('speedDesired'):
            veh_data.speedDesired, byte_index = MsgHelper.unpack_float(byte_data, byte_index)

        if self.vehicle_msg_field_valid.get('accelerationDesired'):
            veh_data.accelerationDesired, byte_index = MsgHelper.unpack_float(byte_data, byte_index)

        if self.vehicle_msg_field_valid.get('hasPrecedingVehicle'):
            veh_data.hasPrecedingVehicle, byte_index = MsgHelper.unpack_int8(byte_data, byte_index)

        if self.vehicle_msg_field_valid.get('precedingVehicleId'):
            str_data, str_len, byte_index, uint8Arr = MsgHelper.depack_string(byte_data, byte_index)
            veh_data.precedingVehicleId = uint8Arr
            veh_data.precedingVehicleIdLength = str_len

        if self.vehicle_msg_field_valid.get('precedingVehicleDistance'):
            veh_data.precedingVehicleDistance, byte_index = MsgHelper.unpack_float(byte_data, byte_index)

        if self.vehicle_msg_field_valid.get('precedingVehicleSpeed'):
            veh_data.precedingVehicleSpeed, byte_index = MsgHelper.unpack_float(byte_data, byte_index)

        if self.vehicle_msg_field_valid.get('signalLightId'):
            str_data, str_len, byte_index, uint8Arr = MsgHelper.depack_string(byte_data, byte_index)
            veh_data.signalLightId = uint8Arr
            veh_data.signalLightIdLength = str_len

        if self.vehicle_msg_field_valid.get('signalLightHeadId'):
            veh_data.signalLightHeadId, byte_index = MsgHelper.unpack_int32(byte_data, byte_index)

        if self.vehicle_msg_field_valid.get('signalLightDistance'):
            veh_data.signalLightDistance, byte_index = MsgHelper.unpack_float(byte_data, byte_index)

        if self.vehicle_msg_field_valid.get('signalLightColor'):
            veh_data.signalLightColor, byte_index = MsgHelper.unpack_int8(byte_data, byte_index)

        if self.vehicle_msg_field_valid.get('speedLimit'):
            veh_data.speedLimit, byte_index = MsgHelper.unpack_float(byte_data, byte_index)

        if self.vehicle_msg_field_valid.get('speedLimitNext'):
            veh_data.speedLimitNext, byte_index = MsgHelper.unpack_float(byte_data, byte_index)

        if self.vehicle_msg_field_valid.get('speedLimitChangeDistance'):
            veh_data.speedLimitChangeDistance, byte_index = MsgHelper.unpack_float(byte_data, byte_index)

        if self.vehicle_msg_field_valid.get('linkIdNext'):
            str_data, str_len, byte_index, uint8Arr = MsgHelper.depack_string(byte_data, byte_index)
            veh_data.linkIdNext = uint8Arr
            veh_data.linkIdNextLength = str_len

        if self.vehicle_msg_field_valid.get('grade'):
            veh_data.grade, byte_index = MsgHelper.unpack_float(byte_data, byte_index)

        if self.vehicle_msg_field_valid.get('activeLaneChange'):
            veh_data.activeLaneChange, byte_index = MsgHelper.unpack_int8(byte_data, byte_index)

        return  veh_data


    def pack_veh_data(self, byte_data: bytearray, byte_index, veh_data: VehData):
        # Calculate nMsgSize based on vehicle_msg_field_valid flags and veh_data field lengths
        
        
        msg_size = (
            round(self.vehicle_msg_field_valid.get('id', 0) * (1 + veh_data.get('idLength', 0))
                  + self.vehicle_msg_field_valid.get('type', 0) * (1 + veh_data.get('typeLength', 0))
                  + self.vehicle_msg_field_valid.get('vehicleClass', 0) * (1 + veh_data.get('vehicleClassLength', 0))
                  + self.vehicle_msg_field_valid.get('speed', 0) * 4  # speed
                  + self.vehicle_msg_field_valid.get('acceleration', 0) * 4  # acceleration
                  + self.vehicle_msg_field_valid.get('positionX', 0) * 4  # positionX
                  + self.vehicle_msg_field_valid.get('positionY', 0) * 4  # positionY
                  + self.vehicle_msg_field_valid.get('positionZ', 0) * 4  # positionZ
                  + self.vehicle_msg_field_valid.get('heading', 0) * 4  # heading
                  + self.vehicle_msg_field_valid.get('color', 0) * 4  # color
                  + self.vehicle_msg_field_valid.get('linkId', 0) * (1 + veh_data.get('linkIdLength', 0))  # linkId
                  + self.vehicle_msg_field_valid.get('laneId', 0) * 4  # laneId
                  + self.vehicle_msg_field_valid.get('distanceTravel', 0) * 4  # distanceTravel
                  + self.vehicle_msg_field_valid.get('speedDesired', 0) * 4  # speedDesired
                  + self.vehicle_msg_field_valid.get('accelerationDesired', 0) * 4  # accelerationDesired
                  + self.vehicle_msg_field_valid.get('hasPrecedingVehicle', 0) * 1  # hasPrecedingVehicle
                  + self.vehicle_msg_field_valid.get('precedingVehicleId', 0) * (1 + veh_data.get('precedingVehicleIdLength', 0))  # precedingVehicleId
                  + self.vehicle_msg_field_valid.get('precedingVehicleDistance', 0) * 4  # precedingVehicleDistance
                  + self.vehicle_msg_field_valid.get('precedingVehicleSpeed', 0) * 4  # precedingVehicleSpeed
                  + self.vehicle_msg_field_valid.get('signalLightId', 0) * (1 + veh_data.get('signalLightIdLength', 0))  # signalLightId
                  + self.vehicle_msg_field_valid.get('signalLightHeadId', 0) * 4  # signalLightHeadId
                  + self.vehicle_msg_field_valid.get('signalLightDistance', 0) * 4  # signalLightDistance
                  + self.vehicle_msg_field_valid.get('signalLightColor', 0) * 1  # signalLightColor
                  + self.vehicle_msg_field_valid.get('speedLimit', 0) * 4  # speedLimit
                  + self.vehicle_msg_field_valid.get('speedLimitNext', 0) * 4  # speedLimitNext
                  + self.vehicle_msg_field_valid.get('speedLimitChangeDistance', 0) * 4  # speedLimitChangeDistance
                  + self.vehicle_msg_field_valid.get('linkIdNext', 0) * (1 + veh_data.get('linkIdNextLength', 0))  # linkIdNext
                  + self.vehicle_msg_field_valid.get('grade', 0) * 4  # grade
                  + self.vehicle_msg_field_valid.get('activeLaneChange', 0) * 1  # activeLaneChange
            )
        )
        veh_msg_size = round(msg_size) + self.msg_each_header_size
        
        # Pack message size as uint16, 2 bytes
        byte_data[byte_index:byte_index+2] = struct.pack('H', veh_msg_size)
        byte_index += 2
        # Pack message type as uint8, 1 byte
        byte_data[byte_index] = MessageType.vehicle_data
        byte_index += 1
        

        # Pack fields conditionally based on vehicle_msg_field_valid
        if self.vehicle_msg_field_valid.get('id'):
            byte_data, byte_index = MsgHelper.pack_string(byte_data, byte_index, veh_data, 'id')

        if self.vehicle_msg_field_valid.get('type'):
            byte_data, byte_index = MsgHelper.pack_string(byte_data, byte_index, veh_data, 'type')

        if self.vehicle_msg_field_valid.get('vehicleClass'):
            byte_data, byte_index = MsgHelper.pack_string(byte_data, byte_index, veh_data, 'vehicleClass')

        if self.vehicle_msg_field_valid.get('speed'):
            byte_data[byte_index:byte_index+4] = struct.pack('f', veh_data.speed)
            byte_index += 4

        if self.vehicle_msg_field_valid.get('acceleration'):
            byte_data[byte_index:byte_index+4] = struct.pack('f', veh_data.acceleration)
            byte_index += 4

        if self.vehicle_msg_field_valid.get('positionX'):
            byte_data[byte_index:byte_index+4] = struct.pack('f', veh_data.positionX)
            byte_index += 4

        if self.vehicle_msg_field_valid.get('positionY'):
            byte_data[byte_index:byte_index+4] = struct.pack('f', veh_data.positionY)
            byte_index += 4

        if self.vehicle_msg_field_valid.get('positionZ'):
            byte_data[byte_index:byte_index+4] = struct.pack('f', veh_data.positionZ)
            byte_index += 4

        if self.vehicle_msg_field_valid.get('heading'):
            byte_data[byte_index:byte_index+4] = struct.pack('f', veh_data.heading)
            byte_index += 4

        if self.vehicle_msg_field_valid.get('color'):
            byte_data[byte_index:byte_index+4] = struct.pack('I', veh_data.color)
            byte_index += 4

        if self.vehicle_msg_field_valid.get('linkId'):
            byte_data, byte_index = MsgHelper.pack_string(byte_data, byte_index, veh_data, 'linkId')

        if self.vehicle_msg_field_valid.get('laneId'):
            byte_data[byte_index:byte_index+4] = struct.pack('i', veh_data.laneId)
            byte_index += 4

        if self.vehicle_msg_field_valid.get('distanceTravel'):
            byte_data[byte_index:byte_index+4] = struct.pack('f', veh_data.distanceTravel)
            byte_index += 4

        if self.vehicle_msg_field_valid.get('speedDesired'):
            byte_data[byte_index:byte_index+4] = struct.pack('f', veh_data.speedDesired)
            byte_index += 4

        if self.vehicle_msg_field_valid.get('accelerationDesired'):
            byte_data[byte_index:byte_index+4] = struct.pack('f', veh_data.accelerationDesired)
            byte_index += 4

        if self.vehicle_msg_field_valid.get('hasPrecedingVehicle'):
            byte_data[byte_index] = veh_data.hasPrecedingVehicle
            byte_index += 1

        if self.vehicle_msg_field_valid.get('precedingVehicleId'):
            byte_data, byte_index = MsgHelper.pack_string(byte_data, byte_index, veh_data, 'precedingVehicleId')

        if self.vehicle_msg_field_valid.get('precedingVehicleDistance'):
            byte_data[byte_index:byte_index+4] = struct.pack('f', veh_data.precedingVehicleDistance)
            byte_index += 4

        if self.vehicle_msg_field_valid.get('precedingVehicleSpeed'):
            byte_data[byte_index:byte_index+4] = struct.pack('f', veh_data.precedingVehicleSpeed)
            byte_index += 4

        if self.vehicle_msg_field_valid.get('signalLightId'):
            byte_data, byte_index = MsgHelper.pack_string(byte_data, byte_index, veh_data, 'signalLightId')

        if self.vehicle_msg_field_valid.get('signalLightHeadId'):
            byte_data[byte_index:byte_index+4] = struct.pack('i', veh_data.signalLightHeadId)
            byte_index += 4

        if self.vehicle_msg_field_valid.get('signalLightDistance'):
            byte_data[byte_index:byte_index+4] = struct.pack('f', veh_data.signalLightDistance)
            byte_index += 4

        if self.vehicle_msg_field_valid.get('signalLightColor'):
            byte_data[byte_index] = veh_data.signalLightColor
            byte_index += 1

        if self.vehicle_msg_field_valid.get('speedLimit'):
            byte_data[byte_index:byte_index+4] = struct.pack('f', veh_data.speedLimit)
            byte_index += 4

        if self.vehicle_msg_field_valid.get('speedLimitNext'):
            byte_data[byte_index:byte_index+4] = struct.pack('f', veh_data.speedLimitNext)
            byte_index += 4

        if self.vehicle_msg_field_valid.get('speedLimitChangeDistance'):
            byte_data[byte_index:byte_index+4] = struct.pack('f', veh_data.speedLimitChangeDistance)
            byte_index += 4

        if self.vehicle_msg_field_valid.get('linkIdNext'):
            byte_data, byte_index = MsgHelper.pack_string(byte_data, byte_index, veh_data, 'linkIdNext')

        if self.vehicle_msg_field_valid.get('grade'):
            byte_data[byte_index:byte_index+4] = struct.pack('f', veh_data.grade)
            byte_index += 4

        if self.vehicle_msg_field_valid.get('activeLaneChange'):
            byte_data[byte_index] = veh_data.activeLaneChange
            byte_index += 1

        return byte_data, msg_size, byte_index



    def pack_msg_header(self, byte_data: bytearray, simulation_state: int, t: float, total_msg_size: int):
        byte_index = 0
        # Pack simulation_state as uint8
        byte_data[byte_index] = simulation_state
        byte_index += 1

        # Pack t as float
        byte_data[byte_index:byte_index+4] = struct.pack('f', t)
        byte_index += 4

        # Pack total_msg_size as uint32
        byte_data[byte_index:byte_index+4] = struct.pack('I', total_msg_size)
        byte_index += 4

        return byte_data, byte_index


    # the primary level header of the message
    def depack_msg_header(self, byte_data: bytearray):

        byte_index = 0
        sim_state, byte_index = MsgHelper.unpack_uint8(byte_data, byte_index)
        sim_time, byte_index = MsgHelper.unpack_float(byte_data, byte_index)
        total_msg_size, byte_index = MsgHelper.unpack_uint32(byte_data, byte_index)

        return sim_state, sim_time, total_msg_size
    




    # the secondary level header of each message
    def depack_msg_type(self, byte_data: bytearray):

        byte_index = 0
        msg_size, byte_index = MsgHelper.unpack_uint16(byte_data, byte_index)
        msg_type, byte_index = MsgHelper.unpack_int8(byte_data, byte_index)
        
        return msg_size, msg_type
    





    @ staticmethod
    def depack_string(byte_data: bytes, byte_index: int):
    
        # Read the length of the string
        strLen = struct.unpack('B', byte_data[byte_index:byte_index+1])[0]
        if strLen > 50:
            raise ValueError("Max characters of id, linkId, type, precedingVehicleId, linkIdNext must be smaller than 50")

        byte_index += 1

        # Initialize a string of size 50 and a uint8 array of size 50
        str_data = [''] * 50
        uint8Arr = [0] * 50

        # Read characters from byte_data according to strLen
        for i in range(strLen):
            byte_value = byte_data[byte_index]
            str_data[i] = chr(byte_value)  # Convert byte to character
            uint8Arr[i] = byte_value
            byte_index += 1

        # Convert list of characters to a single string
        str_data = ''.join(str_data[:strLen])

        return str_data, strLen, byte_index, uint8Arr
    
    @ staticmethod
    def pack_string(byte_data: bytes, byte_index: int, veh_data: VehData, message_field: str):
        # Get the length of the string field
        str_len = veh_data.get(f"{message_field}Length", 0)

        # Pack the length of the string as a single byte
        byte_data[byte_index] = str_len
        byte_index += 1

        # Pack the string itself up to str_len
        byte_data[byte_index:byte_index+str_len] = veh_data.get(message_field)[:str_len]
        byte_index += str_len

        return byte_data, byte_index
    
    @ staticmethod
    def create_vehicle_data():
        veh_data = VehData()
        veh_data.id = b'vehicle123'
        veh_data.idLength = len(b'vehicle123')
        veh_data.speed = 88.5
        return veh_data


def test_pack_simple_message():
    msg_helper = MsgHelper()

    # Set fields to be packed
    msg_helper.vehicle_msg_field_valid['id'] = True
    msg_helper.vehicle_msg_field_valid['speed'] = True

    # Sample veh_data
    veh_data = VehData()
    veh_data.id = b'vehicle123'
    veh_data.idLength = len(b'vehicle123')
    veh_data.speed = 88.5

    # Allocate byte_data buffer
    byte_data = bytearray(100)  # Pre-allocate buffer size

    # Pack data
    packed_data, nMsgSize = msg_helper.pack_veh_data(byte_data=byte_data, veh_data=veh_data)

    print("Packed byte_data:", packed_data[:nMsgSize])
    print("Packed message size:", nMsgSize)

    msg_size, msg_type  = msg_helper.depack_msg_type(packed_data)
    print("Unpacked msg_type:", msg_type)
    print("Unpacked msg_size:", msg_size)
    # veh_data = {}
    unpacked_data = msg_helper.depack_veh_data(packed_data[msg_helper.msg_each_header_size:])

    # print("Unpacked veh_data:", unpacked_data)
    print(bytes(unpacked_data.id[: unpacked_data.idLength]).decode('utf-8'))

    

# test_pack_simple_message()