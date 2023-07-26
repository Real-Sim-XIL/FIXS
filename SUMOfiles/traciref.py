import os, sys
import time
import numpy as np
import math
import matplotlib.pyplot as plt

# we need to import python modules from the $SUMO_HOME/tools directory
if 'SUMO_HOME' in os.environ:
    tools = os.path.join(os.environ['SUMO_HOME'], 'tools')
    sys.path.append(tools)
else:
    sys.exit("please declare environment variable 'SUMO_HOME'")
    
import traci

dt = .1
tSimuEnd = 90
nT = math.ceil(tSimuEnd/dt)
nMaxVeh = 100

#if __name__ == "__main__":
sceneName = "./coordMerge_v1_paper.sumocfg "
traci.start(["sumo-gui", "-c", sceneName, "--start", "--step-length", str(dt)])
#traci.start(["C:/sumo_orig/bin/sumo-gui", "-c", sceneName, "--start", "--step-length", str(dt)])
#traci.start(["C:/sumo_orig/bin/sumo-gui", "-c", sceneName, "--start", "--step-length", str(dt), "--num-clients", "2"], port=int(1337))

#PORT = int(1337)
#traci.init(PORT)

##PORT = int(1337)
##traci.init(PORT,host="localhost")

##PORT = int(1337)
####traci.init(PORT,host="192.168.0.33")
###traci.init(PORT,host="192.168.0.15")
##traci.init(PORT,host="192.168.140.100")

traci.setOrder(1)
    
simTime = 0
iT = 0

t_data = np.empty((nT,nMaxVeh))
t_data[:] = np.nan
type_veh_data = np.empty((nT,nMaxVeh),dtype='object')
#type_veh_data[:] = np.nan
no_veh_data = np.empty((nT,nMaxVeh))
no_veh_data[:] = np.nan
pos_veh_data = np.empty((nT,nMaxVeh))
pos_veh_data[:] = np.nan
spd_veh_data = np.empty((nT,nMaxVeh))
spd_veh_data[:] = np.nan
acc_veh_data = np.empty((nT,nMaxVeh))
acc_veh_data[:] = np.nan
Link_no_veh_data = np.empty((nT,nMaxVeh),dtype='object')
#Link_no_veh_data[:] = np.nan
Lane_no_data = np.empty((nT,nMaxVeh))
Lane_no_data[:] = np.nan

isViewSet = 1

tic = time.time()
while simTime <= tSimuEnd and iT < nT:
    traci.simulation.step()
    simTime = traci.simulation.getTime()
    #print("current simulation time %f" % (simTime))
    t_0 = simTime
    
    vehId = traci.vehicle.getIDList()
    if len(vehId):
                
        for iV in range(0,len(vehId)):
            idStr = vehId[iV]
            idNum = int(idStr.split(".")[1])

            if idNum == 1:
                traci.gui.trackVehicle("View #0", idStr)
                isViewSet = 0
    
            t_data[iT,idNum] = simTime
            type_veh_data[iT,idNum] = traci.vehicle.getTypeID(idStr)
            no_veh_data[iT,idNum] = idNum; # existing vehicles' specified number in simulation
            pos_veh_data[iT,idNum] = traci.vehicle.getLanePosition(idStr)
            spd_veh_data[iT,idNum] = traci.vehicle.getSpeed(idStr)
            # spd_d_veh_arr = cell2mat(spd_d_veh_all(:,2));
            acc_veh_data[iT,idNum] = traci.vehicle.getAcceleration(idStr)
            Link_no_veh_data[iT,idNum] = traci.vehicle.getRoadID(idStr)
            Lane_no_data[iT,idNum] = traci.vehicle.getLaneIndex(idStr)
 
            #print(spd_veh_data[iT,idNum])
            
            #print("id name %s" % (vehId[iV]))

    iT = iT + 1

toc = time.time()
print("simu time %f" % (toc-tic))

traci.close()

plt.plot(t_data, spd_veh_data)
plt.show()

##for i in range(0,nVeh):
##    plt.plot(t, speed[:,i])
