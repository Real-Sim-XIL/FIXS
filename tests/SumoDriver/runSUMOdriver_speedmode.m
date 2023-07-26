% initialization of vehicle data subscription setup before runnning any
% simulink files. A config.yaml needs to present to read the vehicle data
% subscription setup.

close all;clear all;clc;format compact;

%% Initializations
RealSimPath = '..\..\CommonLib';
configFilename = '.\config_SUMOdriver_speedmode.yaml';
stopTime = 250; % simulation stop time in seconds. co-simulation will automatically stop after this seconds
simModelName = 'sumoDriverClient';

%% add path of RealSim tools
addpath(genpath(RealSimPath))

%% Run Batch Scripts
system("runSUMOdriver.bat")
system(['start cmd /c ..\..\TrafficLayer\x64\Release\TrafficLayer.exe -f ', sprintf('%s', configFilename)])

%% Initialize RealSim for Simulink, Read yaml file
[VehicleMessageFieldDefInputVec, VehDataBus, TrafficLayerIP, TrafficLayerPort] = RealSimInitSimulink(configFilename);
RealSimPara = struct;
RealSimPara.speedInit = 0; % initial speed of the ego vehicle when entering SUMO network
RealSimPara.tLookahead = 0.1; % use 0.1 for exteranl control, recommend to use tLookahead >= 0.2 for SUMO driver
RealSimPara.smoothWindow = 1; % number of moving average data point, 1 essentially mean no moving average

RealSimPara.speedSource = 2; % select sine wave for tracking

%% RealSim Start Procedure
tic

% start simulink model
% !!! specify the followings: 
%   1) simulink model name
%   2) stopTime of the simulink model
load_system(simModelName)
set_param(simModelName,'StopTime',num2str(stopTime));
% set_param(simModelName, 'SimulationCommand', 'start');
VehicleOut = sim(simModelName); % alternatively can use 'sim' command

sim_time = toc

%% check results
checkFailed = 0;
filename = '..\testsResults.log';

DataNamesToCheck = {'speed','speedLimit','speedLimitNext','speedLimitChangeDistance',...
    'signalLightHeadId','signalLightColor','precedingVehicleDistance','precedingVehicleSpeed'};
temp = load('sumoDriver_orig','VehicleOut');
Orig = temp.VehicleOut.VehDataBus;
Mod = VehicleOut.VehDataBus;

temp = load('sumoDriver_noRealSim.mat','Veh');
Plain = temp.Veh;

figure;
SUB_ROW = 2; SUB_COL = 4; subNum = 1; ax = [];
set(gcf, 'position', ...
    [1          41        1920        1083])
AxisPos = tightPlotAxes(SUB_ROW, SUB_COL, [0, 0, 0, 0]);
c = genColorCodes(); 

for dataname = DataNamesToCheck
    dataname = dataname{1};
    if numel(Orig.(dataname).Time) ~= numel(Mod.(dataname).Time) || numel(Orig.(dataname).Data) ~= numel(Mod.(dataname).Data)
        message = sprintf('\nSUMO driver test DearbornOSM : ego %s trace vector size mismatch \n', dataname);
        writeLog(filename, message);
        checkFailed = 1;
    elseif max(abs(Orig.(dataname).Time-Mod.(dataname).Time))>1e-5 || max(abs(Orig.(dataname).Data-Mod.(dataname).Data))>1e-5
        message = sprintf('\nSUMO driver test DearbornOSM : ego %s trace value mismatch \n', dataname);
        writeLog(filename, message);
        checkFailed = 1;
    end
    ax(subNum) = axes('OuterPosition', AxisPos(subNum, :));
    subNum = subNum + 1; hold on
    plot(Orig.(dataname).Time, Orig.(dataname).Data);
    plot(Mod.(dataname).Time, Mod.(dataname).Data);
    if strcmpi(dataname,'speed')
        plot(Plain.t, Plain.spd, '--')
    end
    grid on; title(dataname)
end

%% remove RealSim path
rmpath(genpath(RealSimPath))

%% quit matlab
if ~checkFailed
    message = '===> PASSED SUMO driver test DearbornOSM ===> \n';
    writeLog(filename, message)
    
end


%% helper function
function writeLog(filename, message)
    fid = fopen(filename, 'at+');
    fprintf(fid, message);
    fclose(fid);
    fclose('all');
end