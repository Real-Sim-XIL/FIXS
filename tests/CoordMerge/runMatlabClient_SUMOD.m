% initialization of vehicle data subscription setup before runnning any
% simulink files. A config.yaml needs to present to read the vehicle data
% subscription setup.

clear all; 

%% Run Batch Scripts
system("runCoordMergeSUMOD.bat")

%% define constants
CommonLibPath = '..\..\CommonLib';
configFilename = '.\config_SUMOD.yaml';
simModelName = 'coordMergeClient';
stopTime = 200; % simulation stop time in seconds. co-simulation will automatically stop after this seconds

%% add path of RealSim tools
addpath(genpath(CommonLibPath))

%% Initialize RealSim for Simulink, Read yaml file
[VehicleMessageFieldDefInputVec, VehDataBus, TrafficLayerIP, TrafficLayerPort] = RealSimInitSimulink(configFilename);
RealSimPara = struct;
RealSimPara.speedInit = 13.5; % initial speed of the ego vehicle when entering SUMO network
RealSimPara.tLookahead = 0.2; % use 0.1 for exteranl control, recommend to use tLookahead >= 0.2 for SUMO driver
RealSimPara.smoothWindow = 1; % number of moving average data point, 1 essentially mean no moving average

%% RealSim Start Procedure
% start simulink model
load_system(simModelName)
set_param(simModelName,'StopTime',num2str(stopTime));
% set_param(simModelName, 'SimulationCommand', 'start');
VehicleOut = sim(simModelName); % alternatively can use 'sim' command

%% check results
checkFailed = 0;
filename = '..\testsResults.log';

temp = load('coordMergeSUMO_orig','VehicleOut');
Orig = temp.VehicleOut;
Mod = VehicleOut;

if numel(Orig.spdDes.time) ~= numel(Mod.spdDes.time) || numel(Orig.spdDes.signals.values(:)) ~= numel(Mod.spdDes.signals.values(:))
    message = '\nCOORDMERGE SUMO : ego speed trace vector size mismatch \n';
    writeLog(filename, message);
    checkFailed = 1;
    
    rmpath(genpath(CommonLibPath))
    return
elseif max(abs(Orig.spdDes.time-Mod.spdDes.time))>1e-5 || max(abs(Orig.spdDes.signals.values(:)-Mod.spdDes.signals.values(:)))>1e-5
    message = '\nCOORDMERGE SUMO : ego speed mismatch \n';
    writeLog(filename, message);
    checkFailed = 1;
    
    rmpath(genpath(CommonLibPath))
    return
end

figure; 
plot(Orig.spdDes.time, Orig.spdDes.signals.values(:));
hold on
plot(Mod.spdDes.time, Mod.spdDes.signals.values(:));

%% remove RealSim path
rmpath(genpath(CommonLibPath))

%% quit matlab
if ~checkFailed
    message = '===> PASSED COORDMERGE: SUMO test ===> \n';
    writeLog(filename, message)
    
end


%% helper function
function writeLog(filename, message)
    fid = fopen(filename, 'at+');
    fprintf(fid, message);
    fclose(fid);
    fclose('all');
end