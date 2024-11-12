from dataclasses import dataclass, field
import ctypes
from typing import List

@dataclass
class VehFullData_t:
    # Fixed-size uint8 arrays (50 bytes each) for string-like fields
    id: List[ctypes.c_uint8] = field(default_factory=lambda: [ctypes.c_uint8(0)] * 50)
    idLength: ctypes.c_uint8 = ctypes.c_uint8(0)
    type: List[ctypes.c_uint8] = field(default_factory=lambda: [ctypes.c_uint8(0)] * 50)
    typeLength: ctypes.c_uint8 = ctypes.c_uint8(0)
    vehicleClass: List[ctypes.c_uint8] = field(default_factory=lambda: [ctypes.c_uint8(0)] * 50)
    vehicleClassLength: ctypes.c_uint8 = ctypes.c_uint8(0)
    
    # Floating point and integer fields
    speed: ctypes.c_float = ctypes.c_float(0.0)
    acceleration: ctypes.c_float = ctypes.c_float(0.0)
    positionX: ctypes.c_float = ctypes.c_float(0.0)
    positionY: ctypes.c_float = ctypes.c_float(0.0)
    positionZ: ctypes.c_float = ctypes.c_float(0.0)
    heading: ctypes.c_float = ctypes.c_float(0.0)
    color: ctypes.c_uint32 = ctypes.c_uint32(0)
    
    linkId: List[ctypes.c_uint8] = field(default_factory=lambda: [ctypes.c_uint8(0)] * 50)
    linkIdLength: ctypes.c_uint8 = ctypes.c_uint8(0)
    laneId: ctypes.c_int32 = ctypes.c_int32(0)
    distanceTravel: ctypes.c_float = ctypes.c_float(0.0)
    speedDesired: ctypes.c_float = ctypes.c_float(0.0)
    accelerationDesired: ctypes.c_float = ctypes.c_float(0.0)
    
    hasPrecedingVehicle: ctypes.c_uint8 = ctypes.c_uint8(0)
    precedingVehicleId: List[ctypes.c_uint8] = field(default_factory=lambda: [ctypes.c_uint8(0)] * 50)
    precedingVehicleIdLength: ctypes.c_uint8 = ctypes.c_uint8(0)
    precedingVehicleDistance: ctypes.c_float = ctypes.c_float(0.0)
    precedingVehicleSpeed: ctypes.c_float = ctypes.c_float(0.0)
    
    signalLightId: List[ctypes.c_uint8] = field(default_factory=lambda: [ctypes.c_uint8(0)] * 50)
    signalLightIdLength: ctypes.c_uint8 = ctypes.c_uint8(0)
    signalLightHeadId: ctypes.c_int32 = ctypes.c_int32(0)
    signalLightDistance: ctypes.c_float = ctypes.c_float(0.0)
    signalLightColor: ctypes.c_uint8 = ctypes.c_uint8(0)
    
    speedLimit: ctypes.c_float = ctypes.c_float(0.0)
    speedLimitNext: ctypes.c_float = ctypes.c_float(0.0)
    speedLimitChangeDistance: ctypes.c_float = ctypes.c_float(0.0)
    
    linkIdNext: List[ctypes.c_uint8] = field(default_factory=lambda: [ctypes.c_uint8(0)] * 50)
    linkIdNextLength: ctypes.c_uint8 = ctypes.c_uint8(0)
    grade: ctypes.c_float = ctypes.c_float(0.0)
    activeLaneChange: ctypes.c_int8 = ctypes.c_int8(0)

    