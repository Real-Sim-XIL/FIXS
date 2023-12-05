import os, sys
import time
import numpy as np
import math
import matplotlib.pyplot as plt
import plotly.graph_objects as go
from plotly.subplots import make_subplots
import plotly.express as px
import pandas as pd

# we need to import python modules from the $SUMO_HOME/tools directory
if 'SUMO_HOME' in os.environ:
    tools = os.path.join(os.environ['SUMO_HOME'], 'tools')
    sys.path.append(tools)
else:
    sys.exit("please declare environment variable 'SUMO_HOME'")
    
import traci

dt = .1
tSimuEnd = 120
nT = math.ceil(tSimuEnd/dt)
nMaxVeh = 100

#if __name__ == "__main__":
sceneName = r".\tests\SignalIpg\sumoFiles\debug_elevation.sumocfg "
traci.start(["sumo-gui", "-c", sceneName, "--start", "--step-length", str(dt)])

traci.setOrder(1)
    
simTime = 0
iT = 0

isViewSet = 1

posX = []
posY = []
posZ = []
dist = []

tic = time.time()
while simTime <= tSimuEnd and iT < nT:
    traci.simulation.step()
    simTime = traci.simulation.getTime()
    #print("current simulation time %f" % (simTime))
    t_0 = simTime

    vehId = traci.vehicle.getIDList()

    if len(vehId):
        traciPos = traci.vehicle.getPosition3D(vehId[0])

        dist.append(traci.vehicle.getDistance(vehId[0]))
        posX.append(traciPos[0])
        posY.append(traciPos[1])
        posZ.append(traciPos[2])

    # if len(vehId):
                
    #     for iV in range(0,len(vehId)):
    #         idStr = vehId[iV]

    #         #print(spd_veh_data[iT,idNum])
            
    #         #print("id name %s" % (vehId[iV]))

    iT = iT + 1

toc = time.time()
print("simu time %f" % (toc-tic))

traci.close()

CmDf = pd.read_csv(r'.\tests\SignalIpg\debug_cm_data3.csv')

# plotting
__subPlotSizeMap = {
    1: [1,1],
    2: [2,1],
    3: [3,1],
    4: [4,1],
    5: [2,3],
    6: [2,3],
    7: [2,4],
    8: [2,4],
    9: [2,5],
    10: [2,5]
    }

__subPlotRowColMap = {
    1: [[1,1]],
    2: [[1,1],[2,1]],
    3: [[1,1],[2,1],[3,1]],
    4: [[1,1],[2,1],[3,1],[4,1]],
    5: [[1,1],[1,2],[1,3],[2,1],[2,2]],
    6: [[1,1],[1,2],[1,3],[2,1],[2,2],[2,3]],
    7: [[1,1],[1,2],[1,3],[1,4],[2,1],[2,2],[2,3]],
    8: [[1,1],[1,2],[1,3],[1,4],[2,1],[2,2],[2,3],[2,4]],
    9: [[1,1],[1,2],[1,3],[1,4],[1,5],[2,1],[2,2],[2,3],[2,4]],
    10: [[1,1],[1,2],[1,3],[1,4],[1,5],[2,1],[2,2],[2,3],[2,4],[2,5]],
    }

# nPlot = 3
# plotNames = ['posX', 'posY', 'posZ']
nPlot = 1
plotNames = ['posZ']
SUB_ROW = __subPlotSizeMap[nPlot][0]; SUB_COL = __subPlotSizeMap[nPlot][1]; iSub = 0

rc = __subPlotRowColMap[nPlot]

fig = make_subplots(rows=SUB_ROW, cols=SUB_COL, subplot_titles=(('plot'),)*nPlot,shared_xaxes=True, x_title="distance (m)")

# fig.add_trace(
#             go.Scatter(x=dist, y=posX, mode='lines+markers', \
#                 name= plotNames[iSub]+'sumo',marker_size=3),
#             row=rc[iSub][0], col=rc[iSub][1]
#         )
# fig.add_trace(
#             go.Scatter(x=CmDf['Car_Distance'], y=CmDf['Car_tx'], mode='lines+markers', \
#                 name= plotNames[iSub]+'cm',marker_size=3),
#             row=rc[iSub][0], col=rc[iSub][1]
#         )
# fig.layout.annotations[iSub].update(text=plotNames[iSub])
# iSub = iSub + 1

# fig.add_trace(
#             go.Scatter(x=dist, y=posY, mode='lines+markers', \
#                 name= plotNames[iSub]+'sumo',marker_size=3),
#             row=rc[iSub][0], col=rc[iSub][1]
#         )
# fig.add_trace(
#             go.Scatter(x=CmDf['Car_Distance'], y=CmDf['Car_ty'], mode='lines+markers', \
#                 name= plotNames[iSub]+'cm',marker_size=3),
#             row=rc[iSub][0], col=rc[iSub][1]
#         )
# fig.layout.annotations[iSub].update(text=plotNames[iSub])
# iSub = iSub + 1

fig.add_trace(
            go.Scatter(x=dist, y=[x+0.67 for x in posZ], mode='lines+markers', \
                name= plotNames[iSub]+'sumo',marker_size=3),
            row=rc[iSub][0], col=rc[iSub][1]
        )
fig.add_trace(
            go.Scatter(x=CmDf['Car_Distance'], y=CmDf['Car_tz'], mode='lines+markers', \
                name= plotNames[iSub]+'cm',marker_size=3),
            row=rc[iSub][0], col=rc[iSub][1]
        )
fig.layout.annotations[iSub].update(text=plotNames[iSub])
iSub = iSub + 1

fig.update_layout(height=1000, width=1000,
                title_text="")
fig.show()

pass