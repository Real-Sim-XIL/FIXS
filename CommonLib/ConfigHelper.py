import yaml
import os
from collections import defaultdict

# Enum-like mapping
class TypeNamesEnum:
    ego = 0
    link = 1
    point = 2
    vehicleType = 3
    intersection = 4
    detector = 5

# Map to associate the strings with enum values
s_mapTypeValues = {
    "ego": TypeNamesEnum.ego,
    "link": TypeNamesEnum.link,
    "point": TypeNamesEnum.point,
    "vehicleType": TypeNamesEnum.vehicleType,
    "intersection": TypeNamesEnum.intersection,
    "detector": TypeNamesEnum.detector
}

class ConfigHelper:
    def __init__(self):
        # Initialize the s_mapTypeValues
        self.SimulationSetup = defaultdict(lambda: None)
        self.ApplicationSetup = defaultdict(lambda: None)
        self.XilSetup = defaultdict(lambda: None)
        self.CarMakerSetup = defaultdict(lambda: None)
        self.SumoSetup = defaultdict(lambda: None)
        
    def getConfig(self, configName):
        path = os.path.normpath(configName)
        with open(path, 'r') as file:
            config = yaml.safe_load(file)

        # Simulation Setup
        simulation_node = config.get("SimulationSetup", {})
        self.SimulationSetup["EnableRealSim"] = self.parserFlag(simulation_node, "EnableRealSim", True)
        self.SimulationSetup["EnableVerboseLog"] = self.parserFlag(simulation_node, "EnableVerboseLog", False)
        self.SimulationSetup["SimulationEndTime"] = self.parserDouble(simulation_node, "SimulationEndTime", 90000)
        self.SimulationSetup["EnableExternalDynamics"] = self.parserFlag(simulation_node, "EnableExternalDynamics", False)
        self.SimulationSetup["SelectedTrafficSimulator"] = self.parserString(simulation_node, "SelectedTrafficSimulator", "SUMO")
        self.SimulationSetup["TrafficSimulatorIP"] = self.parserString(simulation_node, "TrafficSimulatorIP", "127.0.0.1")
        self.SimulationSetup["TrafficSimulatorPort"] = self.parserInteger(simulation_node, "TrafficSimulatorPort", 1337)
        self.SimulationSetup["SimulationMode"] = self.parserInteger(simulation_node, "SimulationMode", 0)
        self.SimulationSetup["SimulationModeParameter"] = self.parserDouble(simulation_node, "SimulationModeParameter", 0)
        self.SimulationSetup["VehicleMessageField"] = self.parserStringVector(simulation_node, "VehicleMessageField", ["id", "type", "speed", "positionX", "positionY"])
        # Sumo Setup
        sumo_node = config.get("SumoSetup", {})
        self.SumoSetup["SpeedMode"] = sumo_node.get("SpeedMode", 0)
        # Application Setup
        app_node = config.get("ApplicationSetup", {})
        self.ApplicationSetup["EnableApplicationLayer"] = self.parserFlag(app_node, "EnableApplicationLayer", False)
        self.ApplicationSetup["VehicleSubscription"] = self.parseVehicleSubscription(app_node, "VehicleSubscription", [])

        # Xil Setup
        xil_node = config.get("XilSetup", {})
        self.XilSetup["EnableXil"] = xil_node.get("EnableXil", False)
        self.XilSetup["VehicleSubscription"] = self.parseVehicleSubscription(xil_node, "VehicleSubscription", [])

        return 0

    def resetConfig(self):
        # Clear all config settings
        self.SimulationSetup.clear()
        self.ApplicationSetup.clear()
        self.XilSetup.clear()
        self.CarMakerSetup.clear()

    def parserFlag(self, node, name, default=False):
        return node.get(name, default) in ['true', True]

    def parserString(self, node, name, default=""):
        return node.get(name, default)

    def parserDouble(self, node, name, default=0.0):
        return float(node.get(name, default))

    def parserInteger(self, node, name, default=0):
        return int(node.get(name, default))

    def parserStringVector(self, node, name, default):
        return node.get(name, default)


    def parseVehicleSubscription(self, node, name, default=[]):
        # Process each subscription in the list
        subscription_list = node.get(name, default)
        if subscription_list is None:
            return []
        parsed_subscriptions = []
        for sub in subscription_list:
            parsed_subscription = {
                "type": sub.get("type"),
                "attribute": sub.get("attribute", {}),
                "ip": sub.get("ip", []),
                "port": sub.get("port", [])
            }
            parsed_subscriptions.append(parsed_subscription)
        return parsed_subscriptions




# Example Usage
config_helper = ConfigHelper()
config_helper.getConfig(r"C:\Users\hg25079\Documents\GitHub\FIXS\tests\Applications\Ecodriving\ecodrivingConfig.yaml")
