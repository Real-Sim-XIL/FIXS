'''

This script is to generate tables to help obtain speed limit from a VISSIM 

Speed limit in VISSIM can be related to desired speed decision or reduced speed areas

VISSIM vehicle is driven based on a speed distribution, so a speed limit value needs to be tied with each distribution first

The following tables are needed first
DesiredSpeedDistributionTable = 
    'Index': index of speed distribution (int)
    'Name': name of speed distribution (string)
    'SpeedLimitValue': speed limit value (float)
    'SpeedLimitUnit': unit, km/h or mph (string)

LinkGeometryTable = 
    'Index': link id (int)
    'Name': link name (string)
    'Length': length of link (float)
    'NumLanes': number of lanes (int)
    'IsConnector': whether it is connector (int)
    'FromLink': from link index, only for connector (int)
    'FromPos': from link position, only for connector (float)
    'ToLink': to link index, only for connector (int)
    'ToPos': to link position, only for connector (float)
 
Finally, the following speed limit table can be generated 
SpeedLimitTable = 
    'Link': link id (int) 
    'Lane': lane number (int)
    'Position': position on the link (float)
    'VehicleType': type of vehicle that comply (int)
    'SpeedLimitType': type of speed limit, currently only support 0: desired speed distribution (int)
    'SpeedLimitValue': value of speed limit, unit: [m/s],  (float)
    'TimeFrom': *not supported yet, simulation time this speed limit will be activated (float)
    'TimeTo': *not supported yet, simulation time this speed limit will be activated (float)


'''
#Note!! The script will try to interpret the speed limit value from 'name' of
#speed distribution, but in most cases, it needs user manual inputs to make sure
#a correct mapping from desired speed distribution to  

import pandas as pd
import numpy as np
import os
from pandas.core.frame import DataFrame
import win32com.client as com

#############################################################################
######################    PARAMETERS TO SET
#############################################################################
# !!!!!! change the absolute_folder_path
absolute_folder_path = os.path.abspath(r'.\tests\SpeedLimit') # absolute folder path of the vissim .inpx and .layx files
file_name = 'speedLimit' # file name of vissim network and layout without extension .inpx and .layx

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

# Activate QuickMode:
Vissim.Graphics.CurrentNetworkWindow.SetAttValue("QuickMode",0) # use 0 if want to see vehicles in simulation


#############################################################################
######################    INITIALIZE SpeedLimitTable
#############################################################################
DesiredSpeedDistributionTable = {
    'Index': [],
    'Name': [],
    'SpeedLimitValue': [],
    'SpeedLimitUnit': []
}

SpeedLimitTable = {'Link': [], 
    'Lane': [],
    'Position': [],
    'VehicleType': [],
    'SpeedLimitType': [],  
    'SpeedLimitValue': [],   
    'TimeFrom': [],
    'TimeTo': []
}

LinkGeometryTable = {
    'Index': [], 
    'Name': [],
    'Length': [],
    'NumLanes': [],
    'IsConnector': [],
    'FromLink': [],
    'FromPos': [],
    'ToLink': [],
    'ToPos': []
}

#############################################################################
######################    CREATE Link topology table
#############################################################################
LinkAll = Vissim.Net.Links.GetAll()
for link in LinkAll:
    LinkGeometryTable['Index'].append(link.AttValue('No'))
    LinkGeometryTable['Name'].append(link.AttValue('Name'))
    LinkGeometryTable['Length'].append(link.AttValue('Length2D'))
    LinkGeometryTable['NumLanes'].append(link.AttValue('NumLanes'))
    LinkGeometryTable['IsConnector'].append(link.AttValue('IsConn'))
    LinkGeometryTable['FromLink'].append(link.AttValue('FromLink'))
    LinkGeometryTable['FromPos'].append(link.AttValue('FromPos'))
    LinkGeometryTable['ToLink'].append(link.AttValue('ToLink'))
    LinkGeometryTable['ToPos'].append(link.AttValue('ToPos'))

