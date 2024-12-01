classdef HilLimitSystemBlock < matlab.System & matlab.system.mixin.Propagates ...
        & matlab.system.mixin.CustomIcon
    
    % Public, non-tunable properties
    properties(Nontunable)
        OutputTbls (1,1) = true; % Output ranges and flags for each value input
        ValueNames  uint8   % Limit parameter names
    end
    
    % Public, tunable properties
    properties
        LowerLimit double   % Lower limit table
        LowerWarning double % Lower warning table
        UpperWarning double % Upper warning table
        UpperLimit double   % Upper limit
    end
    
    properties(DiscreteState)
        
    end
    
    % Pre-computed constants
    properties(Access = private)
        NumInputs                   % Number input values
        PrevWarningTbl              % Previous time step warning flag
        PrevLimitReachedTbl         % Previous time step limit flag
        PrevEnable                  % Previous time step enable flag
        MinValue                    % Array of minimum value input
        MaxValue                    % Array of maximum value input
        TimeSinceMinReached         % Array with times since last minimum reached
        TimeSinceMaxReached         % Array with times since last maximum reached
        PrevResetValue = false;         % Previous time step reset flag
        Values                      % Values to be checked
    end
    
    methods
        % Constructor
        function obj = HilLimitSystemBlock(varargin)
        end
    end
    
    methods(Access = protected)
        %% Common functions
        function setupImpl(obj)
            % Perform one-time calculations, such as computing constants
            obj.NumInputs = length(obj.ValueNames(obj.ValueNames == 44)) + 1;
            obj.PrevWarningTbl = false(length(obj.LowerLimit),1);
            obj.PrevLimitReachedTbl = false(length(obj.LowerLimit),1);
            obj.MinValue = zeros(length(obj.LowerLimit),1);
            obj.MaxValue = zeros(length(obj.LowerLimit),1);
            obj.TimeSinceMinReached = zeros(length(obj.LowerLimit),1);
            obj.TimeSinceMaxReached = zeros(length(obj.LowerLimit),1);
            obj.Values = zeros(length(obj.LowerLimit),1);
            obj.PrevResetValue = false;
            obj.resetValues;
        end
        
        function [Warning, LimitReached, WarningTbl, LimitReachedTbl, ValTbl, ValMinTbl, ValMaxTbl]  = stepImpl(obj, Enable, Reset, varargin)
            % stepImpl determines if inputs are outside the warning or hard
            % limits
            
            %% Enable 
            if ~Enable
                obj.resetValues;
            end
            
            %% Reset
            if Reset && ~obj.PrevResetValue
                obj.resetValues;
            end
            if Reset
                obj.PrevResetValue = true;
            else
                obj.PrevResetValue = false;
            end
            
            %% Get value flags
            coder.unroll();
            for i = 1:nargin-3
                obj.Values(i) = varargin{i};
            end
            LimitLowerReached = obj.Values(1:obj.NumInputs) <= obj.LowerLimit;
            WarningLowerReached = obj.Values(1:obj.NumInputs) <= obj.LowerWarning;
            WarningUpperReached = obj.Values(1:obj.NumInputs) >= obj.UpperWarning;
            LimitUpperReached = obj.Values(1:obj.NumInputs) >= obj.UpperLimit;
            WarningCurr = WarningLowerReached | WarningUpperReached;
            LimitReachedCurr = LimitLowerReached | LimitUpperReached;
            if ~Enable
                LimitReachedCurr(:) = false;
