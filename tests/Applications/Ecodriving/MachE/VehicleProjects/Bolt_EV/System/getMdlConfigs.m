function ConfigObjs = getMdlConfigs

%% Mdl names
TopMdlName = 'EvPassengerModel';

TestScnrioBlk = [TopMdlName, '/Test Scenario'];
TestScnrioVariants = {'Drive Cycle Source'; 'Open Loop 0-60-0'};
for i = 1:length(TestScnrioVariants)
    TestScnrioVariants{i} = [TestScnrioBlk, '/', TestScnrioVariants{i}];
end

DriverBlk = [TopMdlName, '/Driver'];
DriverVariants = {'Predictive Driver', 'Open Loop Driver'};
for i = 1:length(DriverVariants)
    DriverVariants{i} = [DriverBlk, '/', DriverVariants{i}];
end

%% Get possible variants and configuration sets
load_system(TopMdlName);
SystemSettingConfigs = vsr.mdl.getSystemSettingConfigSets(TopMdlName);
VariantSubsystemConfigs = vsr.mdl.getVariantSubsystemConfigs(TopMdlName);

%% Test Scenarios
% Drive cycles
DriveCycleConfig = vsr.mdl.SimConfigSet;
DriveCycleConfig.Name = 'Drive Cycles';
DriveCycleConfig.IndependentConfigs = true;

DriveCycleConfigBlockConfig = vsr.mdl.SimConfigSet;
DriveCycleConfigBlockConfig.Configs = VariantSubsystemConfigs(ismember({VariantSubsystemConfigs.FullMdlName}, {TestScnrioVariants{1}, DriverVariants{1}}));

DdDriveCycleName = 'DdDriveCycleSrc.sldd';
CycleNumParamName = 'TestScnrioCycleNum';
DdDriveCycle = Simulink.data.dictionary.open(DdDriveCycleName);
DdSection = DdDriveCycle.getSection('Design Data');
CycleStructArray = DdSection.evalin('TestScnrioDriveCycleStructs');
CycleNames = {CycleStructArray.Name};
DdConfig = vsr.mdl.DataDictionaryConfig;
DdConfig.DataDictionaryName = DdDriveCycleName;
BaseCycleNumParam = vsr.mdl.Param(DdConfig, CycleNumParamName);

for i = 1:length(CycleNames)
    NewParam = BaseCycleNumParam.createVariant;
    NewParam.Value = i;
    NewConfig = vsr.mdl.SimConfigSet;
    NewConfig.Name = CycleNames{i};
    NewConfig.Configs = [DriveCycleConfigBlockConfig, NewParam];
    
    DriveCycleConfig.addConfig(NewConfig);
    
end

% Acceleration test
AccDecTestConfig = vsr.mdl.SimConfigSet;
AccDecTestConfig.Configs = VariantSubsystemConfigs(ismember({VariantSubsystemConfigs.FullMdlName}, {TestScnrioVariants{2}, DriverVariants{2}}));
AccDecTestConfig.Name = '0-60-0'; 

% All tests
TestScnrioConfig = vsr.mdl.SimConfigSet;
TestScnrioConfig.Name = 'Test Scenarios';
TestScnrioConfig.IndependentConfigs = true;
TestScnrioConfig.Configs = [DriveCycleConfig, AccDecTestConfig];


%% Final output
ConfigObjs = TestScnrioConfig;

