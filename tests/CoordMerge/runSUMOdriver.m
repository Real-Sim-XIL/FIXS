% initialization of vehicle data subscription setup before runnning any
% simulink files. A config.yaml needs to present to read the vehicle data
% subscription setup.

%% Run Batch Scripts
system("runSUMOdriver.bat")

%% define constants
% CommonLibPath = '..\..\CommonLib';
CommonLibPath = '..\..\realsimreleaseford';
ModelPath = '..\..\Ford_MachE_EVV';
% CommonLibPath = '..\..\RealSimRelease';
configFilename = 'config_SUMOdriver.yaml';


%% add path of RealSim tools
addpath(genpath(CommonLibPath))
addpath(genpath(ModelPath))


%% Genearl Simulation Setups
stopTime = 600; % simulation stop time in seconds. co-simulation will automatically stop after this seconds

%% Initialize RealSim for Simulink, Read yaml file
[VehicleMessageFieldDefInputVec, VehDataBus, TrafficLayerIP, TrafficLayerPort] = RealSimInitSimulink(configFilename);
RealSimPara = struct;
RealSimPara.speedInit = 0; % initial speed of the ego vehicle when entering SUMO network
RealSimPara.tLookahead = 0.2; % use 0.1 for exteranl control, recommend to use tLookahead >= 0.2 for SUMO driver
RealSimPara.smoothWindow = 1; % number of moving average data point, 1 essentially mean no moving average

%% RealSim Start Procedure
% start simulink model
%simModelName = 'coordMergeClient';
cd(ModelPath)
simModelName = 'FordMustangMachE_AWD_ExtRange_EVV_ORNL_Edit';
run("RUN_ME.m") 
load_system(simModelName)
set_param(simModelName,'StopTime',num2str(stopTime));
% set_param(simModelName, 'SimulationCommand', 'start');
VehicleOut = sim(simModelName); % alterModelPathnatively can use 'sim' command