'''
Script that needs to be run before doing a CarMaker simulation

Currently, use a brutal force approach to modify the testrun file to ensure maximum compatibility to previous CM versions.
Since CM 11, there is a provided Python Api to interact with testrun file more elegantly.
'''


import argparse
import os
import glob
import pathlib
import random
import xml.etree.ElementTree as ET
import pandas as pd

TrafficObjectTemplate = {'ObjectKind': 'Movable',
    'ObjectClass': 'Car',
    'Name': 'Car_000',
    'Info': '100',  # need update
    'Movie.Geometry': '3D/Vehicles/IPG_CompanyCar_2018_Blue.mobj',
    'Color': '1.0 0.0 0.0',
    'Basics.Dimension': '4.21 2.0 1.2',
    'Basics.Offset': '0.0 0.0',
    'Basics.Fr12CoM': '2.105',
    'Init.Orientation': '0.0 0.0 0.0',
    'Attrib': '100', # need update
    'RCSClass': 'RCS_Car',
    'DetectMask': '1 1',
    'Route': '0 0',
    'StartPos.Kind': 'Route',
    'StartPos.ObjId': '422',
    'StartPos': '0 0',
    'Init.v': '0',
    'FreeMotion': '1',
    'Lighting': '0',
    'IFF.FM_tx.Name': '',
    'IFF.FM_tx.Factor': '1.0',
    'IFF.FM_tx.Offset': '0.0',
    'IFF.FM_ty.Name': '',
    'IFF.FM_ty.Factor': '1.0',
    'IFF.FM_ty.Offset': '0.0',
    'IFF.FM_tz.Name': '',
    'IFF.FM_tz.Factor': '1.0',
    'IFF.FM_tz.Offset': '0.0',
    'IFF.FM_rx.Name': '',
    'IFF.FM_rx.Factor': '1.0',
    'IFF.FM_rx.Offset': '0.0',
    'IFF.FM_ry.Name': '',
    'IFF.FM_ry.Factor': '1.0',
    'IFF.FM_ry.Offset': '0.0',
    'IFF.FM_rz.Name': '',
    'IFF.FM_rz.Factor': '1.0',
    'IFF.FM_rz.Offset': '0.0'
}


CarObjectList = []
TruckObjectList = []
BusObjectList = []

def setupCarMaker(args):
    # modify the CarMaker testrun file to add traffic objects that will sync with sumo traffic 
    CmObj = addTrafficObjects(args)
    # create signal table that sync traffic signal id between CarMaker and Sumo
    createSignalTable(args, CmObj)

def createSignalTable(args, CmObj):
    ###
    ###
    # CmTrafficLightIndex -> CmTrafficLightIndex
    CmTrafficSignalTable = {
    }

    # read Cm road file to get Cm signal light information
    roadFile = os.path.join(args.cm_project_path, 'Data\\Road', CmObj['roadFile'])
    testrunFileOrig = os.path.join(args.cm_project_path, 'Data\\TestRun', args.testrun)

    # get all lines of current road file
    with open(roadFile, 'r') as fileHandler:
        lines = fileHandler.readlines()

        # get all traffic light controller
        for line in lines:
            if 'Control.TrfLight.' in line:
                CmTrafficSignalTable[line.split('=')[1].split()[1]] = int(line.split('Control.TrfLight.')[1].split('=')[0])

    ###
    ###
    # create signal table
    sumoFilePath = args.sumo_file_path

    sumoNetTree = ET.parse(sumoFilePath)
    sumoNetRoot = sumoNetTree.getroot()

    # dictionary to store traffic light table
    # Controller: each intersection has 1 controller
    # Group: essentially each group mean a phase of a controller
    # Head: each phase/group can have multiple heads
    #
    # CarMaker has two id
    #   traffic signal index is Nb in TrfLight.Objs[Nb], see CM ReferenceManual.pdf UAQ
    #   controller id is the id defined in the Scenario editor
    TrafficSignalTable_dict = {
        'SignalControllerId': [],
        'SignalGroupId': [],
        'SignalHeadId': [],
        'CmTrafficLightIndex': [],
        'CmControllerId': []
    }


    # loop over tlLogic
    for tl in sumoNetRoot.findall('tlLogic'):
        if tl.find('phase') is None:
           print('Cannot find tlLogic in Sumo file!')
           return
        
        for id, s in enumerate(tl.find('phase').attrib['state']):
            TrafficSignalTable_dict['SignalControllerId'].append(tl.attrib['id'])
            TrafficSignalTable_dict['SignalGroupId'].append(-1)
            TrafficSignalTable_dict['SignalHeadId'].append(id)
            CmControllerId = '{}_{}'.format(tl.attrib['id'], id)
            if CmControllerId in CmTrafficSignalTable.keys():
                TrafficSignalTable_dict['CmTrafficLightIndex'].append(CmTrafficSignalTable[CmControllerId])
                TrafficSignalTable_dict['CmControllerId'].append(CmControllerId)
            else:
                TrafficSignalTable_dict['CmTrafficLightIndex'].append(-1)
                TrafficSignalTable_dict['CmControllerId'].append('')

    # output
    TrafficSignalTableDf = pd.DataFrame.from_dict(TrafficSignalTable_dict)
    TrafficSignalTableDf.astype(dtype={'SignalControllerId' : str, 
                    'SignalGroupId': int,
                    'SignalHeadId': int,
                    'CmTrafficLightIndex': int,
                    'CmControllerId': str})
    TrafficSignalTableDf.to_csv(os.path.join(args.cm_project_path, 'Data\\Road', CmObj['roadFile'].split('.')[0]+'_RSsignalTable.csv'), index=False)

    pass

