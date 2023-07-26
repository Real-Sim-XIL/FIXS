import os, sys
import time
import numpy as np
import math
import matplotlib.pyplot as plt

# we need to import python modules from the $SUMO_HOME/tools directory
if 'SUMO_HOME' in os.environ:
    tools = os.path.join(os.environ['SUMO_HOME'], 'tools')
    sys.path.append(tools)
    sys.path.append(os.path.join(tools, 'libtraci'))
else:
    sys.exit("please declare environment variable 'SUMO_HOME'")
    
#import traci
import libtraci as traci

dt = .1
tSimuEnd = 90000
nT = math.ceil(tSimuEnd/dt)
nMaxVeh = 100

#if __name__ == "__main__":
sceneName = r"./SUMOfiles/UA network/UAdemoRealSim_Final.sumocfg "
#traci.start(["sumo-gui", "-c", sceneName, "--start", "--step-length", str(dt)])
#traci.start(["C:/sumo_orig/bin/sumo-gui", "-c", sceneName, "--start", "--step-length", str(dt)])
#traci.start(["C:/sumo_orig/bin/sumo-gui", "-c", sceneName, "--start", "--step-length", str(dt), "--num-clients", "2"], port=int(1337))

#PORT = int(1337)
#traci.init(PORT)

PORT = int(1337)
traci.init(PORT,host="localhost")

##PORT = int(1337)
####traci.init(PORT,host="192.168.0.33")
###traci.init(PORT,host="192.168.0.15")
##traci.init(PORT,host="192.168.140.100")

traci.setOrder(2)
    
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


egoIdStr = '0_2'

t = np.array([])
speed = np.array([])
speedWithoutTraci = np.array([])

accelSet = np.array([])

isViewSet = 1

tic = time.time()
while simTime <= tSimuEnd and iT < nT:
    try:
        traci.simulation.step()
        simTime = traci.simulation.getTime()
        #print("current simulation time %f" % (simTime))
        t_0 = simTime
        
        vehId = traci.vehicle.getIDList()
        if len(vehId):
                    
            for iV in range(0,len(vehId)):
                idStr = vehId[iV]
                #idNum = int(idStr.split(".")[1])

                # if idNum == 1:
                    # traci.gui.trackVehicle("View #0", idStr)
                    # isViewSet = 0
        
                if idStr == egoIdStr:
                    t = np.append(t, simTime)
                    curSpeed = traci.vehicle.getSpeed(idStr)
                    curAccel = traci.vehicle.getAcceleration(idStr)
                    speed = np.append(speed, curSpeed)
                    speedWithoutTraci = np.append(speedWithoutTraci, traci.vehicle.getSpeedWithoutTraCI(idStr))
                    
                    # actionStepLength = traci.vehicle.getActionStepLength(idStr)
                    # stepLength = traci.vehicle.getLastActionTime(idStr)
                    # traci.vehicle.setAcceleration(idStr, math.sin(simTime)+2, actionStepLength-0.1)
                    # accelSet = np.append(accelSet, )

                    # traci.vehicle.setSpeedMode(idStr, 32)
                    # traci.vehicle.setPreviousSpeed(idStr, curSpeed+0.1)
                    
                    print('simTime {}, speed {}, acceleration {}'.format(simTime, curSpeed, curAccel))

                    aa = 1
                    

        iT = iT + 1
    except Exception as e:
        print(str(e))
        break

toc = time.time()
print("simu time %f" % (toc-tic))

#traci.close()

#%matplotlib tk
fig = plt.figure()
plt.plot(t, speed)
plt.plot(t, speedWithoutTraci)
plt.show()
#plt.xlim([0, 18])


# fig = plt.figure()
# plt.plot(t, speed)

aa = 1
##for i in range(0,nVeh):
##    plt.plot(t, speed[:,i])
