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
        self.Carla_setup["EnableExternalControl"] = self.parserFlag(carla_node, "EnableExternalControl", False)
        self.Carla_setup["UseVehicleTypeAsBlueprint"] = self.parserFlag(carla_node, "UseVehicleTypeAsBlueprint", False)
        self.Carla_setup["RealtimePacing"] = self.parserFlag(carla_node, "RealtimePacing", False)
        # Carla render sub-step (interpolate the feed for smoother motion). 0 -> 1:1.
        self.Carla_setup["CarlaTimeStep"] = self.parserDouble(carla_node, "CarlaTimeStep", 0.0)

        # Spectator BEV follow (rigid top-down snap). Default ON, 50 m up, north-up.
        self.Carla_setup["CenteredViewId"] = self.parserString(carla_node, "CenteredViewId", "ego")
        self.Carla_setup["EnableSpectatorFollow"] = self.parserFlag(carla_node, "EnableSpectatorFollow", True)
        self.Carla_setup["SpectatorHeight"] = self.parserDouble(carla_node, "SpectatorHeight", 50.0)
        self.Carla_setup["SpectatorAlignYaw"] = self.parserFlag(carla_node, "SpectatorAlignYaw", False)


        # #174 ego driving-mode ladder:
        #   0 = SumoDriver (the traffic sim owns the ego; Carla renders it)
        #   1 = CarlaDriver / L0    2 = Advisory / L2    3 = Control / L4
        self.Carla_setup["EgoMode"] = self.parserInteger(carla_node, "EgoMode", 0)
        # L0 driver: native Carla TM by default; "Pursuit" selects the EgoDriver
        # fallback module, "Actuation" the external wire-command path.
        self.Carla_setup["EgoL0Driver"] = self.parserString(carla_node, "EgoL0Driver", "TM")
        # #325: the user controller that occupies the driver slot, as a path
        # relative to where the run was launched. Read here rather than left to
        # the bridge's dict lookup, so a scenario that names one and a loader
        # that does not parse it cannot silently disagree.
        self.Carla_setup["EgoBlueprint"] = self.parserString(carla_node, "EgoBlueprint", "vehicle.tesla.model3")
        self.Carla_setup["EgoSpawnPose"] = [float(v) for v in (carla_node.get("EgoSpawnPose") or [])]
        self.Carla_setup["EgoRoutePoints"] = [(float(pt[0]), float(pt[1]))
                                              for pt in (carla_node.get("EgoRoutePoints") or [])]


        # ---- EgoSetup: the ego, described once (#305) --------------------------
        # A vehicle whose motion something outside the traffic simulator computes
        # is the same situation whether that something is CarMaker, an XIL plant
        # or Carla. It used to be described twice -- CarMakerSetup.EgoId and
        # CarlaSetup.EgoId, and a separate branch in TrafficHelper for each -- and
        # nothing stopped the two descriptions naming different vehicles.
        #
        # EgoSetup is that description. Every key falls back to the CarlaSetup key
        # it replaces, so scenarios written before it parse unchanged. Mirrors the
        # EgoSetup section in ConfigHelper.cpp; a key defaulted differently in the
        # two parsers is a config that means two things depending on which bridge
        # reads it.
        ego_node = config.get("EgoSetup", {}) or {}

        def _egoKey(name, fallback):
            v = self.parserString(ego_node, name, "")
            return v if v else fallback

        cm_node = config.get("CarMakerSetup", {}) or {}
        self.Ego_setup["Id"] = _egoKey(
            "Id", self.parserString(carla_node, "EgoId", "")
                  or self.parserString(cm_node, "EgoId", "ego"))
        self.Ego_setup["SumoType"] = _egoKey(
            "SumoType", self.parserString(carla_node, "EgoSumoType", "")
                        or self.parserString(cm_node, "EgoType", "car"))
        self.Ego_setup["Controller"] = _egoKey(
            "Controller", self.parserString(carla_node, "EgoController", ""))
        self.Ego_setup["KeepRoute"] = self.parserInteger(
            ego_node, "KeepRoute",
            self.parserInteger(config.get("SumoSetup", {}) or {}, "EgoKeepRoute", 6))

        # Dynamics -- WHAT COMPUTES THE EGO'S MOTION. EgoMode and EgoL0Driver above
        # are what the bridge reads, and are DERIVED here exactly as the EgoSetup
        # section of ConfigHelper.cpp derives them.
        #
        # This peer had no derivation at all once, so a config using the declared
        # vocabulary was read as EgoMode 0 (the traffic simulator owns the ego) and
        # the Python bridge quietly ran an L0 scenario while TrafficLayer, reading
        # the C++ helper, ran an L2 one. Nothing failed; the ego simply was not
        # taken over, and the embedded controller was never called.
        egoDynamics = _egoKey("Dynamics", self.parserString(carla_node, "EgoDynamics", ""))
        self.Ego_setup["Dynamics"] = egoDynamics
        if egoDynamics:
            dyn = egoDynamics.strip().lower()
            if dyn == "traffic":
                self.Carla_setup["EgoMode"] = 0
                self.Carla_setup["EnableExternalControl"] = False
            elif dyn in ("virenv", "carla"):
                # 2, not 1, on purpose: "advisory-capable" costs nothing when no
                # controller is wired in (the advisory read falls back to
                # EgoTargetSpeed), and it keeps L0 and L2 ONE configuration.
                self.Carla_setup["EgoMode"] = 2
                # Also what TrafficLayer's external-ego ownership test reads.
                # Deriving both from one key is the point: the two processes decide
                # ego ownership together and have no arbiter, so they must not be
                # able to disagree.
                self.Carla_setup["EnableExternalControl"] = True
            elif dyn == "xil":
                raise SystemExit(
                    "ERROR: EgoSetup.Dynamics: 'xil' names the case where an external\n"
                    "       plant owns the ego. That case exists and works, but it is\n"
                    "       switched on by CarMakerSetup.EnableCosimulation today, not\n"
                    "       by this key. Leave Dynamics unset for a CarMaker/XIL run.")
            else:
                raise SystemExit(
                    "ERROR: EgoSetup.Dynamics must be one of traffic|virenv|xil "
                    "(carla = deprecated alias for virenv), got '%s'" % egoDynamics)

        # ActuationSource -- WHO PRODUCES THE PEDALS AND STEER IT RUNS ON.
        # "user" is ONE value; whether a Controller file is named decides whether
        # that user code runs in-process (once per Carla step, reading the plant)
        # or on the far side of the 0.1 s feed. The old vocabulary named those two
        # halves directly and is still accepted.
        egoActuation = _egoKey("ActuationSource",
                               self.parserString(carla_node, "EgoActuationSource", ""))
        self.Ego_setup["ActuationSource"] = egoActuation
        if egoActuation:
            src = egoActuation.strip().lower()
            hasController = bool(self.Ego_setup["Controller"])
            if src in ("simulator", "carlatm"):
                driver = "TM"
            elif src in ("fixs", "internal"):
                driver = "Pursuit"
            elif src == "embedded" or (src == "user" and hasController):
                driver = "Embedded"
            elif src == "external" or (src == "user" and not hasController):
                driver = "Actuation"
            else:
                raise SystemExit(
                    "ERROR: EgoSetup.ActuationSource must be one of simulator|fixs|user "
                    "(carlaTM|internal|external|embedded = deprecated aliases), got '%s'"
                    % egoActuation)
            if src == "embedded" and not hasController:
                raise SystemExit(
                    "ERROR: EgoSetup.ActuationSource: 'embedded' needs a Controller file.")
            self.Carla_setup["EgoL0Driver"] = driver
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
