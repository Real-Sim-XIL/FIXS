import matplotlib.pyplot as plt
import matplotlib as mpl
import numpy as np
import pandas as pd

# Increase font sizes globally
mpl.rcParams['axes.titlesize'] = 40
mpl.rcParams['axes.labelsize'] = 30
mpl.rcParams['xtick.labelsize'] = 20
mpl.rcParams['ytick.labelsize'] = 20
mpl.rcParams['legend.fontsize'] = 20
mpl.rcParams['figure.titlesize'] = 30
import pandas as pd
import xml.etree.ElementTree as ET
from matplotlib import pyplot as plt
import numpy as np


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
def plot_signal_changes_xml(ax, sumosignalresult, sumoSignalConfig, plot_start_time, duration_limit, direction='EB'):
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
        signal_changes_tem['target_phase_state'] = signal_changes_tem['target_phase_state'].map({'G': 'green', 'y': 'yellow', 'r': 'red', 's': 'red'})

        cnt = 1
        prev_idx = 0
        prev_clr = signal_changes_tem.loc[prev_idx,'target_phase_state']
        for index, row in signal_changes_tem.iterrows():
            if index >= 1:
                print(index)
                y1 = [x_loc, x_loc]
                x1 = [float(signal_changes_tem.loc[prev_idx,'time']), float(row.time)]
                ax.plot(x1, y1, color=prev_clr, linewidth=7, solid_capstyle='butt')
                c = row.target_phase_state

                prev_idx = index
                prev_clr = c
                cnt += 1
            else:
                continue

    ax.set_title('Shallowford Rd Space-Time Diagram')
    ax.set_xlabel('Simulation Second (s)', fontsize=20)
    ax.set_ylabel('Distance along ' + direction + ' Direction', fontsize=20)
    ax.set_xlim(plot_start_time, plot_start_time + duration_limit)
    ax.set_yticks(sumoSignalConfig['distance'].values)
    ax.set_yticklabels(sumoSignalConfig['id'].values, fontsize=20)
    # ax.invert_yaxis()

    return ax

SIMULATION_BEGIN_TIME = 28985
def get_speed_acceleration_distance(df):
    df['acceleration'] = df['speed'].diff()
    df['travel_distance'] = df['speed'].cumsum()
    df['time'] = df.index + SIMULATION_BEGIN_TIME
    return df

# Assuming CSVs are already loaded
eco_ICV_df = get_speed_acceleration_distance(pd.read_csv('vehicle_speed_eco_driving_ICV_vehicle_dynamics.csv'))
eco_ICV_Longitudinal_df = get_speed_acceleration_distance(pd.read_csv('vehicle_speed_eco_driving_ICV_Longitudinal_vehicle_dynamics.csv'))
eco_no_dyno_df = get_speed_acceleration_distance(pd.read_csv('vehicle_speed_eco_driving_no_vehicle_dynamics.csv'))
baseline_ICV_df = get_speed_acceleration_distance(pd.read_csv('vehicle_speed_no_eco_driving_ICV_vehicle_dynamics.csv'))
baseline_ICV_Longitudinal_df = get_speed_acceleration_distance(pd.read_csv('vehicle_speed_no_eco_driving_ICV_Longitudinal_vehicle_dynamics.csv'))
baseline_no_dyno_df = get_speed_acceleration_distance(pd.read_csv('vehicle_speed_no_eco_driving_no_vehicle_dynamics.csv'))
eco_EV_df = get_speed_acceleration_distance(pd.read_csv('vehicle_speed_eco_driving_EV_vehicle_dynamics.csv'))
baseline_EV_df = get_speed_acceleration_distance(pd.read_csv('vehicle_speed_no_eco_driving_EV_vehicle_dynamics.csv'))


refer_coord = [1380, 225]

# read sumoSignalConfig_26
sumoSignalConfig = pd.read_csv('sumoSignalConfig_26.csv', index_col=0)
sumoSignalConfig['id'] = sumoSignalConfig['id'].astype(str)
# Calculate the relative distance for each point
sumoSignalConfig['distance'] = np.sqrt((sumoSignalConfig['x'] - refer_coord[0]) ** 2 + (sumoSignalConfig['y'] - refer_coord[1]) ** 2)

