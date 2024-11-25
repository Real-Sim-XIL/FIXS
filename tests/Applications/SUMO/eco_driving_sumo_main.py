import traci
import os
import sys

# Set the SUMO_HOME environment variable if not already set
if 'SUMO_HOME' not in os.environ:
    os.environ['SUMO_HOME'] = r'C:\Program Files (x86)\Eclipse\Sumo\bin'

if 'SUMO_HOME' in os.environ:
    sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))

# Importing SUMO tools
from sumolib import checkBinary

# Define the path to your .sumocfg file
sumo_config_file = r'D:\ORNL\OneDrive - Oak Ridge National Laboratory\ORNL\Project\Real-Twin\CAV_CaseStudy\cav_casestudy\SUMO\chatt.sumocfg'

# Check for the SUMO binary
sumoBinary = checkBinary('sumo-gui')  # or 'sumo-gui' for graphical interface

# Start the SUMO simulation
traci.start([sumoBinary, "-c", sumo_config_file])

# Running the simulation for a specified number of steps
simulation_steps = 100000  # Number of simulation steps to run
for step in range(simulation_steps):
    print(f"Simulation step: {step}")
    traci.simulationStep()  # Advance the simulation by one step

    # You can add additional commands here to control the simulation
    # Example: Get vehicle IDs and positions
    vehicle_ids = traci.vehicle.getIDList()
    for vehicle_id in vehicle_ids:
        position = traci.vehicle.getPosition(vehicle_id)
        print(f"Vehicle {vehicle_id} is at position {position}")

# Close the simulation
traci.close()
print("Simulation finished.")