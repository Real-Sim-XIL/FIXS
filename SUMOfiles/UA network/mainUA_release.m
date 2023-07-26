%% define constants
RealSimLibPath = '..\..\RealSimRelease';
configFilename = '.\test.yaml';

%% add path of RealSim tools
addpath(genpath(RealSimLibPath))

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
temp = load('UA_veh0_2_orig');
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
rmpath(genpath(RealSimLibPath))