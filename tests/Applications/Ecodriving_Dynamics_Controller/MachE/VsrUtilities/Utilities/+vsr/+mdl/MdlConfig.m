classdef MdlConfig < vsr.mdl.SimConfig
    % MdlConfig
    %   Base class definition
    
    %% Public properties
    properties
        FullMdlName char
    end
    
    %% Dependent properties
    properties (Dependent)
        RootMdlName char
        RootDataSet vsr.mdl.DataDictionaryConfig
        SystemSettingConfig vsr.mdl.SystemSettingConfig
    end
    
    %% Protected properties
    properties (SetAccess = protected)
        MdlSettings vsr.mdl.MdlSettingConfig
    end
    properties (Access = protected, Hidden)
        MdlHdl
        LoadingMdl = false
        UpdatingName = false;
        UpdatingMdlSetting = false;
    end
    
    %% Public methods
    methods
        %% Constructor
        function obj = MdlConfig(Mdl)
            % MdlConfig constructor
            % Create object to represent model by providing model name or
            % handle
            obj.MdlHdl = -1;
            if ishandle(Mdl)
                obj.FullMdlName = getBlockName(Mdl);
            else
                obj.FullMdlName = Mdl;
            end
            obj.loadMdl
        end
        
        %% addMdlSetting
        function addMdlSetting(obj, MdlSettingObj)
            if ~obj.UpdatingMdlSetting
                obj.UpdatingMdlSetting = true;
                CurrMdlSettings = {obj.MdlSettings.Name};
                NewMdlSettingName = MdlSettingObj.Name;
                MdlSettingObj.ParentMdlConfig = obj;
                [~,IA] = intersect(CurrMdlSettings, NewMdlSettingName);
                if isempty(IA)
                    obj.MdlSettings = [obj.MdlSettings, MdlSettingObj];
                else
                    SameSettingObj = obj.MdlSettings(IA);
                    if ~isequal(SameSettingObj, MdlSettingObj)
                        SameSettingObj.addVariant(MdlSettingObj);
                    end
                end
                obj.UpdatingMdlSetting = false;
            end
        end
    end
    
    %% get/set methods
    methods
        
        %% get.RootMdlName
        function RootMdl = get.RootMdlName(obj)
            
            RootMdl = bdroot(obj.FullMdlName);
            
        end
        
        %% get.FullMdlName
        function MdlName = get.FullMdlName(obj)
            if ishandle(obj.MdlHdl)
                if ~strcmp(obj.FullMdlName, getBlockName(obj.MdlHdl))
                    obj.FullMdlName = getBlockName(obj.MdlHdl);
                end
            else
                if ~obj.LoadingMdl
                    obj.loadMdl;
                    obj.MdlHdl = get_param(obj.FullMdlName, 'Handle');
                end
            end
            MdlName = obj.FullMdlName;
        end
        %% set.FullMdlName
        function set.FullMdlName(obj, NewName)
            obj.FullMdlName = NewName;
            obj.loadMdl;
            obj.Name = get_param(obj.FullMdlName, 'Name');
            obj.MdlHdl = get_param(obj.FullMdlName, 'Handle');
        end
        
%         %% get.MdlHdl
%         function Val = get.MdlHdl(obj)
%             Val = get_param(obj.FullMdlName, 'Handle');
%         end
    end
    %% Protected methods
    methods (Access = protected)
        %% updateName
        function updateName(obj, NewName)
            obj.loadMdl
            if ~strcmp(get_param(obj.FullMdlName, 'Name'), NewName)
                set_param(obj.FullMdlName, 'Name', NewName);
            end
            if ~obj.UpdatingName
                obj.UpdatingName = true;
                obj.FullMdlName = getBlockName(obj.MdlHdl);
                obj.UpdatingName = false;
            end
        end
        
        %% getChildren
        function ChildrenObjs = getChildren(obj)
            % Return children objects
            ChildrenObjs = obj.MdlSettings;
        end
    end
    
    %% Private methods
    methods (Access = protected)
        %% loadMdl
        function loadMdl(obj)
            
            if ~ishandle(obj.MdlHdl) && ~obj.LoadingMdl
                obj.LoadingMdl = true;
                Name = obj.FullMdlName;
                obj.LoadingMdl = false;
                s = strfind(Name, '/');
                if isempty(s)
                    RootMdlFromFullName = Name;
                else
                    RootMdlFromFullName = Name(1:(s(1)-1));
                end
                if ~bdIsLoaded(RootMdlFromFullName)
                    load_system(RootMdlFromFullName)
                end
            end
        end
    end
end

