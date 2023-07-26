classdef RealSimDepack < matlab.System & RealSimMsgClass & matlab.system.mixin.Propagates
    % RealSimDepack Depack bytes received from Traffic Simulator into Vehicle Data
    % 
    
    
    % Public, tunable properties
    properties

    end
    
    % Public, non-tunable properties
    properties(Nontunable)
       OutputBusName = 'VehDataBus';
    end
    
    properties(DiscreteState)
        
    end
    
    % Pre-computed constants
    properties(Access = private)
        simStatePrevious;
        tPrevious;
        nVehPrevious;
        speedPrevious;
        idPrevious;
        idLenPrevious;
        isVehicleInNetworkPrevious;
        VehicleDataPrevious
    end
    
    methods
        % Constructor
        function obj = RealSimDepack(varargin)
            % Support name-value pair arguments when constructing object
%             setProperties(obj,nargin,varargin{:})
%             coder.allowpcode('plain')

            obj@RealSimMsgClass();
        end
    end
    
    methods(Access = protected)
        %% Common functions
        function setupImpl(obj)
            % Perform one-time calculations, such as computing constants
            VehMsgDefVec2VehMsgDef(obj);
                  
            obj.simStatePrevious = 0;
            obj.tPrevious = 0;
            obj.VehicleDataPrevious = obj.VehicleDataEmpty;
            obj.isVehicleInNetworkPrevious = 0;
            
        end
        
        function [simState, t, nVeh, VehDataBus, isVehicleInNetwork] = stepImpl(obj, ByteData, nByte, connectionState)
            % INPUTS
            %   ByteData: bytes vector received from TrafficSimualtor
            %   nByte: size of bytes received
            %   connectionState: 
            %
            % OUTPUTS
            %   simState: simulation state. NOT actively used
            %   t: simulation time of TrafficSimulator
            %   nVeh: number of vehicle data received.
            %   VehDataBus: a vehicle data bus signal
            %   isVehicleInNetwork: indicator whether ego is in network
            
% %             try
                % defualt values for outputs
                simState = obj.simStatePrevious;
                t = obj.tPrevious;
                nVeh = 0;
                
                % init values for other variables
                totalMsgSize = 0; % this is the total message size received from Real-Sim
                msgProcessed = 0; % to record how many messages have been processed
                
                VehDataBus = obj.VehicleDataPrevious;
    
                %---------------------------
                % read received buffer
                %---------------------------
                % need to check size, before connection, there could be no
                % message at all             
                if nByte >= obj.msgHeaderSize
                    simState = double(typecast(uint8(ByteData(1)),'uint8'));
                    t = double(typecast(uint8(ByteData(2:5)),'single'));
                    totalMsgSize = double(typecast(uint8(ByteData(6:9)),'uint32'));
                    msgProcessed = obj.msgHeaderSize;
                    
                    obj.simStatePrevious = simState;
                    obj.tPrevious = t;
                end
                
                % read actual message fields
                while round(msgProcessed) < round(totalMsgSize)
                    iMsgSizeRecv = double(typecast(uint8(ByteData(msgProcessed+1:msgProcessed+2)),'uint16'));
                    iMsgTypeRecv = double(typecast(uint8(ByteData(msgProcessed+3)),'uint8'));

                    % !!! for now, Simulink only supports vehicle data type
                    if obj.enableDebug
                        obj.vehMsgIdentifer = iMsgTypeRecv;
                    else
                        obj.vehMsgIdentifer = 1;
                    end
                    
                    if iMsgTypeRecv == obj.vehMsgIdentifer
                        VehDataBus = obj.depackVehData(VehDataBus, ByteData(msgProcessed+obj.msgEachHeaderSize+1:msgProcessed+iMsgSizeRecv));
                           
                        nVeh = nVeh + 1;

                        obj.VehicleDataPrevious = VehDataBus;
                        obj.isVehicleInNetworkPrevious = 1;
                    else
                        % not supported yet!!!
                        
                    end
                    
                    msgProcessed = msgProcessed + iMsgSizeRecv;
 
                end
                

                if abs(connectionState-3)>1e-5
                    % if not connected, then set to 0
                    obj.simStatePrevious = 0;
                    obj.tPrevious = 0;
                    obj.nVehPrevious = 0;
                    obj.VehicleDataPrevious = obj.VehicleDataEmpty;
                    obj.isVehicleInNetworkPrevious = 0;
                else
                    if totalMsgSize == 9
                        obj.isVehicleInNetworkPrevious = 0;
                    end
                end
                isVehicleInNetwork = obj.isVehicleInNetworkPrevious;

% %             catch
% %                 printf('ERROR: RealSimDepack');
% %                 return
% %             end
            
        end
        
        function resetImpl(obj)
            % Initialize / reset discrete-state properties

        end

        function validateInputsImpl(obj, ByteData, nByte, connectionState)
            % Validate inputs to the step method at initialization
            if strcmp(class(ByteData), 'uint8')
                
            end
        
        end

        function [sz1, sz2, sz3, sz4, sz5] = getOutputSizeImpl(obj)
            % Maximum length of linear indices and element vector is the
            % number of elements in the input
            sz1 = [1 1]; sz2 = [1 1]; sz3 = [1 1]; sz4 = [1 1]; sz5 = [1 1];
        end
        
        function [fz1, fz2, fz3, fz4, fz5] = isOutputFixedSizeImpl(~)
            %Both outputs are always fixed
            fz1 = 1; fz2 = 1; fz3 = 1; fz4 = 1; fz5 = 1;
        end
        
        function [dt1, dt2, dt3, dt4, dt5] = getOutputDataTypeImpl(obj)
            dt1 = 'double'; dt2 = 'double'; dt3 = 'double'; dt4 = obj.OutputBusName; dt5 = 'double'; 
        end
        
        function [cp1, cp2, cp3, cp4, cp5] = isOutputComplexImpl(obj)
            cp1 = false; cp2 = false; cp3 = false; cp4 = false; cp5 = false; 
        end
        
    end
    
    
    
    methods(Access = private)
        

        
    %%% END OF PRIVATE METHODS    
    end 
    
   
%%% END OF CLASS
end

