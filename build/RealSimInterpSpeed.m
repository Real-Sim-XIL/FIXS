classdef RealSimInterpSpeed < matlab.System & matlab.system.mixin.Propagates & matlab.system.mixin.CustomIcon
    % RealSimInterpSpeed Interpolate ego vehicle speed command from interface
    
    % Public, tunable properties
    properties
        initialSpeed = 0; % initial vehicle speed
        
%         speedInterpolationMode = 0; % 0: no interpolation, 1: linear interpolation
        
        speedLookAheadHorizon = 0; % look ahead horizon in (seconds)
        
        speedUpperBound = 30; % upper bound of speed command in (meter/second)
        
    end
    
    % Public, non-tunable properties
    properties(Nontunable)
        smoothWindow (1,1){mustBeInteger,mustBePositive} = 5; % number of data point to smooth speed command
        
    end
    
    properties(DiscreteState)
        
    end
    
    % Pre-computed constants
    properties(Access = private)
        timePrevious;
        speedPrevious;
        timeNext;
        speedNext;
    
        connectionState;
        
        timeSimulator;
        simulatorStartTime;
        
        timeStepSimulator;
        timeStepSimulatorPrevious;
        
        timeReceivePrevious;
        speedReceivePrevious;
        
        simState;
        
        RealSimDelay;
        
        speedLookAheadPrevious;
        accelerationDesiredPrevious;
        
        speedInterpolationMode;
        
        speedInterpPreviousList;
        accelerationDesiredPreviousList;
    end
    
    methods
        % Constructor
        function obj = RealSimInterpSpeed(varargin)
            % Support name-value pair arguments when constructing object
%             setProperties(obj,nargin,varargin{:})
%             coder.allowpcode('plain')
        end
    end
    
    methods(Access = protected)
        %% Common functions
        function setupImpl(obj)
            % Perform one-time calculations, such as computing constants
            
            obj.timePrevious = 0;
            obj.speedPrevious = 0;
            obj.timeNext = 0;
            obj.speedNext = 0;

            obj.connectionState = 0;
            
            obj.timeSimulator = 0;
            obj.simulatorStartTime = 0;
            
            obj.timeStepSimulator = 0;
            obj.timeStepSimulatorPrevious = 0;
            
            obj.timeReceivePrevious = 0;
            obj.speedReceivePrevious = 0;
            
            obj.simState = 0;
            
            obj.RealSimDelay = 0;
            
            obj.speedLookAheadPrevious = 0;
            obj.accelerationDesiredPrevious = 0;

            obj.speedInterpolationMode = 0;
            
            obj.speedInterpPreviousList = zeros(obj.smoothWindow,1);
            obj.accelerationDesiredPreviousList = zeros(obj.smoothWindow,1);
         
        end
        
        function [speedLookAhead, accelerationDesired, timeSimulator, timeStepSimulator, timeStepTrigger, RealSimDelay] = ...
            stepImpl(obj, simulationTime, connectionState, simState, timeReceive, speedReceive, isVehicleInNetwork, speedActual)
            % INPUTS
            %   ByteData: bytes vector received from TrafficSimualtor
            %   nByte: size of bytes received
            %   initSpeed: initial speed command sent to controller m/s
            %   speedPrev: previous speed command received from TrafficSimulator
            %
            % OUTPUTS
            %   simState: simulation state. NOT actively used
            %   t: simulation time of TrafficSimulator
            %   nVeh: number of vehicle data received.
            %   speed: speed of ego vehicle in TrafficSimulator m/s
            
            speedInterp = 0;
            timeSimulator = 0;
            timeStepSimulator = 0;
            timeStepTrigger = 0;
            RealSimDelay = 0;
            speedLookAhead = 0;
            accelerationDesired = 0;
            
            % if just connected to server at current step
%             if abs(connectionState-3) < 1e-5 && abs(obj.connectionState-3) > 1e-5 
            if abs(simState - 1) < 1e-5 && abs(obj.simState) < 1e-5 && abs(obj.connectionState-3) < 1e-5
                % set connectionState and start time
                obj.simulatorStartTime = simulationTime;
                obj.simState = simState;
%                 temp = 1;
            % if just disconnected to server at current step
            elseif abs(obj.connectionState-3) < 1e-5 && abs(connectionState-3) > 1e-5
                % resetting
                obj.connectionState = 0;
                
                obj.timeSimulator = 0;
                obj.simulatorStartTime = 0;
                
                obj.timeStepSimulator = 0;
                obj.timeStepSimulatorPrevious = 0;  
                
                obj.simState = 0;
                
                obj.RealSimDelay = 0;
            end
