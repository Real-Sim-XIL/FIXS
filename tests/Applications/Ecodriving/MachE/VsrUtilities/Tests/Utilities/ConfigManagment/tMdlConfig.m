classdef tMdlConfig < matlab.unittest.TestCase
    
    properties
        EngineSubsystemName = 'mSimpleSubsystemConfig/Engine Subsystem/Mapped SI Engine';
        VariantSubsystemName = 'mVariantSubsystemConfig/Variant Subsystem Check';
        TestDataDict = 'TestDataDictionary';
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
        
        %% verifyMdlNameChange
        function verifyMdlNameChanges(obj) 
            SubsystemName = obj.EngineSubsystemName;
            MdlObj = vsr.mdl.MdlConfig(SubsystemName);
            NewName = 'Test Change Name';
            MdlObj.Name = NewName;
            obj.verifyEqual(MdlObj.Name, NewName);
            obj.verifyEqual(get_param(MdlObj.FullMdlName, 'Name'), NewName)
            close_system(MdlObj.RootMdlName, 0)
        end
        
        %% verifySubsystemConfigSetup
        function verifySubsystemConfigSetup(obj)
            MdlObj = vsr.mdl.SubsystemConfig(obj.EngineSubsystemName);
            DataSetObj = vsr.mdl.DataDictionaryConfig;
            DataSetObj.DataDictionaryName = obj.TestDataDict;
            MdlObj.DataSet = DataSetObj;
            obj.verifyEqual(DataSetObj.Parent, MdlObj)
        end
        
        %% verifyVariantSubsystemSetup
        function verifyVariantSubsystemSetup(obj)
            %% Set up
            VariantSubsystemParentObj = vsr.mdl.SubsystemConfig(obj.VariantSubsystemName);
            TestMdlRootName = VariantSubsystemParentObj.RootMdlName;
            for i = 1:3
                VariantChoices(i) = vsr.mdl.SubsystemConfig([obj.VariantSubsystemName, '/Subsystem ', num2str(i)]);
                InvalidVariantCoices(i) = vsr.mdl.SubsystemConfig([TestMdlRootName, '/Subsystem ', num2str(i)]);
            end
            
            %% Check errors
            obj.verifyError(@()VariantSubsystemParentObj.addChildSubsystem(InvalidVariantCoices), '')
            
            %% Change variants
            VariantChoices(2).activateVariant
            for i = 1:3
                VariantChoices(i).activateVariant
                obj.verifyEqual(['Choice_', num2str(i)], get_param(obj.VariantSubsystemName, 'LabelModeActiveChoice'))
            end
            VariantChoices(3).activateVariant
            VariantChoices(1).activateVariant
        end
    end
    
end