%                 WarningCurr(:) = false;
            end
            
            WarningTbl = WarningCurr | obj.PrevWarningTbl;
            LimitReachedTbl = LimitReachedCurr | obj.PrevLimitReachedTbl;
            Warning = any(WarningTbl);
            LimitReached = any(LimitReachedTbl);

            obj.PrevWarningTbl = WarningTbl;
            obj.PrevLimitReachedTbl = LimitReachedTbl;
            
            %% Set minimum and maximum values
            obj.MinValue = min(obj.MinValue, obj.Values);
            obj.MaxValue = max(obj.MaxValue, obj.Values);
            ValMinTbl = obj.MinValue;
            ValMaxTbl = obj.MaxValue;
            ValTbl = obj.Values;
        end
        
        function resetImpl(obj)
            % Initialize / reset discrete-state properties
        end
        
        %% Backup/restore functions
        function s = saveObjectImpl(obj)
            % Set properties in structure s to values in object obj
            
            % Set public properties and states
            s = saveObjectImpl@matlab.System(obj);
            
            % Set private and protected properties
            %s.myproperty = obj.myproperty;
        end
        
        function loadObjectImpl(obj,s,wasLocked)
            % Set properties in object obj to values in structure s
            
            % Set private and protected properties
            % obj.myproperty = s.myproperty;
            
            % Set public properties and states
            loadObjectImpl@matlab.System(obj,s,wasLocked);
        end
        
        %% Simulink functions
        function ds = getDiscreteStateImpl(obj)
            % Return structure of properties with DiscreteState attribute
            ds = struct([]);
        end
        
        %% Inputs
        function numIn = getNumInputsImpl(obj)
            numIn = length(obj.ValueNames(obj.ValueNames == 44)) + 3;
        end
        
        function NamesIn = getInputNamesImpl(obj)
            NamesIn = ["Enable", "Reset"];
            NumIn = length(obj.ValueNames(obj.ValueNames == 44)) + 1;
            PortStr = [44, obj.ValueNames(2:end-1), 44];
            CommaLoc = 1:length(PortStr);
            CommaLoc = CommaLoc(PortStr == 44);
            for i = 1:NumIn
                NamesIn = [NamesIn, string(char(PortStr((CommaLoc(i)+2):(CommaLoc(i+1)-2))))];
            end
        end
        
        %% Outputs
        function NumOutputs = getNumOutputsImpl(obj)
            if obj.OutputTbls
                NumOutputs = 7;
            else
                NumOutputs  = 2;
            end
        end
        function varargout = getOutputSizeImpl(obj)
            % Return size for each output port
            varargout{1} = [1 1];
            varargout{2} = [1 1];
            numIn = getNumInputsImpl(obj) - 2;
            for i = 3:nargout
                varargout{i} = [numIn, 1];
            end
        end
        
        function varargout = isOutputFixedSizeImpl(obj)
            for i = 1:nargout
                varargout{i} = true;
            end
        end
        
        function varargout = getOutputDataTypeImpl(obj)
            varargout{1} = 'boolean';
            varargout{2} = 'boolean';
            if nargout > 2
                varargout{3} = 'boolean';
                varargout{4} = 'boolean';
                for i = 5:nargout
                    varargout{i} = 'double';
                end
            end
            
        end
        
        function varargout = isOutputComplexImpl(obj)
            for i = 1:nargout
                varargout{i} = false;
            end
        end
        
        %% Icon
        function icon = getIconImpl(obj)
            % Define icon for System block
            icon = mfilename("class"); % Use class name
            % icon = "My System"; % Example: text icon
            % icon = ["My","System"]; % Example: multi-line text icon
            % icon = matlab.system.display.Icon("myicon.jpg"); % Example: image file icon
        end
        %% resetValues
        function resetValues(obj)
            % resetValues Rest warning flags and min and max values
            obj.PrevWarningTbl(:) = false;
            obj.PrevLimitReachedTbl(:) = false;
            obj.MinValue(:) = nan;
            obj.MaxValue(:) = nan;
            obj.TimeSinceMinReached(:) = 0;
            obj.TimeSinceMaxReached(:) = 0;
        end
    end
    
    methods(Static, Access = protected)
        %% Simulink customization functions
        function header = getHeaderImpl
            % Define header panel for System block dialog
            header = matlab.system.display.Header(mfilename("class"));
        end
        
        function group = getPropertyGroupsImpl
            % Define property section(s) for System block dialog
            group = matlab.system.display.Section(mfilename("class"));
        end
        
        
    end
end
