% initialization of vehicle data subscription setup before runnning any
% simulink files. A config.yaml needs to present to read the vehicle data
% subscription setup.

%% Run Batch Scripts
% system("runCoordMergeSUMO.bat")

%% define constants
CommonLibPath = '..\..\CommonLib';
% CommonLibPath = '..\..\realsimreleaseford';
ModelPath = '.\';
% CommonLibPath = '..\..\RealSimRelease';
configFilename = 'config_vissim.yaml';
simModelName = 'RealSimGeneric';


%% add path of RealSim tools
addpath(genpath(CommonLibPath))
addpath(genpath(ModelPath))


%% Genearl Simulation Setups
stopTime = 600; % simulation stop time in seconds. co-simulation will automatically stop after this seconds

%% Initialize RealSim for Simulink, Read yaml file
[VehicleMessageFieldDefInputVec, VehDataBus, TrafficLayerIP, TrafficLayerPort, Status] = RealSimInitSimulink(configFilename);
if Status < 0
    fprintf('\nERROR! RealSim initialize Simulink failed!\n')
    return
end
RealSimPara = struct;
RealSimPara.speedInit = 13.5; % initial speed of the ego vehicle when entering SUMO network
RealSimPara.tLookahead = 0.1; % use 0.1 for external control, recommend to use tLookahead >= 0.2 for SUMO driver
RealSimPara.smoothWindow = 1; % number of moving average data point, 1 essentially mean no moving average

% %% RealSim Start Procedure
% % start simulink model
% load_system(simModelName)
% set_param(simModelName,'StopTime',num2str(stopTime));
% % set_param(simModelName, 'SimulationCommand', 'start');
% VehicleOut = sim(simModelName); % alterModelPathnatively can use 'sim' command
