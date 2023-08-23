'''

This script is to obtain a signal table from a VISSIM simulation file

The table will be saved in an csv file

Columns

    SignalHeadIndex: index of signal head (int)

    SignalHeadName: name of signal head (string)

    SignalGroupIndex: index of signal group (int)
    
    SignalGroupName: name of signal group (string)
    
    SignalControllerIndex: index of signal controller it belongs to (int)
    
    SignalControllerName: name of signal controller (string)
    
    NumberOfSignalGroup: total number of signal group of current signal controller (int)
    

'''

import pandas as pd
import os
from pandas.core.frame import DataFrame
import win32com.client as com

#############################################################################
######################    PARAMETERS TO SET
#############################################################################
# !!!!!! change the absolute_folder_path
absolute_folder_path = os.path.abspath(r'..\VISSIMfiles') # absolute folder path of the vissim .inpx and .layx files
file_name = 'temp_control_dev2_debug' # file name of vissim network and layout without extension .inpx and .layx

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
Vissim = com.Dispatch("Vissim.Vissim-64.1100") 
Vissim.LoadNet(NetFilename, flag_read_additionally)
# Load a Layout
Vissim.LoadLayout(LayoutFilename)

# Activate QuickMode:
Vissim.Graphics.CurrentNetworkWindow.SetAttValue("QuickMode",0) # use 0 if want to see vehicles in simulation


#############################################################################
######################    INITIALIZE SIGNAL TABLE
#############################################################################
SignalTable = {'SignalHeadIndex': [], 
    'SignalHeadName': [],
    'SignalGroupIndex': [],
    'SignalGroupName': [],  
    'SignalControllerIndex': [],   
    'SignalControllerName': [],  
    'NumberOfSignalGroup': []
    }

#############################################################################
######################    RETREIVE SIGNAL HEADS
#############################################################################


SignalHeadAll = Vissim.Net.SignalHeads.GetAll()

for sigHead in SignalHeadAll:
    sigHeadIndex = sigHead.AttValue('No')
    sigHeadName = sigHead.AttValue('Name')
    
    sgFullName = sigHead.AttValue('SG')
    # SG full name in the format:
    #   x - y
    # x     index of signal controller
    # y     index of signal group
    temp = list(map(int, sgFullName.split('-')))
    sigCtrlIndex = temp[0]
    sigGroupIndex = temp[1]

    sigCtrl = Vissim.Net.SignalControllers.ItemByKey(sigCtrlIndex)
    sigCtrlName = sigCtrl.AttValue('Name')

    sigGroup = sigCtrl.SGs.ItemByKey(sigGroupIndex)
    sigGroupName = sigGroup.AttValue('Name')

    nSigGroup = sigCtrl.SGs.count

    # 
    SignalTable['SignalHeadIndex'].append(sigHeadIndex)
    SignalTable['SignalHeadName'].append(sigHeadName)
    SignalTable['SignalGroupIndex'].append(sigGroupIndex)
    SignalTable['SignalGroupName'].append(sigGroupName)
    SignalTable['SignalControllerIndex'].append(sigCtrlIndex)
    SignalTable['SignalControllerName'].append(sigCtrlName)
    SignalTable['NumberOfSignalGroup'].append(nSigGroup)


SignalTableDf = pd.DataFrame.from_dict(SignalTable)
SignalTableDf.astype(dtype={'SignalHeadIndex' : int, 
                 'SignalHeadName': str,
                 'SignalGroupIndex': int,
                 'SignalGroupName': str,
                 'SignalControllerIndex': int,
                 'SignalControllerName': str,
                 'NumberOfSignalGroup': int})
SignalTableDf.to_csv(os.path.join(absolute_folder_path, 'SignalTable.csv'), index=False)

aa = 1