LinkGeometryTableDf = pd.DataFrame.from_dict(LinkGeometryTable)
LinkGeometryTableDf["Index"]=LinkGeometryTableDf["Index"].astype(float).astype("Int32")
LinkGeometryTableDf["Name"]=LinkGeometryTableDf["Name"].astype(str)
LinkGeometryTableDf["Length"]=LinkGeometryTableDf["Length"].astype(float)
LinkGeometryTableDf["NumLanes"]=LinkGeometryTableDf["NumLanes"].astype(float).astype("Int32")
LinkGeometryTableDf["IsConnector"]=LinkGeometryTableDf["IsConnector"].astype(float).astype("Int32")
LinkGeometryTableDf["FromLink"]=LinkGeometryTableDf["FromLink"].astype(float).astype("Int32")
LinkGeometryTableDf["FromPos"]=LinkGeometryTableDf["FromPos"].astype(float)
LinkGeometryTableDf["ToLink"]=LinkGeometryTableDf["ToLink"].astype(float).astype("Int32")
LinkGeometryTableDf["ToPos"]=LinkGeometryTableDf["ToPos"].astype(float)


LinkGeometryTableDf.to_csv(os.path.join(absolute_folder_path, 'LinkGeometryTable.csv'), index=False)

#############################################################################
######################    CREATE DesiredSpeedDistribution to speed limit map
#############################################################################

# try to interpolate directly from Name if Name contains speed limit information
DesiredSpeedDistributionsAll = Vissim.Net.DesSpeedDistributions.GetAll()
for desiredSpeedDistribution in DesiredSpeedDistributionsAll:
    index = desiredSpeedDistribution.AttValue('No')
    name = desiredSpeedDistribution.AttValue('Name')

    DesiredSpeedDistributionTable['Index'].append(index)
    DesiredSpeedDistributionTable['Name'].append(name)
    if 'km/h' in name:
        try:
            DesiredSpeedDistributionTable['SpeedLimitValue'].append(float(name.split("km/h")[0]))
        except Exception as e: 
            print('could not auto-parse DesiredSpeedDistribution index {}, name {}: '. format(index, name))
            #print(e)
            DesiredSpeedDistributionTable['SpeedLimitValue'].append(None)
    else:
        DesiredSpeedDistributionTable['SpeedLimitValue'].append(None)
    DesiredSpeedDistributionTable['SpeedLimitUnit'].append('km/h')

# use the autoparsed table as the starting point
DesiredSpeedDistributionTableDf = pd.DataFrame.from_dict(DesiredSpeedDistributionTable)
DesiredSpeedDistributionTableDf.astype(dtype={'Index' : int, 
                'Name': str,
                'SpeedLimitValue': float,
                'SpeedLimitUnit': str})
DesiredSpeedDistributionTableDf.to_csv(os.path.join(absolute_folder_path, 'DesSpdDistr2SpeedLimitMap.csv'), index=False)

# if need manual edits, then wait until user finishes
if ENABLE_MANUAL_EDIT_DESIRED_SPEED_MAP:
    # if need user manual inputs to a csv file
    # wait until user finishes manually enter/correct speed limits
    isUserDone = 0
    userinput = input('\n\nPlease do manual edits of the generated DesSpdDistr2SpeedLimitMap.csv.\nEnter done after finishing: ') 
    while not isUserDone:
        if userinput.lower() == 'done'.lower(): 
            print('Done manual edits')
            isUserDone = 1
        else: 
            print("Please enter done after finishing:") 
    print("manual edits finished!\n\n")
    # after manually enter, then read the csv file
    DesiredSpeedDistributionTableDf = pd.read_csv(os.path.join(absolute_folder_path, 'DesSpdDistr2SpeedLimitMap.csv'))
    
#############################################################################
######################    RETREIVE SIGNAL HEADS
#############################################################################
#--------------------------------
# get vehicle class mapping to types
#--------------------------------
# class index --> list of types
VehicleClassMap = {}

VehicleClassesAll = Vissim.Net.VehicleClasses.GetAll()

for vehicleClass in VehicleClassesAll:
    # create empty list for current vehicle class
    VehicleClassMap[vehicleClass.AttValue('No')] = []
    tempStr = vehicleClass.AttValue('VehTypes')
    tempStrList = tempStr.split(',')
    for x in tempStrList:
        VehicleClassMap[vehicleClass.AttValue('No')].append(int(x))

