classdef tSimTask < matlab.unittest.TestCase
    % Test mdl.Component object interacting with Simulink data dictionary
    
    properties
        TestDataDict = 'DdTestSimRunner';
        MdlName =     'mSimpleConfigTest'
        VariantSubsystemName = 'mSimpleConfigTest/Variant Subsystem Check';
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
        %% testSimTaskNotParallel
        function testSimTaskNotParallel(obj)
            %% Setup 
            open_system(obj.MdlName)
            DdConfig = vsr.mdl.DataDictionaryConfig;
            DdConfig.DataDictionaryName = obj.TestDataDict;
            TaskRunnerObj = vsr.taskrunner.TaskRunner;
            
            % Parameter
            ParamValues = [1,10,100,1000];
            VarParam = DdConfig.findParam('Var1');
            VarParam.Value = ParamValues(1);
            for i = 2:length(ParamValues)
                VarParam(i) = VarParam(1).createVariant;
                VarParam(i).Value = ParamValues(i);
            end
            
            % Variants
            VariantSubsystemParentObj = vsr.mdl.SubsystemConfig(obj.VariantSubsystemName);
            for i = 1:3
                VariantChoices(i) = vsr.mdl.SubsystemConfig([obj.VariantSubsystemName, '/Subsystem ', num2str(i)]);
            end
            
            %% Create tasks
            
            % Create config sets
%             AllConfigSets(numel(ParamValues)*numel(VariantChoices)) = vsr.mdl.SimConfigSet;
            idx = 1;
            for i = 1:length(ParamValues)
                for j = 1:length(VariantChoices)
                    ConfigSet(idx) = vsr.mdl.SimConfigSet;
                    ConfigSet(idx).addConfig(VarParam(i));
                    ConfigSet(idx).addConfig(VariantChoices(j));
                    idx = idx+1;
                end
            end
            SimIn = Simulink.SimulationInput;
            SimIn.ModelName = obj.MdlName;
            SimIn = ConfigSet.setSimulationInput(SimIn);
            %% Run simulations
%             TaskRunnerObj = vsr.taskrunner.TaskRunner;
%             TaskRunnerObj.Tasks = SimTasks;
%             TaskRunnerObj.UseParallel = true; %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% Change to test parallel
%             TaskRunnerObj = TaskRunnerObj.run;
%             SimTasks = TaskRunnerObj.Tasks;
            SimOut = sim(SimIn);
            
            %% Verify simulation results
            for i = 1:length(SimOut)
                OutputData = SimOut(i).logsout.find('OutputData');
                FinalValue(i) = OutputData.Values.Data(end);
            end
            
            idx = 1;
            for i = 1:length(ParamValues)
                for j = 1:length(VariantChoices)
                    obj.verifyEqual(FinalValue(idx),ParamValues(i)*j)
                    idx = idx+1;
                end
            end
            
            %% Close system
            close_system(obj.MdlName,0);
        end
        
        
    end
end

