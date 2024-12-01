classdef SubsystemConfig < vsr.mdl.MdlConfig
    % SUBSYSTEMCONFIG Represent subsystem config and data
    
    %% Public properties
    properties
        DataSet vsr.mdl.DataDictionaryConfig
    end
    
    %% Protected properties
    properties (SetAccess = protected)
        ChildSubsystems vsr.mdl.SubsystemConfig
    end
    
    %% Dependent properties
    properties (Dependent = true)
        
    end
    
    %% Public methods
    methods
        %% Constructor
        function obj = SubsystemConfig(varargin)
            obj@vsr.mdl.MdlConfig(varargin);
            obj.ChildSubsystems = vsr.mdl.SubsystemConfig.empty;     
        end
        
        %% addChildSubsystem
        function addChildSubsystem(obj, NewSubsystems)
            %% Determine if single or multiple children
            if length(NewSubsystems) > 1
               for i = 1:length(NewSubsystems)
                   obj.addChildSubsystem(NewSubsystems(i))
               end
               return;
            end
            
            %% Add single child
            VariantFlag = obj.isVariantSubsystem;
            for i = 1:length(NewSubsystems)
                NewSubsystems.Parent = obj;
            end
            if VariantFlag
                VariantChoices = find_system(obj.FullMdlName, 'SearchDepth', 1, 'Variants', 'AllVariants', 'BlockType', 'SubSystem');
                VariantChoices = VariantChoices(2:end);
                if ismember(NewSubsystems.FullMdlName, VariantChoices)
                    if isempty(obj.ChildSubsystems)
                        obj.ChildSubsystems = [obj.ChildSubsystems, NewSubsystems];
                    else
                        obj.ChildSubsystems.addVariant(NewSubsystems);
                    end
                else
                    error('Only subsystems 1 level below can be added as variant subsystem.')
                end
            else
                obj.ChildSubsystems = [obj.ChildSubsystems, NewSubsystems];
            end
        end
        
        %% findChildSubsystem
        function FoundSystem = findChildSubsystem(obj, SysName, IncludeInactiveVariants)
            % findChildSubsystem(obj, SysName) finds and returns a system
            % config object.
            if nargin < 3
                IncludeInactiveVariants = false;
            end
            AllChildren = [obj, obj.getAllChildren(IncludeInactiveVariants)];
            if IncludeInactiveVariants
                AllChildren = [AllChildren, obj.OtherVariants];
            end
            
            AllMdls = [AllChildren.MdlHdl];
            SearchMdlHdl = get_param(SysName, 'Handle');
            MemberLoc = ismember(AllMdls, SearchMdlHdl);
            FoundSystem = AllChildren(MemberLoc);
            
        end
        
        %% isActive
        function flag = isActive(obj)
            obj.loadMdl;
            ParentBlk = get_param(obj.FullMdlName, 'Parent');
            VariantControlLabel = get_param(obj.FullMdlName, 'VariantControl');
            CurrentVariantControlLabel = get_param(ParentBlk, 'LabelModeActiveChoice');
            if strcmp(CurrentVariantControlLabel, VariantControlLabel)
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
            obj.loadMdl;
            ParentBlk = get_param(obj.MdlHdl, 'Parent');
            VariantControlLabel = get_param(obj.FullMdlName, 'VariantControl');
            FcnHdl = @() vsr.mdl.SubsystemConfig.initActiveVariantStatic(ParentBlk, VariantControlLabel);
        end
        
        %% setSimulationInput
        function SimIn = setSimulationInput(obj, SimIn)
            % Add simulation input to Simulink.SimulationInput object
            obj.loadMdl;
            ParentBlk = get_param(obj.FullMdlName, 'Parent');
            VariantControlLabel = get_param(obj.FullMdlName, 'VariantControl');
            SimIn = SimIn.setBlockParameter(ParentBlk, 'LabelModeActiveChoice', VariantControlLabel);
        end
    end
    
    %% Set/Get methods
    methods
        %% set.DataSet
        function set.DataSet(obj, NewDataSet)
            if ~isempty(obj.DataSet)
                obj.DataSet.Parent = vsr.mdl.SubsystemConfig.empty;
            end
            obj.DataSet = NewDataSet;
            obj.DataSet.Parent = obj;
        end
    end
    
    %% Protected methods
    methods (Access = protected)
        %% initActiveVariant
        function initActiveVariant(obj)
            % Set up active variant if variant changed
            obj.loadMdl;
            ParentBlk = get_param(obj.FullMdlName, 'Parent');
            VariantControlLabel = get_param(obj.FullMdlName, 'VariantControl');
            CurrentVariantControlLabel = get_param(ParentBlk, 'LabelModeActiveChoice');
            if ~strcmp(VariantControlLabel, CurrentVariantControlLabel)
                set_param(ParentBlk, 'LabelModeActiveChoice', VariantControlLabel)
            end
        end
        
        
        %% verifyNewVariant
        function verifyNewVariant(obj, NewVariant)
            % Verify that new variant is valid variant of current object
            obj.loadMdl;
            NewVariant.loadMdl;
            if ~strcmp(get_param(obj.FullMdlName, 'Parent'), get_param(NewVariant.FullMdlName, 'Parent')) || ...
                ~strcmp(get_param(obj.FullMdlName, 'Variant'), 'off')
                error('Variant subsystem config objects must have the same variant subsystem as a parent block.')
            end
        end
        
        %% getAllChildren
        function AllChildren =  getAllChildren(obj, IncludeInactiveVariants)
            AllChildren = obj.ChildSubsystems;
            for i = 1:length(obj.ChildSubsystems)
                AllChildren = [AllChildren, obj.ChildSubsystems(i).getAllChildren];
            end
            if IncludeInactiveVariants
                for i = 1:length(AllChildren)
                    OtherVariantCheck = AllChildren.OtherVariants;
                    if ~isempty(OtherVariantCheck)
                        AllChildren = [AllChildren, OtherVariantCheck];
                    end
                end
            end
        end
        %% isVariantSubsystem
        function flag = isVariantSubsystem(obj)
            flag = false;
            obj.loadMdl;
            if strcmp(get_param(obj.FullMdlName, 'Variant'), 'on')
                flag = true;
            end
        end
    end
end

