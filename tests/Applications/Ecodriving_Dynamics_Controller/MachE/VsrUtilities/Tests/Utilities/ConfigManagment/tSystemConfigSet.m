classdef tSystemConfigSet < matlab.unittest.TestCase
    
    %% Public properties
    properties
        TestMdlName = 'mConfigurationSetConfig';
    end
    
    %% Test parameters
    properties (TestParameter)
        ConfigSetName = {'ConfigSetDesktopFixedStep', 'ConfigSetDesktopVariableStep'};
    end
%     
    %% TestMethodTeardown
    methods (TestMethodTeardown)
        %% closeMdls
        function closeMdls(obj)
            close_system(obj.TestMdlName, 0);
        end
    end
    
    %% Test methods
    methods (Test)
       %% verifyChangeSettings
       function verifyChangeConfigSet(obj, ConfigSetName)
           SystemConfigObj = vsr.mdl.SystemConfig(obj.TestMdlName);
           
           ConfigSetObj = vsr.mdl.SystemSettingsConfig(SystemConfigObj);
           ConfigSetObj.Name = ConfigSetName;
           
           ConfigSetObj.activateVariant;
           
           ActiveConfigSet = getActiveConfigSet(obj.TestMdlName);
           obj.verifyEqual(ActiveConfigSet.Name, ConfigSetName);
           
       end
    end
    
end