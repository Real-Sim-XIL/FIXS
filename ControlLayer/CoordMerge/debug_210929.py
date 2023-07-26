import matplotlib.pyplot as plt
import matplotlib
matplotlib.use('TkAgg')

import pandas as pd

columnNames = ['t', 'vehType','vDes', 'pos', 'tfs_set', 'tfs', 'tfs_fix', 'tfs_up', 'v0', 's0', 'vf', 'sf', 'tfs_th', 'leadVehNo', 'tfs_prev', 'tfs_prc']

rampSimu = pd.read_csv("flow_1.9_simu.txt",sep=";")
rampExp = pd.read_csv("flow_1.9_.txt",sep=";")

rampSimu = pd.read_csv("flow_0.10_simu.txt",sep=";")
rampExp = pd.read_csv("flow_0.10_.txt",sep=";")

rampSimu.columns = columnNames
rampExp.columns = columnNames

plt.plot(rampSimu['t'], rampSimu['vDes'])
plt.plot(rampExp['t'], rampExp['vDes'])
plt.show()

plt.plot(rampSimu['t'], rampSimu['v0'])
plt.plot(rampExp['t'], rampExp['v0'])
plt.show()

aa = 1