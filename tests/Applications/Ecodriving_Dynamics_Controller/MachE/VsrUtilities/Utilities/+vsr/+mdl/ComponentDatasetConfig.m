classdef ComponentDatasetConfig < vsr.mdl.DataDictionaryConfig
    %ComponentSubsystemConfig Configures data and blocks for specific
    %components such as a mapped engine or transmission
    %   

    %% Protected properties
    properties (Hidden = true)
        ParamNameMap containers.Map % Maps class property names to names to data dictionary names
    end
    
    %% Public methods
    methods
        %% pushToMdl
        function pushToDictionary(obj)
            % pushToDictionary updates the data dictionary from
            % this object's properties.
            %% Set data values
            obj.verifyParams;
            DataDesignSection = obj.DataDesignSection;
            PropertyNames = obj.ParamNameMap.keys;
            for i = 1:length(PropertyNames)
                DataDesignSection.assignin(obj.ParamNameMap(PropertyNames{i}), obj.(PropertyNames{i}))
                ParamObj = obj.findParam(obj.ParamNameMap(PropertyNames{i}));
                ParamObj.Value = obj.(PropertyNames{i});
            end
            
        end
        
        %% verifyParamValues
        function verifyParamValues(obj)
             %verifyParamValues(obj) check parameter values before updating
             %data dictionary. Use method in derived class definitions for
             %specific components.
        end
        
        %% pullFromDictionary
        function pullFromDictionary(obj)
            PropertyNames = obj.ParamNameMap.keys;
            for i = 1:length(PropertyNames)
                Param = obj.findParam(obj.ParamNameMap(PropertyNames{i}));
                if ~obj.findprop(PropertyNames{i}).Dependent
                    obj.(PropertyNames{i}) = Param.Value;
                end
            end
        end
        
        
        
%         %% connectDataDictionary
%         function connectDataDictionary(obj, DataDictionaryName)
%             %connectDataDictionary(obj, DataDictionaryName) connect object
%             %to data dictionary DataDictionaryName. Parameters for the
%             %component will be saved or loaded from data dictionary.
%             obj.DataDictionaryName = DataDictionaryName;
%         end
%         
        %% mapParam
        function mapParam(obj, PropertyName, ParamName)
            %mapParam(obj, PropertyName, ParamName) maps class definition
            %property PropertyName to data dictionary parameter ParamName.
            %The mapping is used to push parameters to the data dictionary.
            %The property name and parameter name are asssumed equal if
            %ParamName is not provided.
            if nargin < 3
                ParamName = PropertyName;
            end
            obj.ParamNameMap(PropertyName) = ParamName;
            
        end
        
        
        %% getParamSummaryTbl
        function SummaryTbl = getParamSummaryTbl(obj)
            DataSection = obj.DataDesignSection;
            PropertyNames = obj.ParamNameMap.keys;
            ParamNames = cell(size(PropertyNames))';
            ObjectValues = ParamNames;
            DataDictionaryValues = ParamNames;
            isEqual = ParamNames;
            for i = 1:length(PropertyNames)
                ParamNames{i} = obj.ParamNameMap(PropertyNames{i});
                ObjectValues{i} =  obj.(PropertyNames{i});
                Param = obj.findParam(ParamNames{i});
                if isempty(Param)
                    DataDictionaryValues{i} = nan;
                else
                    DataDictionaryValues{i} = Param.Value;
                end
                if isa(DataDictionaryValues{i}, 'Simulink.Parameter')
                    DdValue = DataDictionaryValues{i}.Value;
                else
                    DdValue = DataDictionaryValues{i};
                end
                isEqual{i} = isequal(ObjectValues{i}, DdValue);
                
            end
            SummaryTbl = table(ParamNames, ObjectValues, DataDictionaryValues, isEqual);
            
        end
        
    end
    
    %% Protected methods
    methods (Access = protected)
        
    end
end

