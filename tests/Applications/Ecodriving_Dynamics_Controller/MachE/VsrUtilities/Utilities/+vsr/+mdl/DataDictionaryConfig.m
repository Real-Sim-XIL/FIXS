classdef DataDictionaryConfig < vsr.mdl.SimConfig
    %DataDictionaryConfig 
    % Class definition controls data from a data dictionary.
    
    %% Public properties
    properties
        DataDictionaryName char % Name of data dictionary file
    end
    
    %% Protected properties
    properties (SetAccess = protected)
        Params vsr.mdl.Param
    end
    %% Dependent properties
    properties (Dependent = true)
        DataDictionary Simulink.data.Dictionary % Data dictionary object
        DataDesignSection Simulink.data.dictionary.Section   
    end
    
    %% Private properties
    properties
        UpdatingParams = false;
    end
    
    
    %% public methods
    methods
        %% findParam
        function FoundParam = findParam(obj, ParamName)
            obj.updateParams;
            AllParamNames = {obj.Params.Name};
            FoundParam = obj.Params(strcmp(AllParamNames, ParamName));
        end
        %% createDataDictionary
        % Create data dictionary with DataDictionaryName in current
        % directory or provide new name.
        
        %% updateParams
        function updateParams(obj)
            obj.UpdatingParams = true;
            DdSectionObj = obj.DataDesignSection;
            if ~isempty(DdSectionObj)
               
                try
                    ParamEntries = DdSectionObj.find('DataSource', obj.DataDictionaryName);
                    EntryNames = {ParamEntries.Name};
                    CurrParams = obj.Params;
                    CurrParamNames = {CurrParams.Name};
                    obj.Params = vsr.mdl.Param.empty;
                    
                    %                     [~, IdxKeepParam] = intersect(EntryNames, CurrParamNames, 'stable');
                    [~, ~, RemoveOldParamIdx] = setxor(EntryNames, CurrParamNames, 'stable');
                    
                    % Add old params or new parameters
                    for i = 1:length(ParamEntries)
                        if any(strcmp(CurrParamNames, EntryNames{i}))
                            obj.Params(i) = CurrParams(strcmp(CurrParamNames, EntryNames{i}));
                            obj.Params(i).Parent = obj;
                        else
                            obj.Params(i) = vsr.mdl.Param(obj, EntryNames{i});
                            obj.Params(i).Value = DdSectionObj.evalin(EntryNames{i});
                        end
                    end
                    
                    % Remove parent from paramters that have been removed
                    for i = 1:length(RemoveOldParamIdx)
                        CurrParams(RemoveOldParamIdx(i)).Parent = vsr.mdl.DataDictionaryConfig.empty;
                    end
                catch ME
                    obj.UpdatingParams = false;
                    ME.rethrow;
                end
            else
                obj.Params = vsr.mdl.Param.empty;               
            end
            obj.UpdatingParams = false;
        end
    end
    
    %% get/set methods
    methods
        %% set.DataDictionary      
        function set.DataDictionaryName(obj, NewName)
            % check to if file exist or multiple data dictonary files with the
            % same name are on the path.
            if ~isempty(NewName)
                [~,FileName,FileExt] = fileparts(NewName);
                NewName = [FileName, '.sldd'];
            end
            obj.DataDictionaryName = NewName;
        end
        
        %% get.DataDictionary
        function DdObject = get.DataDictionary(obj)
            if ~isempty(obj.DataDictionaryName)
                DdObject = Simulink.data.dictionary.open(obj.DataDictionaryName);
                if ~obj.UpdatingParams
                    obj.updateParams;
                end
            else
                DdObject = Simulink.data.Dictionary.empty;
            end
            
        end
        
        %% get.DataDesignSection
        function DdSectionObj = get.DataDesignSection(obj)
            DdObject = obj.DataDictionary;
            if ~isempty(DdObject)
                DdSectionObj = DdObject.getSection('Design Data');
            else
                DdSectionObj = Simulink.data.dictionary.Section.empty;
            end
             
        end
        
        %% get.Params
        function ParamsOut = get.Params(obj)
            if ~obj.UpdatingParams
                obj.updateParams;
            end
            ParamsOut = obj.Params;
        end
    end
    
    %% Protected methods
    methods (Access = protected)
        
        %% getChildren
        function ChildrenObjs = getChildren(obj)
            % Return children objects
            ChildrenObjs = obj.Params;
        end
        
    end
end