def addTrafficObjects(args):
    # fix random seed so that have repeated testrun
    random.seed(42)

    # read args
    nCar = int(args.car)
    nTruck = int(args.truck)
    isNoRandomTraffic = args.no_random_traffic
    isOverwriteTestrun = args.overwrite_testrun

    testrunFileOrig = os.path.join(args.cm_project_path, 'Data\\TestRun', args.testrun)
    if isOverwriteTestrun:
        testrunFileNew = testrunFileOrig
    elif args.output_testrun:
        testrunFileNew = os.path.join(args.cm_project_path, 'Data\\TestRun', args.output_testrun)
    else:
        testrunFileNew = os.path.join(args.cm_project_path, 'Data\\TestRun', args.testrun+'_RS')
    cmInstallPath = args.cm_install_path

    nTraffic = 0
    cmVersion='11.0.1'
    # get all lines of current testrun file
    with open(testrunFileOrig) as fileHandler:
        lines = fileHandler.readlines()

    # remove traffic defined in current testrun file
    with open(testrunFileNew, 'w+') as fileHandler:
        for line in lines:
            name, var = line.strip().partition("=")[::2]
            if 'FileCreator' in line:
                cmVersion = var.split()[-1]
            if 'Traffic.N' in line:
                nTraffic = int(var)
            if any(['Traffic.{}'.format(i) in line for i in range(nTraffic)]):
                pass
            else:
                fileHandler.write(line)

    # get possible Traffic Object 3D models
    cmVehiclePath = os.path.join(cmInstallPath, 'carmaker\win64-{}'.format(cmVersion), r'Movie\3D\Vehicles')
    CarFixIdx = 0
    TruckFixIdx = 0
    for file in pathlib.Path(cmVehiclePath).glob('*.objinfo'):
        # to avoid select vehicle parts obj files, only use those have both mobj and objinfo
        mobjFile = file.parent.joinpath(file.name.replace('objinfo','mobj'))
        if not mobjFile.is_file():
            continue
        # parser new object
        curObj = TrafficObjectTemplate.copy()
        with open(file) as fileHandler:
            curObj['Movie.Geometry'] = '3D/Vehicles/'+file.name.replace('objinfo','mobj')
            for line in fileHandler:
                name, var = line.strip().partition("=")[::2]
                if 'ObjectKind' in name: curObj['ObjectKind'] = var.strip()
                if 'ObjectClass' in name: curObj['ObjectClass'] = var.strip()
                if 'Basics.Dimension' in name: curObj['Basics.Dimension'] = var
                if 'Basics.Offset' in name: curObj['Basics.Offset'] = var
                if 'Basics.Fr12CoM' in name: curObj['Basics.Fr12CoM'] = var
                if 'RCSClass' in name: curObj['RCSClass'] = var
                if 'DetectMask' in name: curObj['DetectMask'] = var

        # if not movable, skip
        if curObj['ObjectKind'] != 'Movable':
            continue
        # list to remove
        objToNeglect = ['Forklift', 'Trailer', 'Damaged', 'JohnDeere', 'Volvo_Vera', 'Wrecker', 'McLaren_MP4', 'EasyMile', 'Navya', 'Caterpillar', 'HL757', 'R220']
        if any(o in curObj['Movie.Geometry'] for o in objToNeglect):
            continue

        # find default car and truck if no random traffic 
        # if 'IPG_CompanyCar_2018' in curObj['Movie.Geometry']:
        if 'IPG_CompanyCar_2018' in curObj['Movie.Geometry']:
            CarFixIdx = len(CarObjectList)
        if 'MB_Actros_8x4_Dumper' in curObj['Movie.Geometry']:
            TruckFixIdx = len(TruckObjectList)

        if curObj['ObjectClass'] == 'Car':
            CarObjectList.append(curObj)
        elif curObj['ObjectClass'] == 'Truck':
            TruckObjectList.append(curObj)
        elif curObj['ObjectClass'] == 'Bus':
            BusObjectList.append(curObj)


    with open(testrunFileNew)  as fileHandler:
        lines = fileHandler.readlines()

    # write new traffic objects
    with open(testrunFileNew, 'w') as fileHandler:
        for line in lines:
            if 'Traffic.N' in line:
                fileHandler.write('Traffic.N = {}\n'.format(nCar+nTruck))
                fileHandler.write('Traffic.SpeedUnit = ms\n')
                for iT in range(nCar):        
                    if isNoRandomTraffic:
                        idx = CarFixIdx           
                    else:
                        idx = random.randrange(len(CarObjectList))
                    currentTrafficObject = CarObjectList[idx].copy()
                    currentTrafficObject['Name'] = 'RS_C{:03d}'.format(iT)
                    currentTrafficObject['Info'] = 'RealSimCar {}'.format(iT)
                    for k in currentTrafficObject.keys():
                        fileHandler.write('Traffic.{}.{} = {}\n'.format(iT, k, currentTrafficObject[k]))

                for iT in range(nCar, nCar+nTruck):                   
                    if isNoRandomTraffic:
                        idx = TruckFixIdx           
                    else:
                        idx = random.randrange(len(TruckObjectList))
                    currentTrafficObject = TruckObjectList[idx].copy()
                    currentTrafficObject['Name'] = 'RS_T{:03d}'.format(iT-nCar)
                    currentTrafficObject['Info'] = 'RealSimTruck {}'.format(iT-nCar)
                    for k in currentTrafficObject.keys():
                        fileHandler.write('Traffic.{}.{} = {}\n'.format(iT, k, currentTrafficObject[k]))

            elif 'Traffic.SpeedUnit' in line:
                pass
            else:
                fileHandler.write(line)

    print("RealSim Parser Completed: generated new testrun {}".format(args.output_testrun))

    # output
    CmObj = {}
    # get name of road file
    with open(testrunFileNew, 'r') as fileHandler:
        for line in lines:
            if 'Road.FName' in line:
                CmObj['roadFile'] = line.split()[-1]
                break

    return CmObj




