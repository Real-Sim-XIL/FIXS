classdef BlkMaskCtrl < handle
    % BlkMaskCtrl is a base class used to control mask parameters and settings.
    %% Public properties
    properties
        BlkHdl
    end
    
    %% Protected parameters
    
    
    %% Dependent properties
    properties (Dependent)
        MaskObj
        FullBlkName
    end
    
    
    %% Public methods
    methods
        %% Constructor
        function obj = BlkMaskCtrl(Block)
            obj.BlkHdl = get_param(Block,'Handle');
        end
        
        %% get.MaskObj
        function MaskObj = get.MaskObj(obj)
            MaskObj = get_param(obj.BlkHdl, 'MaskObject');
        end
        
        %% get.FullBlkName
        function BlkName = get.FullBlkName(obj)
            BlkName = [get_param(obj.BlkHdl, 'Parent'), '/', get_param(obj.BlkHdl, 'Name')];
        end
        %% findParam
        function ParamObj = findParam(obj, ParamName)
            ParamObj = obj.MaskObj.getParameter(ParamName);
        end
        
        %% getParamValue
        function Value = getParamValue(obj, ParamName)
            ValueStruct = get_param(obj.BlkHdl, 'MaskWSVariables');
            Value = ValueStruct(strcmp({ValueStruct.Name}, ParamName)).Value;
        end
        
        %% setEnabled
        function setEnabled(obj, ParamNames, Enabled)
            obj.setParamProperty(ParamNames, 'Enabled', Enabled)
        end
        %% setVisible
        function setVisible(obj, ParamNames, Visible)
            obj.setParamProperty(ParamNames, 'Visible', Visible)
        end
        
        %% setEnabledVisible
        
    end
    
    %% Protected methods
    methods (Access = protected)
        %% setParamProperty
        function setParamProperty(obj, ParamNames, PropName, PropValue)
            ParamNames = cellstr(ParamNames);
            for i = 1:length(ParamNames)
                ParamObj = findParam(obj, ParamNames{i});
                ParamObj.(PropName) = PropValue;
            end
        end
    end
    
    
end

