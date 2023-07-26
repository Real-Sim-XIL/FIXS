function startVissim(netName, stopTime, configFilename)
% to make it more module, can add more input arguments, such as RandSeed,
% time, position, and speed the ego vehicle will enter


%% Connecting the COM Server => Open a new Vissim Window:
% vis = actxserver('Vissim.Vissim');
Vissim=actxserver('VISSIM.Vissim.2100');
% access_path=pwd;
% vis.LoadNet([access_path '\TrafficModel\Headquarters 11.inpx']);
% vis.LoadLayout([access_path '\TrafficModel\Headquarters 10.layx']);

% netName='..\VISSIMfiles\simple_2021';
Vissim.LoadNet(fullfile(pwd, [netName,'.inpx']));
Vissim.LoadLayout(fullfile(pwd, [netName,'.layx']));

%% Setup ExtDriverParFile for all vehicle types
Vissim.Net.VehicleType.SetAllAttValues('ExtDriverParFile', fullfile(pwd,configFilename))
    
%% Simulation Setup
simul=Vissim.Simulation;
period_time= stopTime;   step_time=10; step_size=1/step_time;
RandSeed = 42; % random seed, can be any integer value
RandSeedIncr = 1; % incrementation of random seed after each run
NumRuns = 1; % number of simulation runs

simul.set('AttValue', 'SimPeriod', period_time);
simul.set('AttValue', 'SimRes', step_time);
set(simul, 'AttValue', 'RandSeed', RandSeed);
set(simul, 'AttValue', 'RandSeedIncr', RandSeedIncr);
set(simul, 'AttValue', 'NumRuns', NumRuns);
set(simul, 'AttValue', 'UseMaxSimSpeed', true);

present=Vissim.Presentation;
present.set('AttValue','RecordAVIs',1);

%% Start Simulation

if strcmpi(Vissim.Net.NetPara.AttValue('UnitSpeed'), 'MILESPERHOUR')
    UNIT_CONVERT_CONSTANT = 2.23694; % meters-per-second to miles-per-hour
elseif strcmpi(Vissim.Net.NetPara.AttValue('UnitSpeed'), 'KILOMETERSPERHOUR')
    UNIT_CONVERT_CONSTANT = 3.6; % meters-per-second to kilometers-per-hour
else
    UNIT_CONVERT_CONSTANT = 3.6; % use kilometers-per-hour convert
end

% break at XXX seconds to add ego
set(Vissim.Simulation, 'AttValue', 'SimBreakAt', 10);
Vissim.Simulation.RunContinuous

% add ego vehicle
vehicle_type = 1000; % use type specified in the config.yaml
desired_speed = 13*UNIT_CONVERT_CONSTANT; % unit according to the user setting in Vissim [km/h or mph]
link = 1;
lane = 3;
xcoordinate = 1; % unit according to the user setting in Vissim [m or ft]
interaction = true; % please select 'true'
egoVehicle = Vissim.Net.Vehicles.AddVehicleAtLinkPosition(vehicle_type, link, lane, xcoordinate, desired_speed, interaction);

% continue simulation
Vissim.Simulation.RunContinuous

