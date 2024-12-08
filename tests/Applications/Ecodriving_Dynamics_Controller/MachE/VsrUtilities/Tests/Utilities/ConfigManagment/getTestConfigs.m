function TestConfigObjs = getTestConfigs()

%% Setup
MdlName = 'mConfigBlk';

%% Config set
SystemConfigObj = vsr.mdl.SystemConfig(MdlName);
ConfigSetFixedObj = vsr.mdl.SystemSettingsConfig(SystemConfigObj);
ConfigSetFixedObj.Name = 'ConfigSetDesktopFixedStep';
ConfigSetVariableObj = ConfigSetFixedObj.createVariant;
ConfigSetVariableObj.Name = 'ConfigSetDesktopVariableStep';

%% Parameters

CompObj = vsr.mdl.DataDictionaryConfig;
CompObj.DataDictionaryName = 'TestDataDictionary';
VarParam1 = CompObj.findParam('Var');
VarParam2 = VarParam1.createVariant;

%% Block settings
SubsystemConfigObj = vsr.mdl.SubsystemConfig('mConfigBlk/Drive Cycle Source');
CycleFTP75 = vsr.mdl.MdlSettingConfig('cycleVar', 'FTP75');
CycleFTP72 = vsr.mdl.MdlSettingConfig('cycleVar', 'FTP72');
CycleUS06 = vsr.mdl.MdlSettingConfig('cycleVar', 'US06');

% Add cycle
SubsystemConfigObj.addMdlSetting(CycleFTP75)

% Add variants
SubsystemConfigObj.addMdlSetting(CycleFTP72)
CycleFTP75.addVariant(CycleUS06)

%% Variants
VariantSubsystemName = 'mConfigBlk/Variant Subsystem Check';
VariantSubsystemParentObj = vsr.mdl.SubsystemConfig(VariantSubsystemName);
TestMdlRootName = SystemConfigObj.RootMdlName;
for i = 1:3
    VariantChoices(i) = vsr.mdl.SubsystemConfig([VariantSubsystemName, '/Subsystem ', num2str(i)]);
end

%% Create config sets
ConfigSets(1) = vsr.mdl.SimConfigSet;
ConfigSets(1).Name = 'ConfigSetChild1';
ConfigSets(1).addConfig(VarParam1);
ConfigSets(1).addConfig(CycleFTP75);
ConfigSets(1).addConfig(ConfigSetFixedObj);
ConfigSets(1).addConfig(VariantChoices(1));

ConfigSets(2) = vsr.mdl.SimConfigSet;
ConfigSets(2).Name = 'ConfigSetChild2';
ConfigSets(2).addConfig(VarParam1);
ConfigSets(2).addConfig(CycleUS06);
ConfigSets(2).addConfig(ConfigSetFixedObj);
ConfigSets(2).addConfig(VariantChoices(2));

%% Create configuration sets
TestConfigObjs(1) = vsr.mdl.SimConfigSet;
TestConfigObjs(1).Name = 'ConfigSet1';
TestConfigObjs(1).addConfig(VarParam1);
TestConfigObjs(1).addConfig(CycleFTP75);
TestConfigObjs(1).addConfig(ConfigSetFixedObj);
TestConfigObjs(1).addConfig(VariantChoices(1));


TestConfigObjs(2) = vsr.mdl.SimConfigSet;
TestConfigObjs(2).Name = 'ConfigSet2';
TestConfigObjs(2).addConfig(VarParam1);
TestConfigObjs(2).addConfig(CycleUS06);
TestConfigObjs(2).addConfig(ConfigSetFixedObj);
TestConfigObjs(2).addConfig(VariantChoices(2));


TestConfigObjs(3) = vsr.mdl.SimConfigSet;
TestConfigObjs(3).Name = 'ConfigGroup1';
TestConfigObjs(3).Configs = ConfigSets;
TestConfigObjs(3).IndependentConfigs = true;





end