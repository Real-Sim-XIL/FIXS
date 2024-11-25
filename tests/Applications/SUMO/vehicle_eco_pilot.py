import traci
from traffic_light_utils import TrafficLight
from speed_control_utils import *
from utils1 import *
from configparser import ConfigParser


class EcoVehicle:
    def __init__(self, vehicle):
        """Vehicle id, same as SUMO's one"""
        self.id = vehicle
        """ Route ID, assumes the format X_Y where X and Y are single letter junction ids """
        self.route_id = traci.vehicle.getRouteID(vehicle)
        """ SUMO's longitudinal lane position """
        self.lane_position = 0
        """ speed, same as SUMO's one """
        self.speed = 0
        """ accel, same as SUMO's one """
        self.accel = 0
        """ relative distance to the TL, negative when approaching the TL, 0 in the internal lanes, positive after """
        self.relative_distance = 0.0
        """ bit encoding of vehicle exterior signals, same as https://sumo.dlr.de/docs/TraCI/Vehicle_Signalling.html """
        self.signals = 0
        self.tl_light = traci.vehicle.getNextTLS(self.id)

        # constants throughout the simulation
        """ length, same as SUMO's one """
        self.length = traci.vehicle.getLength(self.id)
        """ speed factor, same as SUMO's one, considered constant """
        self.speed_factor = traci.vehicle.getSpeedFactor(self.id)

        v_type = traci.vehicle.getTypeID(self.id)

        self.tau = traci.vehicletype.getTau(v_type)
        self.min_gap = traci.vehicletype.getMinGap(v_type)
        self.max_accel = traci.vehicletype.getAccel(v_type)
        self.max_decel = traci.vehicletype.getDecel(v_type)
        self.previous_step_idm_speed = 0
        self.speed_max = 21

        # eco-driving
        parser = ConfigParser()
        parser.read('shallowford.ini')
        config = self.configToDict(parser)

        self.example_coasting_profile = pd.read_csv('example_coasting_profile.csv', index_col=0)

        self.A = float(config['Vehicle Coasting']['a'])
        self.B = float(config['Vehicle Coasting']['b'])
        self.C = float(config['Vehicle Coasting']['c'])
        self.M = float(config['Vehicle Coasting']['m'])

        self.orginal_desire_spd = float(config['Speed Limit']['orginal_desire_spd'])
        self.next_movement = config['Movement']['next_movement']

        # traffic light ids
        self.tl_ids = ['2', '3', '10', '8', '9', '12']
        self.phase_tracking_dict = {}
        self.get_phases()
        self.color_dict = {'green': 0, 'red': 1}
        self.sim_step = 0.5

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

    def get_phases(self):
        for tl_id in self.tl_ids:
            self.phase_tracking_dict[tl_id] = TrafficLight(tl_id)

    def phase_tracker(self):
        # intersection phase trackers
        x1, x2, x3, x4, x5, x6 = (
            self.phase_tracking_dict['2'].get_remaining_green(), \
            self.phase_tracking_dict['3'].get_remaining_green(), \
            self.phase_tracking_dict['10'].get_remaining_green(), \
            self.phase_tracking_dict['8'].get_remaining_green(), \
            self.phase_tracking_dict['9'].get_remaining_green(), \
            self.phase_tracking_dict['12'].get_remaining_green())
        return x1, x2, x3, x4, x5, x6

    def get_eco_speed(self, x1, x2, x3, x4, x5, x6):
        if self.tl_light:
            dist_to_light = self.tl_light[0][2]
            dist2Stop = dist_to_light * 3.28084
            t1s, t1e, t2s, t2e, r1s, curr_status = process_spat_for_gen_speed(self.tl_light, x1, x2, x3, x4, x5, x6 )
        else:
            dist2Stop = 1
            t1s, t1e, t2s, t2e, r1s, curr_status = 0 / 200, 50 / 200, 150 / 200, 200 / 200, 50 / 200, 'green'

        curr_speed = traci.vehicle.getSpeed(self.id)
        curr_speed_mph = curr_speed * 2.23694
        curr_acc = traci.vehicle.getAcceleration(self.id)

        if traci.vehicle.getLeader(self.id, 100.0):
            lead_dist = traci.vehicle.getLeader(self.id, 100.0)[1] * 3.28084
            lead_speed = traci.vehicle.getSpeed(str(traci.vehicle.getLeader(self.id, 100.0)[0]))
            lead_speed_mph = lead_speed * 2.23694
        else:
            lead_dist = 500 * 3.28084
            lead_speed_mph = self.speed_max * 2.23694

        instant_desired_speed, mode, a_out, max_desired_speed, minimum_desired_speed = gen_desired_spd(
            self.example_coasting_profile, self.A, self.B, self.C, self.M, self.orginal_desire_spd, self.next_movement, curr_speed_mph, curr_acc, 0,
            dist2Stop, lead_speed_mph, lead_dist, 2, 300, 40, curr_status, t1s, t1e, t2s, t2e, r1s)

        print(curr_speed_mph, dist2Stop, t1s, t1e, t2s, t2e, curr_status, instant_desired_speed, mode)

        return instant_desired_speed
