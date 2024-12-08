function varargout = udpateDriveCycleData()

%% Setup
DdDriveCycle = Simulink.data.dictionary.open('DdDriveCycleSrc.sldd');
DdSection = DdDriveCycle.getSection('Design Data');

%% PTBS drive cycles
PtbsCycleObjs = vsr.drivecycle.getPtbsDriveCycles;
PtbsCycleNames = {'FTP75';...
    'US EPA Urban Dynamometer Driving Schedule (FTP-72)';...
    'US06';...
    'Highway Fuel Economy Driving Schedule (HWFET)'   };

[~, CycleIdx] = intersect({PtbsCycleObjs.Name}, PtbsCycleNames, 'stable');
PtbsCycleObjs = PtbsCycleObjs(CycleIdx);

%% Other drive cycles
VsrCycleObjs = vsr.drivecycle.getVsrDriveCycles;
VsrCycleNames =  {'EPA GEM profile with 55 mph cruise';...
    'EPA GEM profile with 65 mph cruise'};

[~, CycleIdx] = intersect({VsrCycleObjs.Name}, VsrCycleNames, 'stable');
VsrCycleObjs = VsrCycleObjs(CycleIdx);
%% Add drive cycles to data dictionary
AllCycleObjs = [PtbsCycleObjs, VsrCycleObjs];
DdSection.assignin('TestScnrioDriveCycleStructs', AllCycleObjs.getCycleDataStruct)

%% Outputs
if nargout >= 1
    varargout{1} = AllCycleObjs;
end
