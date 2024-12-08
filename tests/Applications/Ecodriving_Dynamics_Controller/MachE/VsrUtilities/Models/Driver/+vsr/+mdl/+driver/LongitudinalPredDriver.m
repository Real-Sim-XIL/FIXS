classdef LongitudinalPredDriver < matlab.System  & matlab.system.mixin.SampleTime
    % LongitudinalPredDriver Longitudinal Predictive Driver
    %
    % This template includes the minimum set of functions required
    % to define a System object with discrete state.
    
    % Public, tunable properties
    properties
        VehMass = 100;  % Vehicle mass, VehMass [kg]
        
        aR = 200; % Rolling resistance coefficient, aR [N]
        bR = 2.5; % Rolling and driveline resistance coefficient, bR [N*s/m]
        cR = 0.5; % Aerodynamic drag coefficient, cR [N*s^2/m^2]
        
        g = 9.81; % Gravitational constant, g [m/s^2]
        
        MaxAccelFrcSpdBpts = [10, 100]; % Max acceleration force speed breakpoints, MaxAccelFrcSpdBpts [m/s]
        MaxAccelFrcTbl = [100, 100];    % Max acceleration force table, MaxAccelFrcTbl [N]
        
        AccelPdlBpts = [0,1]; % Accelerator pedal position breakpoints, AccelPdlBpts []
        AccelPdlTbl = [0, 1]; % Accelerator pedal position table, AccelPdlTbl []
        
        BrakePdlBpts = [0,1];   % Brake pedal position breakpoints, BrakePdlBpts []
        BrakeFrcTbl = [0,1e5]; % Brake force table, BrakeFrcTbl [N]
        
        VelThresh = .1;     % Velocity threshold for calculating losses, VelThresh [m/s]
        
        DistOptWeight = 1;       % Distance optimization weight, DistOptWeight []
        PdlRateLimitWeight = .1;    % Pedal rate limit weight, PdlRateLimitWeight []
        NumOptimIters = 30;       % Number of optimization iterations, NumOptimIters []
        
    end
    
    properties(DiscreteState)
        
    end
    
    % Pre-computed constants
    properties(Access = private)
        PrevAccelCmd
        PrevDecelCmd
        PrevTime
        PrevDist;
        DistWindup; % Distance windup used to offset the distance error if the throttle is wide-open and the speed cannot be met
    end
    
    methods(Access = protected)
        %% setupImpl
        function setupImpl(obj)
            % Perform one-time calculations, such as computing constants
            obj.PrevAccelCmd = 0;
            obj.PrevDecelCmd = 0;
            obj.PrevTime = 0;
            obj.PrevDist = 0;
            obj.DistWindup = 0;
        end
        
        %% getSampleTimeImpl
        function sts = getSampleTimeImpl(obj)
            sts = createSampleTime(obj,'Type','Fixed In Minor Step');
        end
        
        %% stepImpl
        function [AccelCmd, DecelCmd, DistWindupOut] = stepImpl(obj, VelLookAhead, VehDistLookAhead, TimeLookAhead, VehVel, VehDist, GradeLookAhead)
            % Implement algorithm. Calculate y as a function of input u and
            % discrete states.
            %% Setup
            %             NumPts = 100;
            CurrTime = getCurrentTime(obj);
            TimeStep = CurrTime - obj.PrevTime;
            
            %% Update windup 
            if obj.PrevDist > VehDist
                obj.DistWindup = 0;
            else
                %% Calculate windup
                if obj.PrevAccelCmd > 0.999
                    obj.DistWindup = obj.DistWindup - (VehDist - obj.PrevDist);
                end
                
            end
            DistWindupOut = obj.DistWindup;
            
            %% Get optimal pedal commands to match desired speed
            if TimeStep > 0 && VelLookAhead
                IterIdx = 1;
                % Golden section optimization
                xlower = -1;
                xupper = 1;
                phi = 1.6180;
                BestPdlCmd = 0;
                PrevPdlCmd = obj.PrevAccelCmd - obj.PrevDecelCmd;
                while IterIdx < obj.NumOptimIters
                    d = (phi-1)*(xupper - xlower);
                    x1 = xlower + d;
                    x2 = xupper - d;
                    f1 = obj.getJ(VelLookAhead, TimeLookAhead, VehVel, GradeLookAhead, TimeStep, PrevPdlCmd, VehDist, VehDistLookAhead, x1);
                    f2 = obj.getJ(VelLookAhead, TimeLookAhead, VehVel, GradeLookAhead, TimeStep, PrevPdlCmd, VehDist, VehDistLookAhead, x2);
                    
                    if f1 < f2
                        BestPdlCmd = x1;
                        xlower = x2;
                    else
                        BestPdlCmd = x2;
                        xupper = x1;
                    end
                    IterIdx = IterIdx + 1;
                end
                AccelCmd = max(0, BestPdlCmd);
                DecelCmd = -min(0, BestPdlCmd);
                
            else
                AccelCmd = obj.PrevAccelCmd;
                DecelCmd = obj.PrevDecelCmd;
                obj.DistWindup = 0;
            end
            
            
            %% Set states
            obj.PrevTime = CurrTime;
            obj.PrevAccelCmd = AccelCmd;
            obj.PrevDecelCmd = DecelCmd;
            obj.PrevDist = VehDist;
        end
        
        function resetImpl(obj)
            % Initialize / reset discrete-state properties
            
        end
        
    end
    
    methods (Access = private)
        
        %% getVehTimeStep
        function dVdt = getVehDeriv(obj, VehVel0, PdlCmd, GradeAng)
            % Get pedal command
            AccelPdl = max(0, PdlCmd);
            DecelPdl = -min(0, PdlCmd);
            
            % Losses
            Floss = -tanh(4*VehVel0/obj.VelThresh)*(obj.aR + obj.bR*abs(VehVel0) + obj.cR*VehVel0^2);
            
            % Accel force
            MaxFrc = obj.getInterp1(obj.MaxAccelFrcSpdBpts, obj.MaxAccelFrcTbl, VehVel0);
            Faccel = obj.getInterp1(obj.AccelPdlBpts, obj.AccelPdlTbl, AccelPdl)*MaxFrc;
            
            % Braking force
            Fdecel = -tanh(4*VehVel0/obj.VelThresh)*obj.getInterp1(obj.BrakePdlBpts, obj.BrakeFrcTbl, DecelPdl);
            
            % Acceleration
            dVdt = 1/obj.VehMass*(Faccel + Fdecel + Floss) - obj.g*sin(GradeAng);
            
        end
        
        %% getJ
        function J = getJ(obj, VelRef, LookAheadTime, VehVel, GradeLookAhead, TimeStep, PrevPdlCmd, VehDist, VehDistLookAhead, PdlCmd)
            % Calculate performance measure
            VelStep = obj.getVehDeriv(VehVel, PdlCmd, GradeLookAhead)*LookAheadTime + VehVel;
            NewVehDist = (VehVel + VelStep)*LookAheadTime/2 + VehDist;
            if abs(VelRef) < obj.VelThresh && PdlCmd > 0
                J = 1e6;
            else
                if PrevPdlCmd < 0 && VehDistLookAhead > VehVel && VehDistLookAhead > obj.VelThresh
                    RatePnlty = 0; 
                else
                    RatePnlty = obj.PdlRateLimitWeight*(PdlCmd - PrevPdlCmd)^2/TimeStep;
                end
                J = (VelStep - VelRef)^2 + obj.DistOptWeight*(NewVehDist - VehDistLookAhead - obj.DistWindup)^2 + RatePnlty;
            end
        end
        
        %% getInterp1
        function Vq = getInterp1(obj, x, y, xq)
            if xq > x(end) % Clip max
                Vq = y(end);
            elseif xq < x(1)
                Vq = y(1);
            else
                x0 = x(1);
                idx = 1;
                while xq < x0 % could update to improve speed
                    idx = idx + 1;
                    x0 = x(idx);
                end
                x1 = x(idx + 1);
                y0 = y(idx);
                y1 = y(idx + 1);
                Vq = y0 + (xq - x0)*(y1 - y0)/(x1 - x0);
                
            end
        end
    end
end
