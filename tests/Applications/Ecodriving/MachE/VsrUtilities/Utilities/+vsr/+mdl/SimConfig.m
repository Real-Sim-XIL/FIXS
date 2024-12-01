classdef SimConfig < matlab.mixin.Copyable & matlab.mixin.Heterogeneous & matlab.mixin.SetGet
    % Class definition SimConfig is the base class for all the
    % model configuration class definitions. The class handles naming and tagging
    % configuration objects. Behaving like a linked list, the class points
    % to other variants.
    %% Properties
    properties
        Name char % Name can be used for searching and paramter values
        Tag  % Tags can be used to group multiple objects and searching
        Label % Used to display in UI (display label becomes Name if empty)
    end
    
    %% Protected properties
    properties (SetAccess = protected)
        Parent vsr.mdl.SimConfig   
        
    end
    properties (SetAccess = protected, Hidden = true)
        LabelProp
    end
    %% Dependent properties
    properties (Dependent) 
        Children vsr.mdl.SimConfig
        OtherVariants vsr.mdl.SimConfig  
        AllVariants vsr.mdl.SimConfig
    end
    
    %% Private properties
    properties (Access = private)
        NextVariant vsr.mdl.SimConfig
        PrevVariant vsr.mdl.SimConfig
    end
    
    %% Public methods
    methods
        function set.Name(obj, NewName) 
            if ~strcmp(obj.Name, NewName)
                obj.updateName(NewName);
            end
            obj.Name = NewName;
        end
        
        %% isActive
        function flag = isActive(obj)
            flag = false;
        end
        
        %% getActiveVariant
        function ActiveVariant = getActiveVariant(obj)
            % Returns active variants. output is empty if no variant is
            % active.
            ActiveVariant = vsr.mdl.SimConfig.empty;
            AllVariantsTemp = obj.AllVariants;
            for i = 1:length(AllVariantsTemp)
                if AllVariantsTemp(i).isActive
                    ActiveVariant = [ActiveVariant, AllVariantsTemp(i)];
                end
            end
        end
        
        %% activateVariant
        function activateVariant(obj)
            % Sets current object as the active variant and updates model
            if ~obj.isActive
                obj.startVariantChange;
                obj.initActiveVariant;
            end       
        end
        
        %% isVariantActivationTunable
        % Returns true if changing variant is tunable. Only true for
        % parameters or sets of parameters that are new and update
        % "Tunable" parameters.
        
        %% getAllChildren
        % Returns all childen below object level
        
       
        %% findbyTag
        
        %% addVariant
        function addVariant(obj, NewVariant)
            % Add new configuration variant to variant list
            obj.verifyNewVariant(NewVariant);
            NewVariant.removeVariant;
            NewVariant.Parent = obj.Parent;
            Next = obj.NextVariant;
            NewVariant.NextVariant = Next;
            NewVariant.PrevVariant = obj;  
            obj.NextVariant = NewVariant;
            if ~isempty(Next)
                Next.PrevVariant = NewVariant;
            end
        end
        
        %% createVariant
        function NewVariant = createVariant(obj, varargin)
            % createVariant(obj) copies obj and adds as new variant
            %
            % NewVariant = createVariant(obj, PropName1, PropValue, PropName2, PropValue2, ...)
            % sets properties based on property name/value pairs 
            NewVariant = obj.copy;
            NewVariant.PrevVariant = vsr.mdl.SimConfig.empty;
            NewVariant.NextVariant = vsr.mdl.SimConfig.empty;
            for i = 1:2:length(varargin)
                NewVariant.(varargin{i}) = varargin{i+1};
            end
            obj.addVariant(NewVariant);
        end
      
        
        %% removeVariant
        function removeVariant(obj)
            PrevNode = obj.PrevVariant;
            NextNode = obj.NextVariant;
            if ~isempty(PrevNode)
                PrevNode.NextVariant = NextNode;
            end
            if ~isempty(NextNode)
                NextNode.PrevVariant = PrevNode;
                if isempty(PrevNode)
                    NextNode.initActiveVariant
                end
            end
            
            obj.PrevVariant = vsr.mdl.SimConfig.empty;
            obj.NextVariant = vsr.mdl.SimConfig.empty;
            obj.Parent = vsr.mdl.SimConfig.empty;
        end
        
        %% setSimulationInput
        function SimIn = setSimulationInput(obj, SimIn)
            % Add simulation input to Simulink.SimulationInput object
            
        end
        
        %% get.OtherVariants
        function OtherVariantList = get.OtherVariants(obj)
            OtherVariantList = vsr.mdl.SimConfig.empty;
            Prev = obj.PrevVariant;
            while ~isempty(Prev)
                CurrVariant = Prev;
                OtherVariantList = [Prev, OtherVariantList];
                Prev = CurrVariant.PrevVariant;
            end
            
            Next = obj.NextVariant;
            while ~isempty(Next)
                CurrVariant = Next;
                OtherVariantList = [OtherVariantList, Next];
                Next = CurrVariant.NextVariant;   
            end
            
        end
        %% get.AllVariants
        function VariantList = get.AllVariants(obj)
            VariantList = [obj, obj.OtherVariants];
        end
        
        %% get.Children
        function Vals = get.Children(obj)
            Vals = obj.getChildren;
        end
        
        %% set.Label
        function set.Label(obj, Str)
            obj.Label = Str;
            obj.LabelProp = Str;
        end
        %% get.Label
        function LabelTxt = get.Label(obj)
            if isempty(obj.LabelProp)
                LabelTxt = obj.Name;
            else
                LabelTxt = obj.Label;
            end
        end
    end
    
    %% Sealed methods
    methods (Sealed)
         %% findByName
        function FoundObjs = findByName(obj, SearchName)
            % Searches all children based on name
            FoundObjs = vsr.mdl.SimConfig.empty;
            if length(obj) > 1
                for i = 1:length(obj)
                    FoundObjs = [FoundObjs, obj(i).findByName(SearchName)];
                end
            else
                if strcmp(SearchName, obj.Name)
                    FoundObjs = obj;
                end
                if ~isempty(obj.Children)
                    FoundObjs = [FoundObjs, obj.Children.findByName(SearchName)];
                end
            end
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