function EngConfigObj = updateEngDdFromAutonomie(AutonomieInitFile, DataDictionaryName)
% updateEngParamsFromAutonomie(AutonomieInitFile, DataDictionaryName)
% Function updates data dictionary engine parameter values from Autonomie
% init file AutonomieInitFile. The data dictionary DataDictionaryName must
% already exist.

EngConfigObj = vsr.mdl.eng.MappedEngConfig;
EngConfigObj.DataDictionaryName = DataDictionaryName;
EngConfigObj.getParamsFromAutonomieInitFile(AutonomieInitFile);
EngConfigObj.pushToDictionary;

