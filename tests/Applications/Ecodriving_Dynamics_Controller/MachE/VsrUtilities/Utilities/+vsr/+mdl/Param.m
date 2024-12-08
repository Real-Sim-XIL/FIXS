classdef Param < vsr.mdl.SimConfig
    
    %% Public properties
    properties
        Value
    end
    
    %% Dependent parameters
    properties (Dependent = true)
        DataDictionaryEntry Simulink.data.dictionary.Entry
    end
    
    %% Private properties
    properties (Access = private, Hidden = true)
        UpdatingName = false;
        OldValue
    end
    %% Public methods
    methods
        %% Constructor
        function obj = Param(ParentObj, ParamName)
            obj.Parent = ParentObj;
            obj.UpdatingName = true;
            obj.Name = ParamName;
            obj.UpdatingName = false;
            
            DataEntry = obj.DataDictionaryEntry;
            obj.Value = DataEntry.getValue;
        end
        
        %% getActivateFcnHdl
        function FcnHdl = getActivateFcnHdl(obj)
            % Create function handle for activating variant without
            % requiring handle objects. This is neccessary for running in
            % parallel.
            ActiveVariantObj = obj.getActiveVariant;
            FcnHdl = @() vsr.mdl.Param.initActiveVariantStatic(ActiveVariantObj.Parent.DataDictionaryName, obj.Name, obj.Value);
        end
        %% setSimulationInput
        function SimIn = setSimulationInput(obj, SimIn)
            SimIn = SimIn.setVariable(obj.Name, obj.Value);
        end
        
        %% isActive
        function flag = isActive(obj)
            DataEntry = obj.DataDictionaryEntry;
            ActValue = DataEntry.getValue;
            
            if isequal(ActValue, obj.Value)
                flag = true;
            else
                flag = false;
            end
        end
        
        
    end
    
    %% Get/set methods
    methods
        
        %% get.DataDictionaryEntry
        function EntryObj = get.DataDictionaryEntry(obj)
            if ~isempty(obj.Parent)
                EntryObj = obj.Parent.DataDesignSection.find('Name', obj.Name);
                if isempty(EntryObj) && ~obj.UpdatingName
                    obj.Parent = vsr.mdl.DataDictionaryConfig.empty;
                end
            else
                EntryObj = Simulink.data.dictionary.Entry.empty;
            end
        end
    end
    
    
    %% Protected methods
    methods (Access = protected)
        %% updateName
        function updateName(obj, NewName)
            if ~obj.UpdatingName
                % Set up
                ActiveVariantObj = obj.getActiveVariant;
                OtherVariantObjs = ActiveVariantObj.OtherVariants;
                
                % Set data dictionary entry name
                DataDictionaryEntryObj = ActiveVariantObj.DataDictionaryEntry;
                if ~isempty(ActiveVariantObj.Name)
                    DataDictionaryEntryObj.Name = NewName;
                end
                
                % Set variant object names
                AllObjs = [ActiveVariantObj, OtherVariantObjs];
                for i = 1:length(AllObjs)
                    AllObjs(i).UpdatingName = true;
                    AllObjs(i).Name = NewName;
                    AllObjs(i).UpdatingName = false;
                end
            end
            obj.UpdatingName = false;
        end
        
        %% initActiveVariant
        function initActiveVariant(obj)
            DataEntry = obj.DataDictionaryEntry;
            if ~isempty(DataEntry)
                DataEntry.setValue(obj.Value);
            end
        end
    end
    
end