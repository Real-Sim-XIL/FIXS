function startVissim(netName, stopTime)
% to make it more module, can add more input arguments, such as RandSeed,
% time, position, and speed the ego vehicle will enter


%% Connecting the COM Server => Open a new Vissim Window:
Vissim = actxserver('Vissim.Vissim.1100');

Vissim.LoadNet(fullfile(pwd, [netName,'.inpx']));
Vissim.LoadLayout(fullfile(pwd, [netName,'.layx']));

%% Simulation Setup
simul=Vissim.Simulation;
simul.set('AttValue', 'SimPeriod', stopTime);


%% Start Simulation
% continue simulation
Vissim.Simulation.RunContinuous

