import traci
import os
import sys
import sumolib
from traffic_light_utils import TrafficLight
from vehicle_eco_pilot import EcoVehicle
from speed_control_utils import *
from sumolib import checkBinary

if 'SUMO_HOME' in os.environ:
    sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))


class SumoEnvMultiAgent:
    def __init__(self):
        # sumo startup utils
        self.sumoBinary = checkBinary('sumo-gui')  # or 'sumo-gui' for graphical interface
        self.sumo_config_file = os.path.join(os.getcwd(), 'chattCavMpr.sumocfg')
        self.sumo_net_file = os.path.join(os.getcwd(), 'chatt.net.xml')
        self.sumoCmd = [self.sumoBinary, "-c", self.sumo_config_file]
        self.graph = sumolib.net.readNet(self.sumo_net_file, withInternal=True)  # internal edges are edges inside intersections or connections
        self.vertex = self.graph.getNodes()
        self.edge = self.graph.getEdges(withInternal=True)
        self.edges = ['-2801', '-280', '-307', '-327', '-281', '-315', '-321', '-300', '-2851', '-285', '-290', '-298', '-295']

        # traffic light ids
        self.tl_ids = ['2', '3', '10', '8', '9', '12']
        self.phase_tracking_dict = {}
        self.get_phases()
        self.color_dict = {'green': 0, 'red': 1}
        self.sim_step = 0.5

        self.speed_min = 0
        self.speed_max = 21
        self.max_acc = 4.0
        self.prev_acc = 0.01

    def get_phases(self):
        for tl_id in self.tl_ids:
            self.phase_tracking_dict[tl_id] = TrafficLight(tl_id)

    def phase_tracker(self):
        # Phase trackers
        x1, x2, x3, x4, x5, x6 = (
            self.phase_tracking_dict['2'].get_remaining_green(), \
            self.phase_tracking_dict['3'].get_remaining_green(), \
            self.phase_tracking_dict['10'].get_remaining_green(), \
            self.phase_tracking_dict['8'].get_remaining_green(), \
            self.phase_tracking_dict['9'].get_remaining_green(), \
            self.phase_tracking_dict['12'].get_remaining_green())
        return x1, x2, x3, x4, x5, x6

    def start(self):
        traci.start(self.sumoCmd)
        traci.route.add(routeID='route1', edges=self.edges)

        sim_time = traci.simulation.getTime()
        x1, x2, x3, x4, x5, x6 = self.phase_tracker()

        while sim_time <= 28985:
            sim_time = traci.simulation.getTime()
            traci.simulationStep()

        while sim_time <= 33000:
            sim_time = traci.simulation.getTime()
            print(sim_time)
            time.sleep(self.sim_step)

            eco_speed = []
            cav_id_list = []

            for veh_id in traci.vehicle.getIDList():
                veh_type = traci.vehicle.getTypeID(veh_id)
                # print(veh_id, ' IS ', veh_type)
                if veh_type == 'CAV':
                    cav_id_list.append(veh_id)
                    cav = EcoVehicle(veh_id)
                    cav_eco_speed = cav.get_eco_speed(x1, x2, x3, x4, x5, x6)
                    # print(traci.vehicle.getSpeed(veh_id), cav_eco_speed)
                    eco_speed.append(cav_eco_speed)

            self.apply_vehicle_control(cav_id_list, eco_speed)
            traci.simulationStep()

    def apply_vehicle_control(self, cav_id_list, eco_speed, smooth=False):
        # to handle the case of a single vehicle
        if type(cav_id_list) == str:
            cav_id_list = [cav_id_list]
            eco_speed = [eco_speed]

        for i, vid in enumerate(cav_id_list):
            if eco_speed[i] is not None:
                if smooth:
                    traci.vehicle.slowDown(vid, eco_speed[i] * 0.44704, 1)
                else:
                    traci.vehicle.setSpeed(vid, eco_speed[i] * 0.44704)

    def close(self):
        traci.close()

    @classmethod
    def configToDict(cls, data):
        """
        Parses config contents into a dictionary.

        Parameters
        ----------
        data : ConfigParser.read() output
            content of config file in ConfigParser format

        Returns
        -------
        dict
            dictionary of config file
        """
        config = {}
        for section in data.sections():
            config[section] = {}
            for item in data[section]:
                config[section][item] = data[section][item]
        return config


if __name__ == "__main__":
    senv = SumoEnvMultiAgent()
    senv.start()