DesiredSpeedDecisionsAll = Vissim.Net.DesSpeedDecisions.GetAll()
for desiredSpeedDecision in DesiredSpeedDecisionsAll:
    linklaneStr=desiredSpeedDecision.AttValue('Lane')
    temp = linklaneStr.split('-')
    link = int(temp[0])
    lane = int(temp[1])
    position = desiredSpeedDecision.AttValue('Pos')
    timeFrom = desiredSpeedDecision.AttValue('TimeFrom')
    timeTo = desiredSpeedDecision.AttValue('TimeTo')

    vehicleTypeList = []
    speedLimitValueList = []

    for vehClass, vehTypes in VehicleClassMap.items():
        # if DesSpeedDistr for current vehClass is not None
        curDesSpdDecStr = desiredSpeedDecision.AttValue('DesSpeedDistr({})'.format(vehClass))
        if curDesSpdDecStr:
            vehicleTypeList = vehicleTypeList + vehTypes
            # from speed distribution index to speed limit value
            curDesSpdDistrIndex = int(curDesSpdDecStr)
            curSpeedLimitUnit = DesiredSpeedDistributionTableDf.loc[DesiredSpeedDistributionTableDf['Index']==curDesSpdDistrIndex]['SpeedLimitUnit'].values[0]
            curSpeedLimitValue = DesiredSpeedDistributionTableDf.loc[DesiredSpeedDistributionTableDf['Index']==curDesSpdDistrIndex]['SpeedLimitValue'].values[0]
            # convert to km/h if unit in mph
            if curSpeedLimitUnit.lower()=='mph':
                curSpeedLimitValue = 0.44704*curSpeedLimitValue
            elif curSpeedLimitUnit.lower()=='km/h':
                curSpeedLimitValue = curSpeedLimitValue/3.6
            else:
                print("ERROR: speed limit unit must be either km/h or mph!")
            # save to speed limit list
            speedLimitValueList = speedLimitValueList + [curSpeedLimitValue]*len(vehTypes)
    
    for vehicleType, speedLimitValue in zip(vehicleTypeList, speedLimitValueList):
        SpeedLimitTable['Link'].append(link)
        SpeedLimitTable['Lane'].append(lane)
        SpeedLimitTable['Position'].append(position)
        SpeedLimitTable['VehicleType'].append(vehicleType)
        SpeedLimitTable['SpeedLimitType'].append(0)
        SpeedLimitTable['SpeedLimitValue'].append(speedLimitValue)
        SpeedLimitTable['TimeFrom'].append(timeFrom)
        SpeedLimitTable['TimeTo'].append(timeTo)


# 
# Vissim.Net.Links.ItemByKey(1).AttValue('NumLanes')


SpeedLimitTableDf = pd.DataFrame.from_dict(SpeedLimitTable)
SpeedLimitTableDf.astype(dtype={'Link' : int, 
                    'Lane': int,
                    'Position': float,
                    'VehicleType': int,
                    'SpeedLimitType': int,
                    'SpeedLimitValue': float,
                    'TimeFrom': float,
                    'TimeTo': float})
SpeedLimitTableDf.to_csv(os.path.join(absolute_folder_path, 'SpeedLimitTable.csv'), index=False)



#############################################################################
######################    Add position on link UDA
#############################################################################
# check if RealSim uda already set or not
realsimUdaExist = False
for uda in Vissim.Net.UserDefinedAttributes.GetAll():
    if uda.AttValue('NameShort') == "RealSim_Pos":
        realsimUdaExist = True
if not realsimUdaExist:
    Vissim.Net.UserDefinedAttributes.AddUserDefinedFormulaAttribute(23333, "Vehicle", "RealSim_Pos", "RealSim_PositionOnLink", 2, 0, "[POS]")
    Vissim.Net.UserDefinedAttributes.ItemByKey(23333).SetAttValue("Decimals", 10)



#############################################################################
######################    SAVE and EXIT VISSIM
#############################################################################
Vissim.SaveNet()
Vissim.Exit()

aa = 1
