'''

This script is to give a vehicle class exemption on route decisions.
i.e. vehicles of this class will not make left/right turns and always go straight.

'''

import pandas as pd
import os
from pandas.core.frame import DataFrame
import win32com.client as com


# This is the vehicle class to be exculded from the static route decisions
VEHICLE_CLASS_TO_EXEMPT = 70

#############################################################################
######################    PARAMETERS TO SET
#############################################################################
# !!!!!! change the absolute_folder_path
absolute_folder_path = os.path.abspath(os.path.join(os.getcwd(), r'.\VISSIMfiles')) # absolute folder path of the vissim .inpx and .layx files
file_name = 'RealSimToyotaTest_2021' # file name of vissim network and layout without extension .inpx and .layx

#############################################################################
######################    LAUNCH VISSIM
#############################################################################
# Load a Vissim Network & Layout
netname = '{}.inpx'.format(file_name)
NetFilename = os.path.join(absolute_folder_path, netname)
flag_read_additionally  = False # you can read network(elements) additionally, in this case set "flag_read_additionally" to true
layoutname = '{}.layx'.format(file_name)
LayoutFilename = os.path.join(absolute_folder_path, layoutname)

# Connecting the COM Server => Open a new Vissim Window:
# !!!! change VISSIM version if needed
Vissim = com.Dispatch("Vissim.Vissim-64.2100") 
Vissim.LoadNet(NetFilename, flag_read_additionally)
# Load a Layout
Vissim.LoadLayout(LayoutFilename)


# get all vehicle classes
VehicleClassesAll = Vissim.Net.VehicleClasses.GetAll()
VehicleClassInRouteStr = "" # all these vehicle classes will follow route decisions
for vehClass in VehicleClassesAll: 
    vehClassNo = vehClass.AttValue("No")
    if vehClassNo != VEHICLE_CLASS_TO_EXEMPT:
        VehicleClassInRouteStr = VehicleClassInRouteStr+str(vehClassNo)+","
VehicleClassInRouteStr = VehicleClassInRouteStr[:-1]

# loop over all routing decisions and disable the class specified
VehicleRoutingDecisionAll = Vissim.Net.VehicleRoutingDecisionsStatic.GetAll()

for vehRoute in VehicleRoutingDecisionAll:
    vehRoute.SetAttValue("AllVehTypes", 0)
    vehRoute.SetAttValue("VehClasses", VehicleClassInRouteStr)

# setup ends
print("Done! Exempted vehicle class {} from static route decisions!".format(VEHICLE_CLASS_TO_EXEMPT))


#############################################################################
######################    SAVE and EXIT VISSIM
#############################################################################
Vissim.SaveNet()
Vissim.Exit()