print(sumoSignalConfig['distance'])








fig, axes = plt.subplots(2, 1, figsize=(40, 20))
sumo_signal = read_signal_xml('signal_result_ECO_Baseline.xml')
# Subplot 1: Travel Distance
axes[0] = plot_signal_changes_xml(axes[0], sumo_signal, sumoSignalConfig, SIMULATION_BEGIN_TIME, max(len(eco_ICV_Longitudinal_df), len(eco_no_dyno_df)), direction='WB')
axes[0].plot(eco_ICV_Longitudinal_df['time'], eco_ICV_Longitudinal_df['travel_distance'], label='Eco Driving ICV', linewidth=2)
axes[0].plot(eco_no_dyno_df['time'], eco_no_dyno_df['travel_distance'], label='Eco Driving No Vehicle Dynamics', linewidth=2)

axes[0].grid(visible=True, linestyle='--', alpha=0.7)
axes[0].legend(loc='best', frameon=True)

# plt.show()
# Subplot 2: Speed
axes[1].plot(eco_ICV_Longitudinal_df['time'], eco_ICV_Longitudinal_df['speed'], label='Eco Driving ICV', linewidth=2)
axes[1].plot(eco_no_dyno_df['time'], eco_no_dyno_df['speed'], label='Eco Driving No Vehicle Dynamics', linewidth=2)
axes[1].set_xlabel('Time (s)')
axes[1].set_ylabel('Speed (m/s)')
axes[1].set_title('Speed-Time Diagram')
axes[1].grid(visible=True, linestyle='--', alpha=0.7)
axes[1].legend(loc='best', frameon=True)

plt.suptitle('ECO ICV vs. ECO w/o Vehicle Dynamics')
plt.tight_layout(rect=[0, 0, 1, 0.95])  # Adjust layout to make room for the suptitle
plt.savefig('ECO_ICV_VS_ECO_NO_DYNO.png', dpi=300)

fig, axes = plt.subplots(2, 1, figsize=(40, 20))
sumo_signal = read_signal_xml('signal_result_ECO_Baseline.xml')
# Subplot 1: Travel Distance
axes[0] = plot_signal_changes_xml(axes[0], sumo_signal, sumoSignalConfig, SIMULATION_BEGIN_TIME, max(len(eco_EV_df), len(eco_no_dyno_df)), direction='WB')
axes[0].plot(eco_EV_df['time'] , eco_EV_df['travel_distance'], label='Eco Driving ICV', linewidth=2)
axes[0].plot(eco_no_dyno_df['time'] ,eco_no_dyno_df['travel_distance'], label='Eco Driving No Vehicle Dynamics', linewidth=2)
axes[0].set_xlabel('Time (s)')
axes[0].set_ylabel('Distance (m)')
axes[0].set_title('Distance-Time Diagram')
axes[0].grid(visible=True, linestyle='--', alpha=0.7)
axes[0].legend(loc='best', frameon=True)

# Subplot 2: Speed
axes[1].plot(eco_EV_df['time'] ,eco_EV_df['speed'], label='Eco Driving ICV', linewidth=2)
axes[1].plot(eco_no_dyno_df['time'], eco_no_dyno_df['speed'], label='Eco Driving No Vehicle Dynamics', linewidth=2)
axes[1].set_xlabel('Time (s)')
axes[1].set_ylabel('Speed (m/s)')
axes[1].set_title('Speed-Time Diagram')
axes[1].grid(visible=True, linestyle='--', alpha=0.7)
axes[1].legend(loc='best', frameon=True)

plt.suptitle('ECO EV vs. ECO w/o Vehicle Dynamics')
plt.tight_layout(rect=[0, 0, 1, 0.95])  # Adjust layout to make room for the suptitle
plt.savefig('ECO_EV_VS_ECO_NO_DYNO.png', dpi=300)


