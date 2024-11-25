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
        self.simulation_setup = defaultdict(lambda: None)
        self.application_setup = defaultdict(lambda: None)
        self.Xil_setup = defaultdict(lambda: None)
        self.CarMaker_setup = defaultdict(lambda: None)
        self.Sumo_setup = defaultdict(lambda: None)
        
    def getConfig(self, configName):
        path = os.path.normpath(configName)
        with open(path, 'r') as file:
            config = yaml.safe_load(file)

        # Simulation Setup
        simulation_node = config.get("SimulationSetup", {})
        self.simulation_setup["EnableRealSim"] = self.parserFlag(simulation_node, "EnableRealSim", True)
        self.simulation_setup["EnableVerboseLog"] = self.parserFlag(simulation_node, "EnableVerboseLog", False)
        self.simulation_setup["SimulationEndTime"] = self.parserDouble(simulation_node, "SimulationEndTime", 90000)
        self.simulation_setup["EnableExternalDynamics"] = self.parserFlag(simulation_node, "EnableExternalDynamics", False)
        self.simulation_setup["SelectedTrafficSimulator"] = self.parserString(simulation_node, "SelectedTrafficSimulator", "SUMO")
        self.simulation_setup["TrafficSimulatorIP"] = self.parserString(simulation_node, "TrafficSimulatorIP", "127.0.0.1")
        self.simulation_setup["TrafficSimulatorPort"] = self.parserInteger(simulation_node, "TrafficSimulatorPort", 1337)
        self.simulation_setup["SimulationMode"] = self.parserInteger(simulation_node, "SimulationMode", 0)
        self.simulation_setup["SimulationModeParameter"] = self.parserDouble(simulation_node, "SimulationModeParameter", 0)
        self.simulation_setup["VehicleMessageField"] = self.parserStringVector(simulation_node, "VehicleMessageField", ["id", "type", "speed", "positionX", "positionY"])
        # Sumo Setup
        sumo_node = config.get("SumoSetup", {})
        self.Sumo_setup["SpeedMode"] = sumo_node.get("SpeedMode", 0)
        # Application Setup
        app_node = config.get("ApplicationSetup", {})
        self.application_setup["EnableApplicationLayer"] = self.parserFlag(app_node, "EnableApplicationLayer", False)
        self.application_setup["VehicleSubscription"] = self.parseVehicleSubscription(app_node, "VehicleSubscription", [])

        # Xil Setup
        xil_node = config.get("XilSetup", {})
        self.Xil_setup["EnableXil"] = xil_node.get("EnableXil", False)
        self.Xil_setup["VehicleSubscription"] = self.parseVehicleSubscription(xil_node, "VehicleSubscription", [])

        return 0

    def resetConfig(self):
        # Clear all config settings
        self.simulation_setup.clear()
        self.application_setup.clear()
        self.Xil_setup.clear()
        self.CarMaker_setup.clear()

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



if __name__ == "__main__":
    config_helper = ConfigHelper()
    CONFIG_PATH = r"C:\Users\hg25079\Documents\GitHub\FIXS\tests\Applications\Ecodriving\ecodrivingConfig.yaml"
    config_helper.getConfig(CONFIG_PATH)
