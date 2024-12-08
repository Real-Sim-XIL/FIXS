from tkinter import E
import traci
# import libtraci as traci
import os
import numpy as np
import sys
import sumolib
from traffic_light_utils import TrafficLight
from speed_control_utils import *
from utils1 import *
from configparser import ConfigParser
import pandas as pd
from sumolib import checkBinary

if 'SUMO_HOME' in os.environ:
    sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))


class SumoEnv:
    def __init__(self):
        # sumo startup utils
        self.sumoBinary = checkBinary('sumo-gui')  # or 'sumo-gui' for graphical interface
        self.sumo_config_file = os.path.join(os.getcwd(), 'chatt.sumocfg')
        self.sumo_net_file = os.path.join(os.getcwd(), 'chatt.net.xml')
        self.sumoCmd = [self.sumoBinary, "-c", self.sumo_config_file, "--step-length", "0.1"]
        self.graph = sumolib.net.readNet(self.sumo_net_file, withInternal=True)  # internal edges are edges inside intersections or connections
        self.vertex = self.graph.getNodes()
        self.edge = self.graph.getEdges(withInternal=True)
        self.edges = ['-2801', '-280', '-307', '-327', '-281', '-315', '-321', '-300', '-2851', '-285', '-290', '-298', '-295']
        self.all_lanes = ['0', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '0']

        # traffic light ids
        self.tl_ids = ['2', '3', '10', '8', '9', '12']
        self.phase_tracking_dict = {}
        self.get_phases()
        self.spat_statuses = {}
        self.color_dict = {'green': 0, 'red': 1}
        self.sim_step = 1

        self.speed_min = 0
        self.speed_max = 21
        self.max_acc = 4.0
        self.prev_acc = 0.01

    def get_phases(self):
        for tl_id in self.tl_ids:
            self.phase_tracking_dict[tl_id] = TrafficLight(tl_id)

    def phase_tracker(self):
        # Phase trackers
        for tl_id in self.tl_ids:
            self.phase_tracking_dict[tl_id].get_remaining_green()
            self.spat_statuses[tl_id] = self.phase_tracking_dict[tl_id].spat_status

    def reset(self):
        if 'ego' in traci.vehicle.getIDList():
            traci.vehicle.remove('ego')
            # traci.simulationStep()
            #
            traci.vehicle.add('ego', 'route1', departPos=str(1), departSpeed=str(0.1), departLane='0')
            traci.vehicle.setColor('ego', color=(255, 0, 0, 255))
            traci.vehicle.setSpeedMode('ego', 31)  # https://sumo.dlr.de/docs/TraCI/Change_Vehicle_State.html#speed_mode_0xb3
        else:
            # traci.simulationStep()
            traci.vehicle.add('ego', 'route1', departPos=str(1), departSpeed=str(0.1), departLane='0')
            traci.vehicle.setColor('ego', color=(255, 0, 0, 255))
            traci.vehicle.setSpeedMode('ego', 31)  # https://sumo.dlr.de/docs/TraCI/Change_Vehicle_State.html#speed_mode_0xb3

        
    def vtcpmf_fuel_model(self, v_speed, v_accel):
        """
        VT-CPFM Fuel Model
        """
        r_f = (
                1.23 * 0.6 * 0.98 * 3.28 * (v_speed ** 2)
                + 9.8066 * 3152 * (1.75 / 1000) * 0.033 * v_speed
                + 9.8066 * 3152 * (1.75 / 1000) * 0.033
                + 9.8066 * 3152 * 0
        )

        power = ((r_f + 1.04 * 3152 * v_accel) / (3600 * 0.92)) * v_speed

        if power >= 0:
            return 0.00078 + 0.000019556 * power + 0.000001 * (power ** 2)
        else:
            return 0.00078

    def step(self, dist2Stop, t1s, t1e, t2s, t2e, r1s, curr_status):

        curr_speed = traci.vehicle.getSpeed('ego')
        curr_speed_mph = curr_speed * 2.23694
        curr_acc = traci.vehicle.getAcceleration('ego')
        curr_light = traci.vehicle.getNextTLS('ego')

        if traci.vehicle.getLeader('ego', 100.0):
            lead_dist = traci.vehicle.getLeader('ego', 100.0)[1] * 3.28084
            lead_speed = traci.vehicle.getSpeed(str(traci.vehicle.getLeader('ego', 100.0)[0]))
            lead_speed_mph = lead_speed * 2.23694
        else:
            lead_dist = 500 * 3.28084
            lead_speed_mph = self.speed_max * 2.23694

        instant_desired_speed, mode, a_out, max_desired_speed, minimum_desired_speed = gen_desired_spd(
            example_coasting_profile, A, B, C, M, orginal_desire_spd, next_movement, curr_speed_mph, curr_acc, 0,
            dist2Stop, lead_speed_mph, lead_dist, 2, 300, 40, curr_status, t1s + 5, t1e + 5, t2s, t2e, r1s)

        print(curr_speed_mph, instant_desired_speed, t1s, t1e, t2s, t2e, curr_status)

        return instant_desired_speed, a_out

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

