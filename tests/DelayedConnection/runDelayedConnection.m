% initialization of vehicle data subscription setup before runnning any
% simulink files. A config.yaml needs to present to read the vehicle data
% subscription setup.


%% define constants
curFilePath = fileparts(mfilename('fullpath'));
RealSimAppPath = fullfile(curFilePath);
RealSimPath = fullfile(curFilePath,'..\..\CommonLib');
configFilename = fullfile(RealSimAppPath,'config_RS_noSimulinkRS.yaml');

%% add path of RealSim tools
addpath(genpath(RealSimPath))

%% initialize RealSim for Simulink, read yaml file
[VehicleMessageFieldDefInputVec, VehDataBus, TrafficLayerIP, TrafficLayerPort] = RealSimInitSimulink(configFilename);
RealSimPara = struct;
RealSimPara.speedInit = 13.5; % initial speed of the ego vehicle when entering SUMO network
RealSimPara.tLookahead = 0.1; % use 0.1 for external control, recommend to use tLookahead >= 0.2 for SUMO driver
RealSimPara.smoothWindow = 1; % number of moving average data point, 1 essentially mean no moving average
