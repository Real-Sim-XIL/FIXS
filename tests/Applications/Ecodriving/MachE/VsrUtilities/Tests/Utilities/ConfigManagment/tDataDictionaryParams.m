classdef tDataDictionaryParams < matlab.unittest.TestCase
    % Test mdl.Component object interacting with Simulink data dictionary
    
    properties
        TestDataDict = 'TestDataDictionary';
    end
    %% TestMethodTeardown
    methods (TestMethodTeardown)
        %% discardDataDictionaryChanges
        function discardDataDictionaryChanges(obj)
            CompObj = vsr.mdl.DataDictionaryConfig;
            CompObj.DataDictionaryName = obj.TestDataDict;
            CompObj.DataDictionary.discardChanges;
            CompObj.DataDictionary.close;
        end
    end
    
    %% Test methods
    methods (Test)
        %% verifySettingDataDictionaryName
        function verifySettingDataDictionaryName(obj)
           %% Setup 
            CompObj = vsr.mdl.DataDictionaryConfig;
            
            %% Test setting data dictionary name
            obj.verifyEmpty(CompObj.DataDictionary); % verify empty if data dictionary name not set
            
            CompObj.DataDictionaryName = obj.TestDataDict;
            obj.verifyEqual(CompObj.DataDictionaryName, [obj.TestDataDict, '.sldd']);
            
            CompObj.DataDictionaryName = [obj.TestDataDict, '.sldd'];
            obj.verifyEqual(CompObj.DataDictionaryName, [obj.TestDataDict, '.sldd']); 
        end
        
        %% testFindParam
        function testFindParam(obj)
            %% Set up
            CompObj = vsr.mdl.DataDictionaryConfig;
            CompObj.DataDictionaryName = obj.TestDataDict;
            
            %% Verify empty if not found
            obj.verifyEmpty(CompObj.findParam('NotValidParam'))
            
            %% Find valid param
            Param = CompObj.findParam('Param');
            ParamEntry = Param.DataDictionaryEntry;
            ParamEntry.setValue(1);
            Param = CompObj.findParam('Param');
            obj.verifyEqual(Param.Value, 1)
            
            
            %% Close out data dictionary without saving changes
            CompObj.DataDictionary.discardChanges;
            CompObj.DataDictionary.close;
        end
        
        %% testDataDictionaryParamChange
        function testDataDictionaryParamChange(obj)
            
            %% Setup 
            CompObj = vsr.mdl.DataDictionaryConfig;
            CompObj.DataDictionaryName = obj.TestDataDict;
            
            %% Test getting parameters
            obj.verifyEqual(length(CompObj.Params), 6);
            obj.verifyEmpty(CompObj.findParam('NotValidParamName'))
            VarParam = CompObj.findParam('Var');
            
            
            %% Test changing values
            
            % Test getting value
            obj.verifyEqual(VarParam.Value, 0);
            
            % Test changing value
            VarParam.Value = 100;
            obj.verifyEqual(VarParam.Value, 100);
            ParamEntry = CompObj.DataDesignSection.getEntry('Var');
            obj.verifyEqual(ParamEntry.getValue, 100);
            
            %% Test changing parameter names
            VarParam1 = CompObj.findParam('Var1');
            % Change to a name that is exist should produce an error
            obj.verifyError(@() set(VarParam1, 'Name', 'Var'), 'SLDD:sldd:EntryAlreadyExists')
            obj.verifyEqual(VarParam1.Name, 'Var1')    
            
            % Change to valid name
            VarParam1.Name = 'ValidParamName';
            obj.verifyEqual(VarParam1.Name, 'ValidParamName');
            ParamEntry = CompObj.DataDesignSection.getEntry('ValidParamName');
            obj.verifyEqual(ParamEntry.Name, 'ValidParamName');
            
            %% Test adding data
            CompObj.DataDesignSection.assignin('NewVar', 10)
            NewParam = CompObj.findParam('NewVar');
            obj.verifyEqual(NewParam.Value, 10)
            
            %% Test removing parmeters
            CompObj.DataDesignSection.assignin('RemoveParam', 10)
            RemoveParam = CompObj.findParam('RemoveParam'); 
            CompObj.DataDesignSection.deleteEntry('RemoveParam');
            obj.verifyEqual(RemoveParam.Value, 10);
            obj.verifyEmpty(RemoveParam.Parent);
            
            RemoveParam.Value = 11;
            obj.verifyEqual(RemoveParam.Value, 11);
            
            % Repeat 
            CompObj.DataDesignSection.assignin('RemoveParam', 12)
            RemoveParam = CompObj.findParam('RemoveParam'); 
            CompObj.DataDesignSection.deleteEntry('RemoveParam');
            obj.verifyEqual(RemoveParam.Value, 12);
            obj.verifyEmpty(RemoveParam.Parent);
            
            RemoveParam.Value = 13;
            obj.verifyEqual(RemoveParam.Value, 13);
            
            %% Close out data dictionary without saving changes
            CompObj.DataDictionary.discardChanges;
            CompObj.DataDictionary.close;
            
        end
        
        % tAddRemoveDataDictionaryParam
        %% testParamVariants
        function testParamVariants(obj)
            
            %% Setup
            CompObj = vsr.mdl.DataDictionaryConfig;
            CompObj.DataDictionaryName = obj.TestDataDict;
            VarName = 'Var';
            VarStartValue = 0;
            VarEntry = CompObj.DataDesignSection.getEntry(VarName);
            %% Check outputs with one input
            MainParam = CompObj.findParam(VarName);
            obj.verifyEmpty(MainParam.OtherVariants);
            obj.verifyTrue(MainParam.isActiveVariant);
            
            %% Add variant parameter
            VariantParam = MainParam.createVariant;
            obj.verifyTrue(MainParam.isActiveVariant);
            obj.verifyFalse(VariantParam.isActiveVariant);
            obj.verifyEqual(MainParam, VariantParam.OtherVariants)
            obj.verifyEqual(VariantParam, MainParam.OtherVariants)
            
            % Inactive
            VariantParam.Value = 100;
            obj.verifyEqual(VariantParam.Value, 100)
            obj.verifyEqual(MainParam.Value, VarStartValue)
            obj.verifyEqual(VarEntry.getValue, VarStartValue);
            
            % Activate variant
            VariantParam.activateVariant
            obj.verifyFalse(MainParam.isActiveVariant);
            obj.verifyTrue(VariantParam.isActiveVariant);
            obj.verifyEqual(VarEntry.getValue, 100);
            obj.verifyEqual(VariantParam.Value, 100)
            obj.verifyEqual(MainParam.Value, VarStartValue)
            obj.verifyEqual(VariantParam, MainParam.getActiveVariant)
            obj.verifyEqual(MainParam, VariantParam.OtherVariants)
            obj.verifyEqual(VariantParam, MainParam.OtherVariants)
            
            %% Change parameter name (impacts all variants)
            MainParam.activateVariant;
            VariantParam2 = VariantParam.createVariant;
            VariantParam2.Value = 200;
            obj.verifyError(@() set(VariantParam2, 'Name', 'Var1'), 'SLDD:sldd:EntryAlreadyExists')
            obj.verifyEqual(VariantParam2.Name, 'Var')
            VariantParam2.Name = 'NewParamName';
            obj.verifyEqual(VariantParam2.Name, VariantParam.Name)
            obj.verifyEqual(VariantParam.Name, MainParam.Name)

            obj.verifyEqual([VariantParam2.OtherVariants.Value], [0, 100])
            obj.verifyEqual([MainParam.OtherVariants.Value], [100, 200])

            %% Remove variant
            VariantParam.removeVariant;
            obj.verifyEmpty(VariantParam.Parent)
            obj.verifyEqual(VariantParam2.OtherVariants, MainParam)
            obj.verifyEqual(MainParam.OtherVariants, VariantParam2)
            VariantParam2.activateVariant;
            obj.verifyTrue(VariantParam2.isActiveVariant);
            obj.verifyEqual(VarEntry.getValue, 200);
            VariantParam2.removeVariant;
            obj.verifyEqual(VarEntry.getValue, 0);       
            
            %% Close out data dictionary without saving changes
            CompObj.DataDictionary.discardChanges;
            CompObj.DataDictionary.close;
            
        end
    end
end