%             temp = obj.connectionState;
            obj.connectionState = connectionState;
            
            % get absolute time in Simulator, increase every Simulink step
            if obj.simState && abs(obj.connectionState-3) < 1e-5  
                obj.timeSimulator = simulationTime-obj.simulatorStartTime;
            else
                obj.timeSimulator = -1;
            end
            
            % get time step in Simulator, change every VISSIM/SUMO step
            % as byproduct, get triggered time every 0.1 second
            if obj.simState && abs(obj.connectionState-3) < 1e-5 && (obj.timeSimulator >= obj.timeStepSimulatorPrevious + 0.1 - 1e-5 || abs(obj.simulatorStartTime-simulationTime) < 1e-5)
                obj.timeStepSimulator = obj.timeSimulator;
                obj.timeStepSimulatorPrevious = obj.timeStepSimulator; 
                timeStepTrigger = 1;
            else
                obj.timeStepSimulator = obj.timeStepSimulatorPrevious;
            end
            
            timeSimulator = obj.timeSimulator;
            timeStepSimulator = obj.timeStepSimulator;
            
            
            % check if has new data from traffic simulator
            if timeReceive >= 0.2 && abs(timeReceive-obj.timeReceivePrevious) > 1e-5
                hasNewData = 1;
            else
                hasNewData = 0;
            end
            
            % speed handling
            if obj.simState && obj.connectionState && isVehicleInNetwork
                %%% interpolation
                if obj.speedInterpolationMode == 0
                    speedInterp = speedReceive;
                elseif obj.speedInterpolationMode == 1
                    % update interpolation point
                    if hasNewData == 1
                        obj.timePrevious = obj.timeNext;
                        obj.timeNext = obj.timeStepSimulator+0.1; 
                        obj.speedPrevious = obj.speedNext;
                        obj.speedNext = speedReceive;
                    end
                    
                    % doing interpolation
                    speedTemp = interp1([obj.timePrevious, obj.timeNext], [obj.speedPrevious, obj.speedNext], obj.timeSimulator, 'linear');
                    if obj.timeSimulator > obj.timeNext
                        speedInterp = obj.speedNext;
                    else
                        speedInterp = speedTemp;
                    end
                    
                end
            else
                % use initial speed
                speedInterp = obj.initialSpeed;
            end
            
            % calculate a lookahead speed accounting for dynamics if
            % traffic simulator is determining the desired speed          
            if isVehicleInNetwork
                % should update lookahead speed whenever receive new data from traffic simulator
                if hasNewData == 1
                    % speedInterp is from SUMO, speedActual
                    % due to tcp/ip delay, the remaining time could be less
                    % than 0.1 seconds
                    dtRem = 0.1-mod(obj.timeSimulator,0.1);
                    accelerationDesiredCurrent = min(max(-5, (speedInterp-speedActual)/dtRem), 5);
                    speedLookAheadCurrent = min(max(0, speedActual + obj.speedLookAheadHorizon*accelerationDesiredCurrent), obj.speedUpperBound);
                    
                    if obj.smoothWindow >= 2
                        obj.speedInterpPreviousList(1:obj.smoothWindow-1) = obj.speedInterpPreviousList(2:obj.smoothWindow);
                        obj.speedInterpPreviousList(end) = speedInterp;
                        
                        obj.accelerationDesiredPreviousList(1:obj.smoothWindow-1) = obj.accelerationDesiredPreviousList(2:obj.smoothWindow);
                        obj.accelerationDesiredPreviousList(end)=accelerationDesiredCurrent;
                        
                        accelerationDesired = min(max(-5, (mean(obj.speedInterpPreviousList)-speedActual)/dtRem), 5);                        
%                         accelerationDesired = mean(obj.accelerationDesiredPreviousList);
                        speedLookAhead = min(max(0, speedActual + obj.speedLookAheadHorizon*accelerationDesired), obj.speedUpperBound);
                    else
                        accelerationDesired = accelerationDesiredCurrent;
                        speedLookAhead = speedLookAheadCurrent;
                    end
                    
                    obj.accelerationDesiredPrevious = accelerationDesired;
                    obj.speedLookAheadPrevious = speedLookAhead;
                else
                    accelerationDesired = obj.accelerationDesiredPrevious;
                    speedLookAhead = obj.speedLookAheadPrevious;
                end
            else
                accelerationDesired = 0;
                speedLookAhead = speedInterp;
            end

            
            temp = obj.simulatorStartTime;
            
            % calc delay
            if timeReceive >= 0.2 && abs(timeReceive-obj.timeReceivePrevious) > 1e-5
                obj.RealSimDelay = obj.timeSimulator + 0.2 - timeReceive;
                obj.timeReceivePrevious = timeReceive;
            end
            
            RealSimDelay = obj.RealSimDelay;
            
        end
        
        
        function resetImpl(obj)
            % Initialize / reset discrete-state properties

        end

        function validateInputsImpl(obj, simulationTime, connectionState, simState, timeReceive, speedReceive, isVehicleInNetwork, speedActual)
            % Validate inputs to the step method at initialization

        
        end

%         function [name,name2,name3,name4] = getInputNamesImpl(obj)
%             % Return input port names for System block
%             name = 'simulationTime';
%             name2 = 'connectionState';
%             name3 = 'timeReceive';
%             name4 = 'speedReceive';
%         end
% 
%         function [name,name2,name3,name4] = getOutputNamesImpl(obj)
%             % Return output port names for System block
%             name = 'speedInterp';
%             name2 = 'timeSimulator';
%             name3 = 'timeStepSimulator';
%             name4 = 'timeStepTrigger';
%         end

        function [sz1, sz2, sz3, sz4, sz5, sz6] = getOutputSizeImpl(obj)
            % Maximum length of linear indices and element vector is the
            % number of elements in the input
            sz1 = 1; sz2 = 1; sz3 = 1; sz4 = 1; sz5 = 1; sz6 = 1;
        end
        
        function [fz1, fz2, fz3, fz4, fz5, fz6] = isOutputFixedSizeImpl(~)
            %Both outputs are always fixed
            fz1 = 1; fz2 = 1; fz3 = 1; fz4 = 1; fz5 = 1; fz6 = 1;
        end
        
        function [dt1, dt2, dt3, dt4, dt5, dt6] = getOutputDataTypeImpl(obj)
            dt1 = 'double'; dt2 = 'double'; dt3 = 'double'; dt4 = 'double'; dt5 = 'double'; dt6 = 'double';
        end
        
        function [cp1, cp2, cp3, cp4, cp5, cp6] = isOutputComplexImpl(obj)
            cp1 = false; cp2 = false; cp3 = false; cp4 = false; cp5 = false; cp6 = false;
        end
        
    end
    
    
    
    methods(Access = private)
        

        
    %%% END OF PRIVATE METHODS    
    end 
    
   
%%% END OF CLASS
end