if __name__ == '__main__':
    argparser = argparse.ArgumentParser(description=__doc__)
    argparser.add_argument('--cm-project-path', '-p', 
                        metavar = 'PATH', 
                        type=str, 
                        help='CM project path')
    argparser.add_argument('--cm-install-path', 
                        metavar = 'PATH', 
                        type=str, 
                        default='C:\\IPG',
                        help='CM installation path')
    argparser.add_argument('--testrun', '-t',
                        metavar = 'INFOFILE',
                        type=str,
                        help='CM testrun name')
    argparser.add_argument('--output-testrun',
                        metavar = 'INFOFILE',
                        type=str,
                        help='output CM testrun name')
    argparser.add_argument('--no-random-traffic',
                        action='store_true',
                        help='disable random traffic objects')
    argparser.add_argument('--car',
                        metavar="NUM_CAR",
                        default=30,
                        help='set number of car created (default: 30)')
    argparser.add_argument('--truck',
                        metavar="NUM_TRUCK",
                        default=10,
                        help='set number of truck created (default: 10)')
    argparser.add_argument('--overwrite-testrun',
                        action='store_true',
                        help='if set original testrun file will be overwritten')
    argparser.add_argument('--sumo-file-path',
                        metavar = 'PATH', 
                        type=str, 
                        help='path to sumo file that contains tlLogic')
    arguments = argparser.parse_args()

    setupCarMaker(arguments)


# TrafficObjectTemplate = {'ObjectKind': 'Movable',
# 	'ObjectClass': 'Car',
# 	'Name': 'T14',
# 	'Info': 'UNNAMED Object 14',
# 	'Movie.Geometry': '3D/Vehicles/VW_Beetle_2012_Blue.mobj',
# 	'Color': '1.0 0.0 0.0',
# 	'Basics.Dimension': '4.28 1.82 1.28',
# 	'Basics.Offset': '0.19 0.0',
# 	'Basics.Fr12CoM': '2.15',
# 	'Init.Orientation': '0.0 0.0 0.0',
# 	'RCSClass': 'RCS_Car',
# 	'DetectMask': '1 1',
# 	'Route': '0 1',
# 	'StartPos.Kind': 'Route',
# 	'StartPos.ObjId': '422',
# 	'StartPos': '70 0',
# 	'Init.v': '0.0',
# 	'FreeMotion': '0',
# 	'Lighting': '0',
# 	'UpdRate': '200',
# 	'Motion.Kind': '4Wheel',
# 	'Motion.mass': '1530.0',
# 	'Motion.I': '470.0  2080.0  2160.0',
# 	'Motion.Overhang': '0.7  0.75',
# 	'Motion.Cf': '1.4e5',
# 	'Motion.Cr': '1.2e5',
# 	'Motion.C_roll': '1.6e5',
# 	'Motion.D_roll': '1.6e4',
# 	'Motion.C_pitch': '2.3e5',
# 	'Motion.D_pitch': '2.3e4',
# 	'Motion.SteerCtrl.Ang_max': '40.0',
# 	'Motion.AnimateHitch': '0',
# 	'Man.TreatAtEnd': 'FreezePos',
# 	'Man.N': '0'
# }
