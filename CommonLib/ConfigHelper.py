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
        # #305 the ego, described once, for every backend
        self.Ego_setup = defaultdict(lambda: None)
        self.Sumo_setup = defaultdict(lambda: None)
        self.Carla_setup = defaultdict(lambda: None)
        self.DataLog_setup = defaultdict(lambda: None)
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
        # Warm-up (#86), mirroring ConfigHelper.cpp. Replaced SimulationMode /
        # SimulationModeParameter; see doc/ConfigSetup.md for the mapping.
        self.simulation_setup["WarmUpUntilEgoEntry"] = self.parserFlag(simulation_node, "WarmUpUntilEgoEntry", False)
        self.simulation_setup["WarmUpTime"] = self.parserDouble(simulation_node, "WarmUpTime", 0)
        self.simulation_setup["VehicleMessageField"] = self.parserStringVector(simulation_node, "VehicleMessageField", ["id", "type", "speed", "positionX", "positionY"])
        # Sumo Setup
        sumo_node = config.get("SumoSetup", {})
        self.Sumo_setup["SpeedMode"] = sumo_node.get("SpeedMode", 0)
        # Consumed by run_cosim.py (not the C++ engine): whether it launches SUMO
        # itself, or waits for the user to start it. Parity with SumoSetup.AutoStart
        # in ConfigHelper.cpp. Default true.
        self.Sumo_setup["AutoStart"] = self.parserFlag(sumo_node, "AutoStart", True)
        # How many TraCI clients SUMO waits for before it steps. Mirrors
        # SumoSetup.NumClients in ConfigHelper.cpp (same default). run_cosim passes
        # it to SUMO as --num-clients; without it parsed here the key was read back
        # as None on the python side, so the yaml value silently stayed 1.
        self.Sumo_setup["NumClients"] = self.parserInteger(sumo_node, "NumClients", 1)
        # Application Setup
        app_node = config.get("ApplicationSetup", {})
        self.application_setup["EnableApplicationLayer"] = self.parserFlag(app_node, "EnableApplicationLayer", False)
        self.application_setup["VehicleSubscription"] = self.parseVehicleSubscription(app_node, "VehicleSubscription", [])

        # Xil Setup
        xil_node = config.get("XilSetup", {})
        self.Xil_setup["EnableXil"] = self.parserFlag(xil_node, "EnableXil", False)
        self.Xil_setup["VehicleSubscription"] = self.parseVehicleSubscription(xil_node, "VehicleSubscription", [])

        # Carla Setup
        carla_node = config.get("CarlaSetup", {})
        self.Carla_setup["EnableVerboseLog"] = self.parserFlag(carla_node, "EnableVerboseLog", False)
        self.Carla_setup["EnableCosimulation"] = self.parserFlag(carla_node, "EnableCosimulation", True)
        # Co-sim bridge selector consumed by run_cosim.py (not the C++ engine):
        # true = run_synchronization.py, false = TrafficLayer + VirCarlaEnv. Parity
        # with CarlaSetup.EnablePythonBackend in ConfigHelper.cpp. Default true.
        self.Carla_setup["EnablePythonBackend"] = self.parserFlag(
            carla_node, "EnablePythonBackend", True)
        self.Carla_setup["CarlaServerIP"] = self.parserString(carla_node, "CarlaServerIP", "127.0.0.1")
        self.Carla_setup["CarlaServerPort"] = self.parserInteger(carla_node, "CarlaServerPort", 420)
        self.Carla_setup["CarlaClientIP"] = self.parserString(carla_node, "CarlaClientIP", "127.0.0.1")
        self.Carla_setup["CarlaClientPort"] = self.parserInteger(carla_node, "CarlaClientPort", 430)
        self.Carla_setup["CarlaMapName"] = self.parserString(carla_node, "CarlaMapName", "Town01")
        # 0 == every Carla tick. This key is the pose RE-APPLY cadence and, absent,
        # must not impose one: the bridge resolves 0 to CarlaTimeStep. The old 0.1
        # default here was a leftover from before #219, when this key WAS the feed
        # period; it silently pinned traffic to 10 Hz however fine the world step,
        # so with CarlaTimeStep 0.025 every vehicle held a stale pose for 3 of every
        # 4 ticks and jumped a whole feed of travel on the 4th. Matches
        # ConfigHelper.cpp; see #261.
        self.Carla_setup["TrafficRefreshRate"] = self.parserDouble(carla_node, "TrafficRefreshRate", 0.0)
        self.Carla_setup["InterestedIds"] = self.parserStringVector(carla_node, "InterestedIds", ["ego"])

        # ---- the rest of CarlaSetup, at parity with ConfigHelper.cpp -----------
        # The Python bridge (Carla/VirEnv) reads this block in full, so every key
        # the C++ VirCarlaEnv honours has to be parsed here with the SAME default.
        # A key defaulted differently in the two parsers is a config that means two
        # things depending on which bridge reads it.
        self.Carla_setup["UseVehicleTypeAsBlueprint"] = self.parserFlag(carla_node, "UseVehicleTypeAsBlueprint", False)
        self.Carla_setup["RealtimePacing"] = self.parserFlag(carla_node, "RealtimePacing", False)
        # Carla render sub-step (interpolate the feed for smoother motion). 0 -> 1:1.
        self.Carla_setup["CarlaTimeStep"] = self.parserDouble(carla_node, "CarlaTimeStep", 0.0)

        # Spectator BEV follow (rigid top-down snap). Default ON, 50 m up, north-up.
        self.Carla_setup["CenteredViewId"] = self.parserString(carla_node, "CenteredViewId", "ego")
        self.Carla_setup["EnableSpectatorFollow"] = self.parserFlag(carla_node, "EnableSpectatorFollow", True)
        self.Carla_setup["SpectatorHeight"] = self.parserDouble(carla_node, "SpectatorHeight", 50.0)
        self.Carla_setup["SpectatorAlignYaw"] = self.parserFlag(carla_node, "SpectatorAlignYaw", False)


        # EgoMode / EgoL0Driver / EnableExternalControl: read in the EgoSetup
        # block below, as the v0.9.0 spelling of Dynamics and ActuationSource.
        # #325: the user controller, as a path relative to where the run was
        # launched.
        self.Carla_setup["EgoBlueprint"] = self.parserString(carla_node, "EgoBlueprint", "vehicle.tesla.model3")
        self.Carla_setup["EgoSpawnPose"] = [float(v) for v in (carla_node.get("EgoSpawnPose") or [])]
        self.Carla_setup["EgoRoutePoints"] = [(float(pt[0]), float(pt[1]))
                                              for pt in (carla_node.get("EgoRoutePoints") or [])]


        # ---- EgoSetup: the ego, described once (#305) --------------------------
        # Every key falls back to the per-backend key it replaces, so older
        # scenarios parse unchanged. MIRRORS the EgoSetup section in
        # ConfigHelper.cpp -- a key defaulted differently in the two parsers means
        # two things depending on which bridge reads it.
        ego_node = config.get("EgoSetup", {}) or {}

        def _egoKey(name, fallback):
            v = self.parserString(ego_node, name, "")
            return v if v else fallback

        cm_node = config.get("CarMakerSetup", {}) or {}
        self.Ego_setup["Id"] = _egoKey(
            "Id", self.parserString(carla_node, "EgoId", "")
                  or self.parserString(cm_node, "EgoId", "ego"))
        self.Ego_setup["Type"] = _egoKey(
            "Type", self.parserString(carla_node, "EgoSumoType", "")
                    or self.parserString(cm_node, "EgoType", "car"))
        self.Ego_setup["Controller"] = _egoKey(
            "Controller", self.parserString(carla_node, "EgoController", ""))

        # Dynamics -- WHAT COMPUTES THE EGO'S MOTION. Canonical values, so the
        # bridge compares strings. v0.9.0 said this with EgoMode AND
        # EnableExternalControl, two keys that could disagree (#305).
        dyn = self.parserString(ego_node, "Dynamics", "").strip().lower()
        if not dyn and ("EnableExternalControl" in carla_node or "EgoMode" in carla_node):
            ext = self.parserFlag(carla_node, "EnableExternalControl", False)
            mode = self.parserInteger(carla_node, "EgoMode", 0)
            dyn = "virenv" if (ext and mode >= 1) else "traffic"
        if dyn == "xil":
            raise SystemExit(
                "ERROR: EgoSetup.Dynamics: 'xil' names the case where an external "
                "plant owns the ego. That case exists and works, but it is switched "
                "on by CarMakerSetup.EnableCosimulation today, not by this key.")
        if dyn and dyn not in ("traffic", "virenv"):
            raise SystemExit(
                "ERROR: EgoSetup.Dynamics must be one of traffic|virenv|xil, got '%s'" % dyn)
        self.Ego_setup["Dynamics"] = dyn

        # ActuationSource -- WHO PRODUCES THE PEDALS AND STEER. "user" is ONE
        # value; the Controller key decides where it runs.
        src = self.parserString(ego_node, "ActuationSource", "").strip().lower()
        if not src:
            l0 = (self.parserString(carla_node, "EgoL0Driver", "") or "").strip().lower()
            src = {"tm": "simulator", "pursuit": "fixs", "fallback": "fixs",
                   "egodriver": "fixs", "actuation": "user", "embedded": "user"}.get(l0, "")
        if src and src not in ("simulator", "fixs", "user"):
            raise SystemExit(
                "ERROR: EgoSetup.ActuationSource must be one of simulator|fixs|user, "
                "got '%s'" % src)
        self.Ego_setup["ActuationSource"] = src
        self.Carla_setup["EgoRouteRepeat"] = self.parserInteger(carla_node, "EgoRouteRepeat", 50)
        self.Carla_setup["EgoTargetSpeed"] = self.parserDouble(carla_node, "EgoTargetSpeed", 8.33)
        self.Carla_setup["TrafficManagerPort"] = self.parserInteger(carla_node, "TrafficManagerPort", 8000)

        # Signal Subscription -- which junctions this client is served. The bridge
        # matches these ids to its traffic-light table; a junction subscribed with
        # no row there is a light that never changes.
        self.application_setup["SignalSubscription"] = self.parseVehicleSubscription(
            app_node, "SignalSubscription", [])

        # DataLog Setup -- generic FIXS infrastructure logging (CommonLib/VirEnv/DataLogger)
        datalog_node = config.get("DataLogSetup", {}) or {}
        self.DataLog_setup["EnableDataLog"] = self.parserFlag(datalog_node, "EnableDataLog", False)
        self.DataLog_setup["DataLogPath"] = self.parserString(datalog_node, "DataLogPath", "")
        self.DataLog_setup["DataLogWho"] = self.parserStringVector(datalog_node, "DataLogWho", [])
        self.DataLog_setup["DataLogFields"] = self.parserStringVector(datalog_node, "DataLogFields", [])

    def resetConfig(self):
        # Clear all config settings
        self.simulation_setup.clear()
        self.application_setup.clear()
        self.Xil_setup.clear()
        self.CarMaker_setup.clear()
        self.Ego_setup.clear()
        self.Sumo_setup.clear()
        self.Carla_setup.clear()
        self.DataLog_setup.clear()

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
    config_path = 'defaultConfig.yaml'
    config_helper.getConfig(config_path)