SERVER_IP = '127.0.0.1'
SERVER_PORT = '430'
# IP for the simulink client to connect to
IP='127.0.0.1'
PORT='420'


from math import cos
import socket
from CommonLib.SocketHelper import SocketHelper
from CommonLib.ConfigHelper import ConfigHelper
from CommonLib.MsgHelper import MsgHelper
import argparse

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-c", "--config", type=str, help="Path to the Configuration file", default='./ecodrivingConfig.yaml')
    args = parser.parse_args()
    config_helper = ConfigHelper()
    config_helper.getConfig(args.config)
    msg_helper = MsgHelper()
    msg_helper.set_vehicle_message_field(config_helper.simulation_setup['VehicleMessageField'])
    socket_helper = SocketHelper(config_helper=config_helper, msg_helper=msg_helper)
    
    socket2FIXS = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    socket2FIXS.connect((SERVER_IP, int(SERVER_PORT)))
    print('Connected to FIXS server')

    socket2simulink = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    socket2simulink.bind((IP, int(PORT)))
    socket2simulink.listen(1)
    # if a connection is established, accept it
    socket2simulink, addr = socket2simulink.accept()
    print('Connected by Simulink client')
    senv = SumoEnv()

    parser = ConfigParser()
    parser.read('shallowford.ini')
    config = senv.configToDict(parser)

    example_coasting_profile = pd.read_csv('example_coasting_profile.csv', index_col=0)

    A = float(config['Vehicle Coasting']['a'])
    B = float(config['Vehicle Coasting']['b'])
    C = float(config['Vehicle Coasting']['c'])
    M = float(config['Vehicle Coasting']['m'])

    orginal_desire_spd = float(config['Speed Limit']['orginal_desire_spd'])
    next_movement = config['Movement']['next_movement']
    traci.init(port=1337, host='127.0.0.1')
    traci.setOrder(2)
    traci.route.add(routeID='route1', edges=senv.edges)
    senv.reset()
    VEHICLE_DYNAMICS = False
    ECO_DRIVING = True
    VEHICLE_DYNAMICS_MODEL = 'DYUMMY'
    vehicle_speed_list = []
    while True:
        try:
            instant_desired_speed = 0
            set_speed = False
            senv.phase_tracker()
            if 'ego' in traci.vehicle.getIDList():
                # print(traci.vehicle.getRoadID('ego'))

                if traci.vehicle.getRoadID('ego') != '-295':
                    traci.vehicle.setDecel('ego', senv.max_acc)
                    traci.vehicle.setAccel('ego', senv.max_acc)

                    senv.tl_light = traci.vehicle.getNextTLS('ego')

                    if senv.tl_light:
                        dist_to_light = senv.tl_light[0][2]
                        dist2Stop = dist_to_light * 3.28084
                        t1s, t1e, t2s, t2e, r1s, curr_status = process_spat_for_gen_speed_update(senv.tl_light, senv.spat_statuses)
                    else:
                        dist2Stop = 1
                        t1s, t1e, t2s, t2e, r1s, curr_status = 0 / 200, 50 / 200, 150 / 200, 200 / 200, 50 / 200, 'green', ('r', 0)



                    try:
                        instant_desired_speed, a_out = senv.step(dist2Stop, t1s, t1e, t2s, t2e, r1s, curr_status)
                        instant_desired_speed = instant_desired_speed * 0.44704
                        set_speed = True
                        traci.simulationStep()
                    except:
                        senv.reset()
                        traci.simulationStep()
                elif traci.vehicle.getRoadID('ego') == '-295':
                    # raise Exception('End of simulation')
                    senv.reset()
                    traci.simulationStep()
            else:
                traci.simulationStep()

            sim_state, sim_time = socket_helper.recv_data(socket2FIXS)
            socket_helper.vehicle_data_send_list.extend(socket_helper.vehicle_data_receive_list)
            
            for i in range(len(socket_helper.vehicle_data_send_list)):
                if not (set_speed) and ECO_DRIVING:
                   instant_desired_speed = socket_helper.vehicle_data_send_list[i].speed

                if ECO_DRIVING:
                    socket_helper.vehicle_data_send_list[i].speedDesired = instant_desired_speed
                    
                else:
                    socket_helper.vehicle_data_send_list[i].speedDesired = socket_helper.vehicle_data_send_list[i].speed
                
            ###  FIXS Layer to Simulink
            socket_helper.sendData(sim_state, sim_time, socket2simulink)
            print(f'speed desired: {socket_helper.vehicle_data_send_list[0].speedDesired}')
            speed_desired = socket_helper.vehicle_data_send_list[0].speedDesired
            # print(f'speed: {socket_helper.vehicle_data_send_list[0].speed}')
            socket_helper.clear_data()

            # receive data from the client (the actual vehicle data after the vehidle dynamics model)
            socket_helper.recv_data(socket2simulink)
            # send data to the server
            print(f'simulink speed desired: {socket_helper.vehicle_data_receive_list[0].speedDesired}')
            speed_desired_simulink = socket_helper.vehicle_data_receive_list[0].speedDesired
            # print(f'simulink speed: {socket_helper.vehicle_data_receive_list[0].speed}')
            ### Simulink data to FIXS Layer
            socket_helper.vehicle_data_send_list.extend(socket_helper.vehicle_data_receive_list)
            if VEHICLE_DYNAMICS:
                for idx in range(len(socket_helper.vehicle_data_send_list)):
                    socket_helper.vehicle_data_send_list[idx].speed = speed_desired_simulink
                    socket_helper.vehicle_data_send_list[idx].speedDesired = speed_desired_simulink
            else:
                for idx in range(len(socket_helper.vehicle_data_send_list)):
                    socket_helper.vehicle_data_send_list[idx].speed = speed_desired
                    socket_helper.vehicle_data_send_list[idx].speedDesired = speed_desired
            vehicle_speed_list.append(socket_helper.vehicle_data_send_list[0].speed)
            socket_helper.sendData(sim_state, sim_time, socket2FIXS)

            socket_helper.clear_data()
        except:
            # save the vehicle speed data
            vehicle_speed_df = pd.DataFrame(vehicle_speed_list, columns=['speed'])
            dynamics = '_vehicle_dynamics' if VEHICLE_DYNAMICS else '_no_vehicle_dynamics'
            eco_driving = '_eco_driving' if ECO_DRIVING else '_no_eco_driving'
            vehicle_dynamics = '_'+VEHICLE_DYNAMICS_MODEL
            file_name = 'vehicle_speed'+ eco_driving
            if VEHICLE_DYNAMICS:
                file_name += vehicle_dynamics
            file_name += dynamics + '.csv'
            # vehicle_speed_df.to_csv('dummy_vehicle_speed.csv')
            senv.close()

            
