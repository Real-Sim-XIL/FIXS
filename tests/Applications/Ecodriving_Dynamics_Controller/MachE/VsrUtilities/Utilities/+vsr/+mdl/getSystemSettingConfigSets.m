function SystemSettingConfigs = getSystemSettingConfigSets(MdlName)
% VariantConfigs = getSystemSettingConfigSets(MdlName) returns
% vsr.mdl.SystemSettingsConfig objects for every configuration set in MdlName. Function
% currently does not support finding variants model references.


%% Get config set names
MdlName = bdroot(MdlName);
ConfigSetNames = getConfigSets(MdlName);
SystemSettingConfigs = vsr.mdl.SystemSettingsConfig.empty;
SystemConfigObj = vsr.mdl.SystemConfig(MdlName);
for i = 1:length(ConfigSetNames)
    SystemSettingConfigs(i) = vsr.mdl.SystemSettingsConfig(SystemConfigObj);
    SystemSettingConfigs(i).Name = ConfigSetNames{i};
end
for i = 2:length(ConfigSetNames)
    SystemSettingConfigs(1).addVariant(SystemSettingConfigs(i));
end

%% Model references
% RefMdls = find_mdlrefs(MdlName);



