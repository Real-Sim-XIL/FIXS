classdef MdlSettingConfig < vsr.mdl.SimConfig
    %BlkSettingConfig 
    % Class definition controls block mask settings such as a popup or
    % checkbox. Use vsr.mdl.Param or vsr.mdl.DataDictionaryConfig to change
    % parameter values.
    
    
    %% Public properties
    properties 
        SettingValue char; % String value to apply to block
        ParentMdlConfig vsr.mdl.MdlConfig
    end
    
    %% Public methods
    methods
        %% Constructor
        function obj = MdlSettingConfig(ParamName, ParamValue)
            obj.Name = ParamName;
            if nargin >= 2
                obj.SettingValue = ParamValue;
            end
        end
        
        %% set.ParentMdlConfig
        function set.ParentMdlConfig(obj, ParentMdlObj)
            % Use addMdlSetting to set parent model config object
            if isempty(obj.ParentMdlConfig)
                ParentMdlObj.addMdlSetting(obj);
                obj.ParentMdlConfig = ParentMdlObj;
            elseif obj.ParentMdlConfig ~= ParentMdlObj 
                ParentMdlObj.addMdlSetting(obj);
                obj.ParentMdlConfig = ParentMdlObj;
            end
            obj.Parent = obj.ParentMdlConfig;
        end
        
        %% isActive
        function flag = isActive(obj)
            CurrSettingValue = get_param(obj.ParentMdlConfig.FullMdlName, obj.Name);
            if strcmp(CurrSettingValue, obj.SettingValue)
                flag = true;
            else
                flag = false;
            end
        end
        
        %% getActivateFcnHdl
        function FcnHdl = getActivateFcnHdl(obj)
            % Create function handle for activating variant without
            % requiring handle objects. This is neccessary for running in
            % parallel.
            FcnHdl = @() vsr.mdl.MdlSettingConfig.initActiveVariantStatic(obj.ParentMdlConfig.FullMdlName, obj.Name, obj.SettingValue);
        end
        %% setSimulationInput
        function SimIn = setSimulationInput(obj, SimIn)
            % Add simulation input to Simulink.SimulationInput object
            SimIn = SimIn.setBlockParameter(obj.ParentMdlConfig.FullMdlName, obj.Name, obj.SettingValue);
        end
    end
    
    %% Protected methods
    methods (Access = protected)
        %% updateName
        function updateName(obj, NewName)
            % Used by derived classes to update information when the object
            % name is changed. For example check if other objects have the
            % same name or update data dictionary parameter name when name
            % is changed.
        end
        
        %% startVariantChange
        function startVariantChange(obj)
            % Set up active variant if variant changed. Method is executed
            % at start of activateVariant method
            
        end
        
        %% initActiveVariant
        function initActiveVariant(obj)
            % Set up active variant if variant changed. Method is executed
            % at end of activateVariant method
%             obj.ParentMdlConfig.loadMdl;
            set_param(obj.ParentMdlConfig.FullMdlName, obj.Name, obj.SettingValue)
        end
        
        %% verifyNewVariant
        function verifyNewVariant(obj, NewVariant)
            % Verify that new variant is valid variant of current object
            if isempty(obj.ParentMdlConfig)
                error('Variant setting must have a parent model before')
            end
            if isempty(NewVariant.ParentMdlConfig)
                NewVariant.ParentMdlConfig = obj.ParentMdlConfig;
            end
            if obj.ParentMdlConfig ~= NewVariant.ParentMdlConfig
                error('Variants of model settings must have the same parent model.')
            end
        end
    end
    
    %% Static methods
    methods (Static)
        %% initActiveVariantStatic
        function initActiveVariantStatic(BlkName, ParamName, ParamValue)
            set_param(BlkName, ParamName, ParamValue)
        end
    end
end