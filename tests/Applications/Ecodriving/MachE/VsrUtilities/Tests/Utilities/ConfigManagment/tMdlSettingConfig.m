classdef tMdlSettingConfig < matlab.unittest.TestCase
    
    properties
        SubsystemName = 'mMdlSettingConfig/Drive Cycle Source';
    end
    
    %% TestMethodTeardown
    methods (TestMethodTeardown)
        %% closeMdls
        function closeMdls(obj)
            close_system('mSimpleSubsystemConfig', 0);
            close_system('mVariantSubsystemConfig', 0);
        end
    end
    
    %% Test methods
    methods (Test)
       %% verifyChangeSettings
       function verifyChangeSettings(obj)
           
           
           %% Add drive cycle popup options
           SubsystemConfigObj = vsr.mdl.SubsystemConfig(obj.SubsystemName);
           CycleFTP75 = vsr.mdl.MdlSettingConfig('cycleVar', 'FTP75');
           CycleFTP72 = vsr.mdl.MdlSettingConfig('cycleVar', 'FTP72');
           CycleUS06 = vsr.mdl.MdlSettingConfig('cycleVar', 'US06');
           
           % Add cycle
           SubsystemConfigObj.addMdlSetting(CycleFTP75)
           obj.verifyEqual(CycleFTP75, SubsystemConfigObj.MdlSettings)
           obj.verifyEqual(CycleFTP75.ParentMdlConfig, SubsystemConfigObj)
           
           % Add variants
           SubsystemConfigObj.addMdlSetting(CycleFTP72)
           obj.verifyEqual(CycleFTP75.ParentMdlConfig, SubsystemConfigObj)
           obj.verifyEqual(CycleFTP75, SubsystemConfigObj.MdlSettings)
           
           CycleFTP75.addVariant(CycleUS06)
           obj.verifyEqual(CycleFTP75.ParentMdlConfig, SubsystemConfigObj)
           obj.verifyEqual(CycleFTP75, SubsystemConfigObj.MdlSettings)
           obj.verifyLength(CycleFTP75.OtherVariants, 2)
           
           %% Add checkbox options
           RepeatCycleOn = vsr.mdl.MdlSettingConfig('cycleRepeat', 'on');
           RepeatCycleOff = vsr.mdl.MdlSettingConfig('cycleRepeat', 'off');
           SubsystemConfigObj.addMdlSetting(RepeatCycleOn);
           SubsystemConfigObj.addMdlSetting(RepeatCycleOff);
           
           %% Change settings and verify 
           CycleFTP75.activateVariant
           RepeatCycleOn.activateVariant
           obj.verifyEqual(get_param(obj.SubsystemName, CycleFTP75.Name), CycleFTP75.SettingValue)
           obj.verifyEqual(get_param(obj.SubsystemName, RepeatCycleOn.Name), RepeatCycleOn.SettingValue)
           
           CycleUS06.activateVariant
           RepeatCycleOff.activateVariant
           obj.verifyEqual(get_param(obj.SubsystemName, CycleUS06.Name), CycleUS06.SettingValue)
           obj.verifyEqual(get_param(obj.SubsystemName, RepeatCycleOff.Name), RepeatCycleOff.SettingValue)
           
           %% Close 
           close_system(obj.SubsystemName)
           
       end
    end
    
end