classdef SimTask < vsr.taskrunner.Task
    % SimTask class definition handles running simulation
    
    %% Public properties
    properties
        MdlName char ; % Can be derived from ConfigSets but set value for now
%         ConfigSet vsr.mdl.SimConfigSet % Apply config set for each simulation
    end
    
    %% Protected properties
    properties (SetAccess = protected)
        ConfigSetFcnHdl function_handle
        SimOut Simulink.SimulationOutput
    end
    
    
    %% Public methods
    methods
        %% runTask
        function obj = runTask(obj)
            load_system(obj.MdlName);
            obj.ConfigSetFcnHdl();
            obj.SimOut = sim(obj.MdlName, 'ReturnWorkspaceOutputs', 'on');
        end
        
        %% setConfigSet
        function obj = setConfigSet(obj, ConfigSet)
            obj.ConfigSetFcnHdl = ConfigSet.getActivateFcnHdl;
        end
    end
    
end
    