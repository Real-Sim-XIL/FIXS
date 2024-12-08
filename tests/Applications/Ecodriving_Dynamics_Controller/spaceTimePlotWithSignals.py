import pandas as pd
import xml.etree.ElementTree as ET
from matplotlib import pyplot as plt
import numpy as np
import seaborn as sns


##############################################################
# Parse the signal changes (SaveTLSSwitchStates) XML file
##############################################################
def read_signal_xml(signal_file):
    tree = ET.parse(signal_file)
    root = tree.getroot()

    # Extract data from XML into a list of dictionaries
    data_list = []

    for tls in root.findall('tlsState'):
        data_list.append(tls.attrib)

    # Convert the list of dictionaries to a dataframe
    sumoSignalResult = pd.DataFrame(data_list)

    return sumoSignalResult


# plot signal changes
def plot_signal_changes_xml(sumosignalresult, sumoSignalConfig, plot_start_time, duration_limit, direction='WB'):
    print(direction)
    sumoSignalConfig = sumoSignalConfig[sumoSignalConfig['approach_direction'] == direction].reset_index(drop=True)

    for sc in sumoSignalConfig.id.unique():
        print(sc)
        phase_num = sumoSignalConfig[sumoSignalConfig['id'] == sc]['name'].values[0]
        movement_indexes = eval(sumoSignalConfig[sumoSignalConfig['id'] == sc]['movement_index'].values[0])
        x_loc = sumoSignalConfig[sumoSignalConfig['id'] == sc]['distance'].values[0]

        signal_changes_tem = sumosignalresult[sumosignalresult['id'] == sc].reset_index(drop=True)
        signal_changes_tem['target_phase_state'] = signal_changes_tem['state'].str[movement_indexes[0]: movement_indexes[0] + 1]

        # only keep phase changes for the targeted phase
        signal_changes_tem = signal_changes_tem[signal_changes_tem['target_phase_state'] != signal_changes_tem['target_phase_state'].shift()]
        signal_changes_tem['target_phase_state'] = signal_changes_tem['target_phase_state'].map({'G': 'green', 'y': 'yellow', 'r': 'red'})

        cnt = 1
        prev_idx = 0
        prev_clr = signal_changes_tem.loc[prev_idx,'target_phase_state']
        for index, row in signal_changes_tem.iterrows():
            if index >= 1:
                print(index)
                y1 = [x_loc, x_loc]
                x1 = [float(signal_changes_tem.loc[prev_idx,'time']), float(row.time)]
                plt.plot(x1, y1, color=prev_clr, linewidth=7, solid_capstyle='butt')
                c = row.target_phase_state

                prev_idx = index
                prev_clr = c
                cnt += 1
            else:
                continue

    plt.title('Shallowford Rd Space-Time Diagram')
    plt.xlabel('Simulation Second (s)', fontsize=20)
    plt.ylabel('Distance along '+ direction +' Direction', fontsize=20)
    plt.xlim(plot_start_time, plot_start_time + duration_limit)
    plt.yticks(sumoSignalConfig['distance'].values, sumoSignalConfig['id'].values, fontsize=20)
    plt.gca().invert_yaxis()


##############################################################
# parse the vehicle trajectory data
##############################################################
def read_trajectory_xml(trajectory_file, refer_coord):
    tree = ET.parse(trajectory_file)
    root = tree.getroot()

    # Extract data from XML into a list of dictionaries
    data_list = []

    for ts in root.findall('timestep'):
        for traj in ts.findall('vehicle'):
            # print(traj.attrib)
            tem_dict = traj.attrib
            tem_dict.update(ts.attrib)
            data_list.append(tem_dict)

    trajectory = pd.DataFrame(data_list)
    # Calculate the relative distance for each point
    trajectory['distance'] = np.sqrt((trajectory['x'].astype(float) - refer_coord[0]) ** 2 + (trajectory['y'].astype(float) - refer_coord[1]) ** 2)

    return trajectory


def plot_trajectory(trajectory_data, trajectory_data_full, num_veh_limit, highlight_veh_number='None'):
    line_width = 1
    trajectory_data['time'] = trajectory_data['time'].astype(float)
    trajectory_data['distance'] = trajectory_data['distance'].astype(float)
    trajectory_data = trajectory_data[trajectory_data['id'] != highlight_veh_number]

    if num_veh_limit > len(trajectory_data.id.unique()):
        num_veh_limit = len(trajectory_data.id.unique())
    else:
        pass

    # for i, veh in enumerate(trajectory_data.id.unique()[0:num_veh_limit]):
    #     print(len(trajectory_data.id.unique()), i)
    #     dfs = trajectory_data[trajectory_data['id'] == veh].reset_index(drop=True)
    #     # if the O and D are all on the major road, then plot.
    #     # if dfs['Link'].iloc[0] in linkinfo['Link'].unique().tolist() and dfs['Link'].iloc[-1] in linkinfo['Link'].unique().tolist():
    #     clr = 'grey'
    #     plt.plot(dfs['time'].astype(float), dfs['distance'].astype(float), color=clr, linewidth=line_width, solid_capstyle='butt')

    sns.lineplot(data=trajectory_data, x='time', y='distance', hue='id', legend=False, palette=['grey'])

    # highlight a specific vehicle
    if highlight_veh_number != 'None':
        dfs = trajectory_data_full[trajectory_data_full['id'] == highlight_veh_number]
        clr = 'lightgreen'
        line_width = 4
        plt.plot(dfs['time'].astype(float), dfs['distance'].astype(float), color=clr, linewidth=line_width, solid_capstyle='butt')


if __name__ == "__main__":
    # define a reference point coordinates at the leftmost point to calculate the distance over the corridor
    refer_coord = [230, 705]

    # read sumoSignalConfig_26
    sumoSignalConfig = pd.read_csv('sumoSignalConfig_26.csv', index_col=0)
    sumoSignalConfig['id'] = sumoSignalConfig['id'].astype(str)

    # Calculate the relative distance for each point
    sumoSignalConfig['distance'] = np.sqrt((sumoSignalConfig['x'] - refer_coord[0]) ** 2 + (sumoSignalConfig['y'] - refer_coord[1]) ** 2)

    trajectory_data = read_trajectory_xml(r'EcoDrivingWithoutDynamicsMaxRecall_10Hz_LoopRoute/fcd.xml', refer_coord)
    sumo_signal = read_signal_xml(r'EcoDrivingWithoutDynamicsMaxRecall_10Hz_LoopRoute/signal_result.xml')

    # only keep the WB direction trajectories
    wb_lanes = ['-2801', '-280', '-307', '-327', '-281', '-315', '-321', '-300', '-2851', '-285', '-290', '-298', '-295']
    trajectory_data['segment'] = trajectory_data['lane'].str.split('_').str[0]
    trajectory_data_wb = trajectory_data[trajectory_data['segment'].isin(wb_lanes)].reset_index(drop=True)

    # plot
    fig = plt.figure()
    fig.set_size_inches(15, 10)
    plt.rcParams.update({'font.size': 20})

    plot_signal_changes_xml(sumo_signal, sumoSignalConfig, 28985, 1200, direction='WB')
    plot_trajectory(trajectory_data_wb, trajectory_data, 300, 'ego')
    plt.show()

    print('test')