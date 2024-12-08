classdef SystemConfig < vsr.mdl.MdlConfig
    %SYSTEMCONFIG
    % Handle model changes for
    
    properties
        ConfigSet vsr.mdl.SystemSettingsConfig
        DataSet vsr.mdl.DataDictionaryConfig
    end
    
    %% Public methods
    methods
        %% Constructor
        function obj = SystemConfig(varargin)
            obj@vsr.mdl.MdlConfig(varargin);
        end
        
        %% set.ConfigSet
        function set.ConfigSet(obj, NewConfigSet)
            
        end
    end
    
    %% Protected methods
    methods (Access = protected)
        
        %% initActiveVariant
        function initActiveVariant(obj)
            % Set up active variant if variant changed. Method is executed
            % at end of activateVariant method
            
        end
        
        %% verifyNewVariant
        function verifyNewVariant(obj, NewVariant)
            % Verify that new variant is valid variant of current object
            if ~isa(NewVariant, 'vsr.mdl.SystemConfig')
                error('Variant for a ''vsr.mdl.SystemConfig'' object can only be another ''vsr.mdl.SystemConfig''')
            end
        end
        
        %% getChildren
        function ChildrenObjs = getChildren(obj)
            % Return children objects
            ChildrenObjs = ConfigSet;
        end
        
    end
end