fig, axes = plt.subplots(2, 1, figsize=(40, 20))
sumo_signal = read_signal_xml('signal_result_Baseline.xml')
# Subplot 1: Travel Distance
axes[0] = plot_signal_changes_xml(axes[0], sumo_signal, sumoSignalConfig, SIMULATION_BEGIN_TIME, max(len(baseline_ICV_Longitudinal_df), len(baseline_no_dyno_df)), direction='WB')
axes[0].plot(baseline_ICV_Longitudinal_df['time'], baseline_ICV_Longitudinal_df['travel_distance'], label='Eco Driving ICV', linewidth=2)
axes[0].plot(baseline_no_dyno_df['time'] ,baseline_no_dyno_df['travel_distance'], label='Eco Driving No Vehicle Dynamics', linewidth=2)
axes[0].set_xlabel('Time (s)')
axes[0].set_ylabel('Distance (m)')
axes[0].set_title('Distance-Time Diagram')
axes[0].grid(visible=True, linestyle='--', alpha=0.7)
axes[0].legend(loc='best', frameon=True)

# Subplot 2: Speed
axes[1].plot(baseline_ICV_Longitudinal_df['time'], baseline_ICV_Longitudinal_df['speed'], label='Eco Driving ICV', linewidth=2)
axes[1].plot(baseline_no_dyno_df['time'], baseline_no_dyno_df['speed'], label='Eco Driving No Vehicle Dynamics', linewidth=2)
axes[1].set_xlabel('Time (s)')
axes[1].set_ylabel('Speed (m/s)')
axes[1].set_title('Speed-Time Diagram')
axes[1].grid(visible=True, linestyle='--', alpha=0.7)
axes[1].legend(loc='best', frameon=True)

plt.suptitle('Baseline ICV vs. Baseline w/o Vehicle Dynamics')
plt.tight_layout(rect=[0, 0, 1, 0.95])  # Adjust layout to make room for the suptitle
plt.savefig('Baseline_ICV_VS_Baseline_NO_DYNO.png', dpi=300)

fig, axes = plt.subplots(2, 1, figsize=(40, 20))
sumo_signal = read_signal_xml('signal_result_Baseline.xml')
# Subplot 1: Travel Distance
axes[0] = plot_signal_changes_xml(axes[0], sumo_signal, sumoSignalConfig, SIMULATION_BEGIN_TIME, max(len(baseline_EV_df), len(baseline_no_dyno_df)), direction='WB')
axes[0].plot(baseline_EV_df['time'], baseline_EV_df['travel_distance'], label='Eco Driving ICV', linewidth=2)
axes[0].plot(baseline_no_dyno_df['time'], baseline_no_dyno_df['travel_distance'], label='Eco Driving No Vehicle Dynamics', linewidth=2)
axes[0].set_xlabel('Time (s)')
axes[0].set_ylabel('Distance (m)')
axes[0].set_title('Distance-Time Diagram')
axes[0].grid(visible=True, linestyle='--', alpha=0.7)
axes[0].legend(loc='best', frameon=True)

# Subplot 2: Speed
axes[1].plot(baseline_EV_df['time'], baseline_EV_df['speed'], label='Eco Driving ICV', linewidth=2)
axes[1].plot(baseline_no_dyno_df['time'], baseline_no_dyno_df['speed'], label='Eco Driving No Vehicle Dynamics', linewidth=2)
axes[1].set_xlabel('Time (s)')
axes[1].set_ylabel('Speed (m/s)')
axes[1].set_title('Speed-Time Diagram')
axes[1].grid(visible=True, linestyle='--', alpha=0.7)
axes[1].legend(loc='best', frameon=True)

plt.suptitle('Baseline EV vs. Baseline w/o Vehicle Dynamics')
plt.tight_layout(rect=[0, 0, 1, 0.95])  # Adjust layout to make room for the suptitle
plt.savefig('Baseline_EV_VS_Baseline_NO_DYNO.png', dpi=300)