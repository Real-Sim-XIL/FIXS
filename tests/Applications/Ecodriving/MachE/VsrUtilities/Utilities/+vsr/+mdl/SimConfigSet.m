classdef SimConfigSet < vsr.mdl.SimConfig
    % SimConfigSet repesents a group of config settings
    % Use SimConfigSet for configuring a model with several settings
    % changes. Example: group engine plant and controller in the same set.
    
    %% Public properties
    properties
        Configs vsr.mdl.SimConfig
        IndependentConfigs = false; % Set to true to make the configs independent for displaying in the model config block dialog
    end
    
    %% Public methods
    methods
        %% addConfig
        function addConfig(obj, NewConfig)
            obj.Configs = [obj.Configs, NewConfig];
        end
        
        %% isActive
        function flag = isActive(obj)
            flag = true;
            for i = 1:length(obj.Configs)
                if ~obj.Configs(i).isActive
                    flag = false;
                    break;
                end
            end
        end
        
        %% setSimulationInput
        function SimIn = setSimulationInput(obj, SimIn)
            % Add simulation input to Simulink.SimulationInput object
            if length(SimIn) == 1 && numel(obj) > 1
                SimInSingle = SimIn;
                for i = 1:numel(obj)
                    SimIn(i) = SimInSingle;
                end
                SimIn = reshape(SimIn, size(obj));
            end
            
            for i = 1:numel(obj)
                for j = 1:numel(obj(i).Configs)
                    SimIn(i) = obj(i).Configs(j).setSimulationInput(SimIn(i));
                end
            end
            
        end
    end
    
    %% Protected methods
    methods (Access = protected)
        
        %% initActiveVariant
        function initActiveVariant(obj)
            % Set up active variant if variant changed. Method is executed
            % at end of activateVariant method
            if ~obj.IndependentConfigs
                for i = 1:length(obj.Configs)
                    obj.Configs(i).activateVariant;
                end
            else
                warning('Coniguration set cannot be activated because ''IndependentConfigs'' property is true.')
            end
        end
        
        %% getChildren
        function ChildrenObjs = getChildren(obj)
            % Return children objects
            ChildrenObjs = obj.Configs;
        end
        
    end
end