import yaml
import argparse
import os
import shutil

def setCarMakerConfig(args):
    
    with open(args.configFile, 'r') as file:
        Config = yaml.safe_load(file)

    RS_tmp_folder = 'RS_tmp'
    RealSimTmpPath = os.path.join(args.cm_project_path, RS_tmp_folder)
    os.makedirs(RealSimTmpPath, exist_ok=True)
    SignalTableFile = os.path.join(args.signal_table_path)
    RealSimCarMakerConfigName = os.path.join(RealSimTmpPath, 'RealSimCarMakerConfig.txt')

    TrafficLayerIP = ''
    TrafficLayerPort = ''
    CarMakerPort = ''
    TrafficSignalPort = ''
     
    # sanity check to make sure field XilSetup and ApplicationSetup exist
    if 'XilSetup' not in Config.keys():
        Config['XilSetup'] = {}
        Config['XilSetup']['EnableXil'] = False
    
    if 'ApplicationSetup' not in Config.keys():
        Config['ApplicationSetup'] = {}
        Config['ApplicationSetup']['EnableApplicationLayer'] = False
    
    if 'EnableXil' not in Config['XilSetup'].keys():
        Config['XilSetup']['EnableXil'] = False
    
    if 'EnableApplicationLayer' not in Config['ApplicationSetup'].keys():
        Config['ApplicationSetup']['EnableApplicationLayer'] = False
    
    
    # get input arguments to XilSetup
    if Config['XilSetup']['EnableXil']:
        if len(Config['XilSetup']['VehicleSubscription']) <= 0:
            print('\nERROR! must specify one vehicle subscription for XilSetup!\n')
            return -1
        elif len(Config['XilSetup']['VehicleSubscription']) == 1:
            TrafficLayerIP = Config['XilSetup']['VehicleSubscription'][0]['ip'][0]
            TrafficLayerPort = Config['XilSetup']['VehicleSubscription'][0]['port'][0]
            EgoId = Config['XilSetup']['VehicleSubscription'][0]['attribute']['id'][0]
            CarMakerPort = TrafficLayerPort
        elif len(Config['XilSetup']['VehicleSubscription']) == 2:
            if 'CarMakerSetup' not in Config.keys():
                print('\nERROR! if two vehicle subscription are defined, CarMakerSetup must be specified and EnableEgoSimulink must be true!\n')
                return -1
                    
            if 'EnableEgoSimulink' not in Config['CarMakerSetup'].keys(): 
                Config['CarMakerSetup']['EnableEgoSimulink'] = False
            
            if 'CarMakerIP' in Config['CarMakerSetup'].keys() and 'CarMakerPort' in Config['CarMakerSetup'].keys():
                for i in range(0, len(Config['XilSetup']['VehicleSubscription'])):
                    if not ((Config['XilSetup']['VehicleSubscription'][i]['ip'][0] == Config['CarMakerSetup']['CarMakerIP']) and \
                            Config['XilSetup']['VehicleSubscription'][i]['port'][0] == Config['CarMakerSetup']['CarMakerPort']):
                        TrafficLayerIP = Config['XilSetup']['VehicleSubscription'][i]['ip'][0]
                        TrafficLayerPort = Config['XilSetup']['VehicleSubscription'][i]['port'][0]
                    
                    if Config['XilSetup']['VehicleSubscription'][i]['port'][0] == Config['CarMakerSetup']['CarMakerPort']:
                        CarMakerPort = Config['XilSetup']['VehicleSubscription'][i]['port'][0]
                        EgoId = Config['XilSetup']['VehicleSubscription'][i]['attribute']['id'][0]
            else:
                print('\nERROR! must specify CarMaker IP and Port when 2 vehicle subscriptions are defined!\n')
                return -1
        else:
            print('\nERROR! currently only support at one vehicle subscription for Simulink, and at most two for CarMaker Simulink!\n')
            return -1
        
        # handle signal subscription
        if 'SignalSubscription' in Config['XilSetup'].keys():
            if len(Config['XilSetup']['SignalSubscription']) == 1:
                TrafficSignalPort = Config['XilSetup']['SignalSubscription'][0]['port'][0]
            elif 'CarMakerIP' in Config['CarMakerSetup'].keys() and 'TrafficSignalPort' in Config['CarMakerSetup'].keys():
                    for i in range(0, len(Config['XilSetup']['SignalSubscription'])):
                        if ((Config['XilSetup']['SignalSubscription'][i]['ip'][0] == Config['CarMakerSetup']['CarMakerIP']) and \
                                Config['XilSetup']['SignalSubscription'][i]['port'][0] == Config['CarMakerSetup']['TrafficSignalPort']):
                            TrafficSignalPort = Config['XilSetup']['SignalSubscription'][i]['port'][0]
            else:
                print('\nERROR! must specify TrafficSignalPort when multiple signal subscriptions are defined!\n')
                return -1
    
    elif Config['ApplicationSetup']['EnableApplicationLayer'] and not Config['XilSetup']['EnableXil']:
        if len(Config['ApplicationSetup']['VehicleSubscription']) <= 0:
            print('\nERROR! must specify one vehicle subscription for ApplicationSetup!\n')
        elif len(Config['XilSetup']['VehicleSubscription']) > 1:
            print('\nERROR! currently only support one vehicle subscription for ApplicationSetup for Simulink!\n')
        else:
            TrafficLayerIP = Config['ApplicationSetup']['VehicleSubscription'][0]['ip'][0]
            TrafficLayerPort = Config['ApplicationSetup']['VehicleSubscription'][0]['port'][0]
        
    else:
        print('Real-Sim: must at least enable Application layer or Xil layer!')
        return -1
    
    
    # write to a CarMaker text file 
    if 'CarMakerSetup' in Config.keys():
        # if CarMaker, then write a plain text file with configurations
        # so that CarMaker scripts can be compiled on dSPACE without
        # need of special YAML parser libraries

        # handle default values if not defined in config.yaml
        if 'EnableCosimulation' not in Config['CarMakerSetup'].keys(): 
            Config['CarMakerSetup']['EnableCosimulation'] = False 
        if 'EnableEgoSimulink' not in Config['CarMakerSetup'].keys():
            Config['CarMakerSetup']['EnableEgoSimulink'] = True
        if 'EgoId' not in Config['CarMakerSetup'].keys():
            Config['CarMakerSetup']['EgoId'] = EgoId 
        if 'EgoType' not in Config['CarMakerSetup'].keys():
            Config['CarMakerSetup']['EgoType'] = '' 
        if 'TrafficLayerIP' not in Config['CarMakerSetup'].keys():
            Config['CarMakerSetup']['TrafficLayerIP'] = TrafficLayerIP 
        if 'CarMakerPort' not in Config['CarMakerSetup'].keys():
            Config['CarMakerSetup']['CarMakerPort'] = CarMakerPort 
        if 'TrafficRefreshRate' not in Config['CarMakerSetup'].keys():
            Config['CarMakerSetup']['TrafficRefreshRate'] = 0.001 
        if 'TrafficSignalPort' not in Config['CarMakerSetup'].keys():
            Config['CarMakerSetup']['TrafficSignalPort'] = TrafficSignalPort
        if 'SynchronizeTrafficSignal' not in Config['CarMakerSetup'].keys(): 
            Config['CarMakerSetup']['SynchronizeTrafficSignal'] = False 

        # create full path to SignalTable file on the dSPACE RT-Linux OS path
        tmp = os.path.abspath(args.cm_project_path).split('\\')
        #SignalTablePathDS = os.path.join('/', tmp[-2],tmp[-1],RS_tmp_folder, SignalTableFile.split('\\')[-1]).replace('\\', '/')
        SignalTablePathDS = SignalTableFile.split('\\')[-1]

        # save to text file
        with open(RealSimCarMakerConfigName, 'w') as file:
            VehMsgStrTmp = Config['SimulationSetup']['VehicleMessageField'][0]
            for i in range(1,len(Config['SimulationSetup']['VehicleMessageField'])):
                VehMsgStrTmp = VehMsgStrTmp+','+Config['SimulationSetup']['VehicleMessageField'][i]
            
            file.write('VehicleMessageField={}\n'.format(VehMsgStrTmp))
            file.write('EnableCosimulation={}\n'.format(Config['CarMakerSetup']['EnableCosimulation']))
            file.write('EnableEgoSimulink={}\n'.format(Config['CarMakerSetup']['EnableEgoSimulink']))
            file.write('EgoId={}\n'.format(Config['CarMakerSetup']['EgoId']))
            file.write('EgoType={}\n'.format(Config['CarMakerSetup']['EgoType']))
            file.write('TrafficLayerIP={}\n'.format(Config['CarMakerSetup']['TrafficLayerIP']))
            file.write('CarMakerPort={}\n'.format(Config['CarMakerSetup']['CarMakerPort']))
            file.write('TrafficRefreshRate={}\n'.format(Config['CarMakerSetup']['TrafficRefreshRate']))
            file.write('SynchronizeTrafficSignal={}\n'.format(Config['CarMakerSetup']['SynchronizeTrafficSignal']))
            file.write('TrafficSignalPort={}\n'.format(Config['CarMakerSetup']['TrafficSignalPort']))
            file.write('SignalTableFilename={}\n'.format(SignalTablePathDS))

        if os.path.exists(SignalTableFile):
            shutil.copy(SignalTableFile, os.path.join(RealSimTmpPath, SignalTableFile.split('\\')[-1]))


    return 0

if __name__ == '__main__':
    argparser = argparse.ArgumentParser(description=__doc__)
    argparser.add_argument('--configFile', '-f', 
                        metavar = 'CONFIGFILE', 
                        type=str, 
                        help='configuration yaml file')
    argparser.add_argument('--cm-project-path', '-p', 
                        metavar = 'PATH', 
                        type=str, 
                        default='./',
                        help='CM project path')
    argparser.add_argument('--signal-table-path', 
                        metavar = 'PATH', 
                        type=str, 
                        default='',
                        help='CM+SUMO signal table path')
    arguments = argparser.parse_args()

    setCarMakerConfig(arguments)
