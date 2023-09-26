classdef RealSimPack < matlab.System & RealSimMsgClass & matlab.system.mixin.Propagates
    % RealSimPack Pack vehicle data into bytes and transmit to Traffic Simulator

    
    % Public, tunable properties
    properties
%         VehicleId= ''; % id of vehicle controlled, [string] without quote
%         VehicleType = ''; % vehicle type, NOT currently used, can be filled with any value, [string] without quote
%         LinkId = ''; % link id, NOT currently used, can be filled with any value, [string]
%         LaneId = 0; % lane id, NOT current used, can be filled with any value, [int]
    
        % string will eventually be treated as non-tunable by Simulink
     
    end
    
    % Public, non-tunable properties
    properties(Nontunable)
        
    end
    
    properties(DiscreteState)
        
    end
    
    % Pre-computed constants
    properties(Access = private)
        
    end
    
    methods
        % Constructor
        function obj = RealSimPack(varargin)
            % Support name-value pair arguments when constructing object
%             setProperties(obj,nargin,varargin{:})
%             coder.allowpcode('plain')

            obj@RealSimMsgClass();
            
            setProperties(obj,nargin,varargin{:})
        end
    end
    
    methods(Access = protected)        
        
        %% Common functions
        function setupImpl(obj)
            % Perform one-time calculations, such as computing constants
            
            % convert vector message field definition to struct 
            VehMsgDefVec2VehMsgDef(obj);
            
        end
        
        function [ByteData, nMsgSize] = stepImpl(obj, simState, t, VehDataBus)
            % INPUTS
            %   simState: always set to 1. NOT actively used.
            %   t: simulation time
            %   speedDesired: desired speed in m/s
            %   accelerationDesired: desired acceleration in m/s^2
            %
            %   NOTE: if select both speedDesired and accelerationDesired
            %   in config.yaml, then only accelerationDesired will be used
            %   to control vehicles in TrafficSimulator. if only one of
            %   speedDesired and accelerationDesired selected in
            %   config.yaml, then only the one selected will be used for
            %   control, the other input can be leave as 0.
            %
            % OUTPUTS
            %   ByteData: bytes vector that will be sent to TrafficSimualtor
            %   nMsgSize: size of bytes to be sent
            
% %             try

                % initialize ByteData
                ByteData = zeros(200, 1, 'uint8');
                
                % parser ByteData
                [ByteData, nMsgSize] = obj.packVehData(simState, t, ByteData, VehDataBus);
                
% %             catch
% %                 printf('ERROR: RealSimPack');
% %                 return
% %             end
            
        end
        
        function resetImpl(obj)
            % Initialize / reset discrete-state properties

        end

        function validateInputsImpl(obj, simState, t, VehData)
            % Validate inputs to the step method at initialization

        
        end

        function [sz1, sz2] = getOutputSizeImpl(obj)
            % Maximum length of linear indices and element vector is the
            % number of elements in the input
            sz1 = [200 1]; sz2 = [1 1]; 
        end
        
        function [fz1, fz2] = isOutputFixedSizeImpl(~)
            %Both outputs are always fixed
            fz1 = 1; fz2 = 1;      
        end
        
        function [dt1, dt2] = getOutputDataTypeImpl(obj)
            dt1 = 'uint8'; %Linear indices are always double values
            dt2 = 'double';   
        end
        
        function [cp1, cp2] = isOutputComplexImpl(obj)
            cp1 = false; %Linear indices are always real values
            cp2 = false;
        end
        
    end
    
    
    
    methods(Access = private)
        
        
        
    %%% END OF PRIVATE METHODS    
    end 
    
   
%%% END OF CLASS
end
