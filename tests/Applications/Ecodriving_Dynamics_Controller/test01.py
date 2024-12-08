# read and print the csvs
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

# read the csvs
df1 = pd.read_csv('./speed_controller_dynamics.csv')
# set the column names
df1.columns = ['speed']
df2 = pd.read_csv('./speed_controller.csv')
df2.columns = ['speed']
x_lim = min(df1.shape[0], df2.shape[0])
# # print the csvs which are speed files
fig = plt.figure()
# print on the same figure
plt.plot(df1['speed'], label='speed_controller_dynamics')
plt.plot(df2['speed'], label='speed_controller')
plt.xlim(0, x_lim)
plt.legend()
plt.show()
