from dataclasses import dataclass, field
import ctypes
from typing import List

@dataclass
class VehData:
    # Fixed-size arrays for string-like fields (50 bytes each)
    id: List[int] = field(default_factory=lambda: [0] * 50)
    idLength: int = 0
    type: List[int] = field(default_factory=lambda: [0] * 50)
    typeLength: int = 0
    vehicleClass: List[int] = field(default_factory=lambda: [0] * 50)
    vehicleClassLength: int = 0
    
    # Floating point and integer fields
    speed: float = 0.0
    acceleration: float = 0.0
    positionX: float = 0.0
    positionY: float = 0.0
    positionZ: float = 0.0
    heading: float = 0.0
    color: int = 0  # Assuming color is an integer representation (e.g., ARGB or RGB)
    
    linkId: List[int] = field(default_factory=lambda: [0] * 50)
    linkIdLength: int = 0
    laneId: int = 0
    distanceTravel: float = 0.0
    speedDesired: float = 0.0
    accelerationDesired: float = 0.0
    
    hasPrecedingVehicle: int = 0  # Boolean-like integer (0 or 1)
    precedingVehicleId: List[int] = field(default_factory=lambda: [0] * 50)
    precedingVehicleIdLength: int = 0
    precedingVehicleDistance: float = 0.0
    precedingVehicleSpeed: float = 0.0
    
    signalLightId: List[int] = field(default_factory=lambda: [0] * 50)
    signalLightIdLength: int = 0
    signalLightHeadId: int = 0
    signalLightDistance: float = 0.0
    signalLightColor: int = 0  # Assuming an integer representation of the color
    
    speedLimit: float = 0.0
    speedLimitNext: float = 0.0
    speedLimitChangeDistance: float = 0.0
    
    linkIdNext: List[int] = field(default_factory=lambda: [0] * 50)
    linkIdNextLength: int = 0
    grade: float = 0.0
    activeLaneChange: int = 0  # Boolean-like integer (-1, 0, or 1)

    def get(self, field_name, default=None):
        return getattr(self, field_name, default)
    