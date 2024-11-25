from math import cos
import socket
from CommonLib.SocketHelper import SocketHelper
from CommonLib.ConfigHelper import ConfigHelper
from CommonLib.MsgHelper import MsgHelper
import argparse
# ip and port for the FIXS server
SERVER_IP = '127.0.0.1'
SERVER_PORT = '430'
# IP for the simulink client to connect to
IP='127.0.0.1'
PORT='420'

if __name__ == '__main__':
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
    sim_time = 0
    while True:
        print('receving data from FIXS server')
        sim_state, sim_time = socket_helper.recv_data(socket2FIXS)
        # if int(sim_time) == socket_helper.config_helper.simulation_setup["SimulationEndTime"]:
        #     print('Simulation ends')
        #     break
        socket_helper.vehicle_data_send_list.extend(socket_helper.vehicle_data_receive_list)
        for i in range(len(socket_helper.vehicle_data_send_list)):
            socket_helper.vehicle_data_send_list[i].speedDesired = socket_helper.vehicle_data_send_list[i].speed + cos(sim_time)
        # receive data from the client
        print('sending data to Simulink client')
        socket_helper.sendData(sim_state, sim_time, socket2simulink)
        socket_helper.clear_data()

        # receive data from the client (the actual vehicle data after the vehidle dynamics model)
        socket_helper.recv_data(socket2simulink)
        # send data to the server
        socket_helper.vehicle_data_send_list.extend(socket_helper.vehicle_data_receive_list)
        socket_helper.sendData(sim_state, sim_time, socket2FIXS)
        socket_helper.clear_data()