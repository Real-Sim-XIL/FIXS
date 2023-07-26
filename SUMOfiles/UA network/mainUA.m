%% define constants
CommonLibPath = '..\..\CommonLib';
configFilename = '.\test.yaml';

%% add path of RealSim tools
addpath(genpath(CommonLibPath))

%% Genearl Simulation Setups
stopTime = 70; % simulation stop time in seconds. co-simulation will automatically stop after this seconds

%% Initialize RealSim for Simulink, Read yaml file
VehicleMessageFieldDefInputVec = RealSimInitSimulink(configFilename);

%% RealSim Start Procedure
% start simulink model
simModelName = 'UAclient';
load_system(simModelName)
set_param(simModelName,'StopTime',num2str(stopTime));
% set_param(simModelName, 'SimulationCom/mand', 'start');
VehicleOut = sim(simModelName); % alternatively can use 'sim' command

%% check results
temp = load('UA_veh0_2_orig_as0p1');
Orig = temp.Veh;
Mod = VehicleOut;

figure; 
plot(Orig.t, Orig.speed);
hold on
plot(Mod.spdDes.time, Mod.spdDes.signals.values(:));
plot(Mod.spdAct.time, Mod.spdAct.signals.values(:));
% plot(Mod.spdDesInterp.time, Mod.spdDesInterp.signals.values(:));
legend('orig', 'des', 'act')
grid on

%% remove RealSim path
rmpath(genpath(CommonLibPath))



%%

temp = load('UA_veh0_2_orig_as0p1');
Orig = temp.Veh;
% temp = load('UA_veh0_2_orig_as0p1');
% Orig = temp.VehicleOut;

% fileName = 'UA_veh0_2_tf1p0_as0p1';
% fileName = 'UA_veh0_2_tf1p0_hack0p5_as0p1';
% fileName = 'UA_veh0_2_tf1p0_hack0p7_as0p1';
% fileName = 'UA_veh0_2_tf1p0_hack1p0_as0p1';
% fileName = 'UA_veh0_2_tf1p0_hack2p0_as0p1';
% fileName = 'UA_veh0_2_tf1p0_hack3p0_as0p1';

fileNameList = {'UA_veh0_2_tf1p0_as0p1', ...
    'UA_veh0_2_tf1p0_hack0p5_as0p1', ...
    'UA_veh0_2_tf1p0_hack0p7_as0p1', ...
    'UA_veh0_2_tf1p0_hack1p0_as0p1', ...
    'UA_veh0_2_tf1p0_hack2p0_as0p1', ...
    'UA_veh0_2_tf1p0_hack3p0_as0p1'};
% fileNameList = {'UA_veh0_2_tf1p0_hack1p0_as0p1', 'UA_temp'};
for iF = 1:numel(fileNameList)
    
    fileName = fileNameList{iF};
    
    temp = load(fileName);
    
    Mod = temp.VehicleOut;
    
    figure;
    
    plot(Orig.t, Orig.speed);
    % plot(Orig.spdAct.time, Orig.spdAct.signals.values(:));
    hold on
    plot(Mod.spdDes.time, Mod.spdDes.signals.values(:));
    plot(Mod.spdAct.time, Mod.spdAct.signals.values(:));
    yyaxis right
    plot(Mod.accelDesCalc.time, Mod.accelDesCalc.signals.values(:));
    % plot(Mod.spdDesInterp.time, Mod.spdDesInterp.signals.values(:));
    title(fileName, 'interpreter', 'none')
    legend('orig', 'des', 'act', 'accelDes')
    grid on
    
    set(gcf, 'position', [ 1          41        1920        1083])
end
