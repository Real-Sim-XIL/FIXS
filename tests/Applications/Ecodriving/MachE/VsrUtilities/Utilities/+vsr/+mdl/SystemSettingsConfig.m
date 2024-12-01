classdef SystemSettingsConfig < vsr.mdl.SimConfig
    %SYSTEMSETTINGSCONFIG handle changing model configuration sets
    %(Simulink.Config). The class definition assumes the configuration set
    % is saved in a data dictionary.
    
    %% Public methods
    methods 
        %% Constructor
        function obj = SystemSettingsConfig(ParentIn)
            obj.Parent = ParentIn;
        end
        
        %% isActive
        function flag = isActive(obj)
            CurrConfigSet = getActiveConfigSet(obj.Parent.FullMdlName);
            if strcmp(obj.Name, CurrConfigSet.Name)
                flag = true;
            else
                flag = false;
            end
        end
    end
    %% Protected methods
    methods (Access = protected)

        %% initActiveVariant
        function initActiveVariant(obj)
            % Set up active variant if variant changed. Method is executed
            % at end of activateVariant method
            setActiveConfigSet(obj.Parent.FullMdlName, obj.Name)

        end
        
        %% verifyNewVariant
        function verifyNewVariant(obj, NewVariant)
            % Verify that new variant is valid variant of current object
        end
        
        %% getChildren
        function ChildrenObjs = getChildren(obj)
            % Return children objects
            ChildrenObjs = vsr.mdl.SimConfig.empty;
        end
        
    end
end

