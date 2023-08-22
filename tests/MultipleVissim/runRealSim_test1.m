% initialization of vehicle data subscription setup before runnning any
% simulink files. A config.yaml needs to present to read the vehicle data
% subscription setup.

%     add and select egoCAV type 1000

close all;clear all;clc;format compact;

%% Initializations
RealSimPath = '..\..\CommonLib';
configFilename = '.\config_test1.yaml';
stopTime = 90; % simulation stop time in seconds. co-simulation will automatically stop after this seconds
vissimFilename = '.\\TrafficModel\\Headquarters 11';
simModelName = 'multipleVissimClient';

%% add path of RealSim tools
addpath(genpath(RealSimPath))

%% Run Batch Scripts
system(['start cmd /c ..\..\TrafficLayer\x64\Release\TrafficLayer.exe -f ', sprintf('%s', configFilename)])

%% Initialize RealSim for Simulink, Read yaml file
[VehicleMessageFieldDefInputVec, VehDataBus, TrafficLayerIP, TrafficLayerPort] = RealSimInitSimulink(configFilename);
RealSimPara = struct;
RealSimPara.speedInit = 13; % initial speed of the ego vehicle when entering SUMO network
RealSimPara.tLookahead = 0.1; % use 0.1 for exteranl control, recommend to use tLookahead >= 0.2 for SUMO driver
RealSimPara.smoothWindow = 1; % number of moving average data point, 1 essentially mean no moving average

RealSimPara.speedSource = 2; % select sine wave for tracking

%% RealSim Start Procedure
% start Vissim in second Matlab window
% this Matlab window will be closed automatically after simulation ends
% !!! specify the followings: 
%   1) path of matlab.exe, 
%   2) path of VISSIM COM script, 
%   3) VISSIM COM script name and input arguments
system([' matlab -nodesktop -nosplash -r ', ...
    '"', ...
    sprintf('startVissim(''%s'', %f, ''%s''); ', vissimFilename, stopTime+1, configFilename), ...
    'quit force; " & ']);

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
    'signalLightId','signalLightHeadId','precedingVehicleDistance','precedingVehicleSpeed'};
temp = load('multipleVissimTest1_orig','VehicleOut');
Orig = temp.VehicleOut.VehDataBus;
Mod = VehicleOut.VehDataBus;

figure;
SUB_ROW = 2; SUB_COL = 4; subNum = 1; ax = [];
set(gcf, 'position', ...
    [1          41        1920        1083])
AxisPos = tightPlotAxes(SUB_ROW, SUB_COL, [0, 0, 0, 0]);
c = genColorCodes(); 

for dataname = DataNamesToCheck
    dataname = dataname{1};
    ax(subNum) = axes('OuterPosition', AxisPos(subNum, :));
    subNum = subNum + 1; hold on
    plot(Orig.(dataname).Time, Orig.(dataname).Data);
    if strcmpi(dataname, 'signalLightId')
        strLen = Mod.('signalLightIdLength').Data(end);
        ModData = arrayfun(@str2double,char(Mod.(dataname).Data(:,1:strLen)));
        ModData(isnan(ModData)) = 0;
    else
        ModData = Mod.(dataname).Data;
    end
    plot(Mod.(dataname).Time, ModData);
    grid on; title(dataname)
    
    if numel(Orig.(dataname).Time) ~= numel(Mod.(dataname).Time) || numel(Orig.(dataname).Data) ~= numel(ModData)
        message = sprintf('\nSPEEDLIMIT TEST 1 VISSIM : ego %s trace vector size mismatch \n', dataname);
        writeLog(filename, message);
        checkFailed = 1;
    elseif max(abs(Orig.(dataname).Time-Mod.(dataname).Time))>1e-5 || max(abs(Orig.(dataname).Data-ModData))>1e-5
        message = sprintf('\nSPEEDLIMIT TEST 1 VISSIM : ego %s trace value mismatch \n', dataname);
        writeLog(filename, message);
        checkFailed = 1;
    end
end

%% remove RealSim path
rmpath(genpath(RealSimPath))

%% quit matlab
if ~checkFailed
    message = '===> PASSED MULTIPLEVISSIM TEST 1: VISSIM test ===> \n';
    writeLog(filename, message)
    
end


%% helper function
function writeLog(filename, message)
    fid = fopen(filename, 'at+');
    fprintf(fid, message);
    fclose(fid);
    fclose('all');
end