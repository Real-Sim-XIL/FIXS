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
tSimuEnd = 10000
nT = math.ceil(tSimuEnd/dt)
nMaxVeh = 100

#if __name__ == "__main__":
sceneName = "./tests/SpeedLimit/speedLimit.sumocfg "
traci.start(["sumo-gui", "-c", sceneName, "--start", "--step-length", str(dt)])

traci.setOrder(1)
    
simTime = 0
iT = 0

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

            #print(spd_veh_data[iT,idNum])
            
            #print("id name %s" % (vehId[iV]))

    iT = iT + 1

toc = time.time()
print("simu time %f" % (toc-tic))

traci.close()

##for i in range(0,nVeh):
##    plt.plot(t, speed[:,i])
