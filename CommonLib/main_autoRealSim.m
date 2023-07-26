% initialization of vehicle data subscription setup before runnning any
% simulink files. A config.yaml needs to present to read the vehicle data
% subscription setup.

close all;clear all;clc;format compact;

%% Initializations
RealSimPath = '.\';
configFilename = '.\config.yaml';
stopTime = 200; % simulation stop time in seconds. co-simulation will automatically stop after this seconds
vissimFilename = '..\\RealSimInterface\\tests\\speedLimit\\speedLimit';
vissimFilename = '..\\RealSimInterface\\VISSIMfiles\\RealSimToyotaTest_2021';

%% add path of RealSim tools
addpath(genpath(RealSimPath))

%% Initialize RealSim for Simulink, Read yaml file
[VehicleMessageFieldDefInputVec, VehDataBus, TrafficLayerIP, TrafficLayerPort] = RealSimInitSimulink_Toyota(configFilename);

%% RealSim Start Procedure
% get an image of current cmd.exe pid
pidListPrior = getCmdPid();

% start RealSim interface
% !!! specify the followings: 
%   1) path of TrafficLayer.exe
%   2) name of .yaml configuration file
system(['cd "', fullfile(pwd, RealSimPath), '" &; ".\TrafficLayer.exe" -f "', configFilename, '" &']);

% get an image of cmd.exe pid after start a new one
pidListPost = getCmdPid();

% get pid of the RealSim interface cmd.exe
pidToKill = setdiff(pidListPost, pidListPrior);

% start Vissim in second Matlab window
% this Matlab window will be closed automatically after simulation ends
% !!! specify the followings: 
%   1) path of matlab.exe, 
%   2) path of VISSIM COM script, 
%   3) VISSIM COM script name and input arguments
system([' "C:\Program Files\MATLAB\R2019a\bin\matlab.exe" -nodesktop -nosplash -r ', ...
    '"', ...
    sprintf('startVissim(''%s'', %f, ''%s''); ', vissimFilename, stopTime+1, configFilename), ...
    'quit; " ']);

% start simulink model
% !!! specify the followings: 
%   1) simulink model name
%   2) stopTime of the simulink model
simModelName = 'RealSimTemplate';
load_system(simModelName)
set_param(simModelName,'StopTime',num2str(stopTime));
set_param(simModelName, 'SimulationCommand', 'start');
% simout = sim(simModelName); % alternatively can use 'sim' command


%% RealSim Shutdown Procedure
% % !!! uncomment to auto-close TrafficLayer.exe after simulation finishes
% % kill RealSim cmd.exe
% if numel(pidToKill) == 1
%     system(['taskkill /F /PID ', num2str(pidToKill)]);
%     fprintf('TrafficLayer.exe has been killed!\n');
% else
%     fprintf('Warning: cannot kill RealSim TrafficLayer.exe, please close it manually\n');
% end

%% define some utility functions
function pidList = getCmdPid()
    [~,pidString]=system('tasklist /FI "IMAGENAME eq cmd.exe" /FO LIST | find "PID:"');
    pidCell=split(strrep(replace(pidString,newline,''),' ',''), 'PID:'); % remove newline symbol and spaces, split into PID list
    pidList=cellfun(@str2num,pidCell(2:end-1)); % remove first empty pid and last one (last pid appears to change every time, not sure meaning of it)
end
