% initialization of vehicle data subscription setup before runnning any
% simulink files. A config.yaml needs to present to read the vehicle data
% subscription setup.

% clear
function [VehicleMessageFieldDefInputVec, VehDataBus, TrafficLayerIP, TrafficLayerPort, Status] = RealSimInitSimulink(configFilename)
    %% initialize outputs
    Status = 0;
	VehicleMessageFieldDefInputVec = zeros(1, 29);
    VehDataBus= Simulink.Bus;
    VehDataBus.Elements = [];
    TrafficLayerIP = '';
    TrafficLayerPort = '';
    CarMakerPort = '';
    
    %% Read config
	% read configuration file
	Config = ReadYaml(configFilename);          

	% create vehicle data subscription vector
	% currently support 29 data identifier
	%[id, type, vehicleClass, speed, acceleration, positionX, positionY, positionZ, heading,
	%color, linkId, laneId, distanceTravel, speedDesired, accelerationDesired,
	%hasPrecedingVehicle, precedingVehicleId, precedingVehicleDistance,
	%precedingVehicleSpeed, signalLightId, signalLightHeadId, signalLightDistance, signalLightColor, speedLimit,
	%speedLimitNext, speedLimitChangeDistance, linkIdNext, grade, activeLaneChange]

    if ~isfield(Config.SimulationSetup, 'VehicleMessageField')
        VehicleMessageFieldDefInputVec = ones(1, 29);
    else
        for i = 1:numel(Config.SimulationSetup.VehicleMessageField)
            %     curMsgId = ;
            switch Config.SimulationSetup.VehicleMessageField{i}
                case 'id'
                    VehicleMessageFieldDefInputVec(1) = 1;
                case 'type'
                    VehicleMessageFieldDefInputVec(2) = 1;
                case 'vehicleClass'
                    VehicleMessageFieldDefInputVec(3) = 1;
                case 'speed'
                    VehicleMessageFieldDefInputVec(4) = 1;
                case 'acceleration'
                    VehicleMessageFieldDefInputVec(5) = 1;
                case 'positionX'
                    VehicleMessageFieldDefInputVec(6) = 1;
                case 'positionY'
                    VehicleMessageFieldDefInputVec(7) = 1;
                case 'positionZ'
                    VehicleMessageFieldDefInputVec(8) = 1;
                case 'heading'
                    VehicleMessageFieldDefInputVec(9) = 1;
                case 'color'
                    VehicleMessageFieldDefInputVec(10) = 1;
                case 'linkId'
                    VehicleMessageFieldDefInputVec(11) = 1;
                case 'laneId'
                    VehicleMessageFieldDefInputVec(12) = 1;
                case 'distanceTravel'
                    VehicleMessageFieldDefInputVec(13) = 1;
                case 'speedDesired'
                    VehicleMessageFieldDefInputVec(14) = 1;
                case 'accelerationDesired'
                    VehicleMessageFieldDefInputVec(15) = 1;
                case 'hasPrecedingVehicle'
                    VehicleMessageFieldDefInputVec(16) = 1;
                case 'precedingVehicleId'
                    VehicleMessageFieldDefInputVec(17) = 1;
                case 'precedingVehicleDistance'
                    VehicleMessageFieldDefInputVec(18) = 1;
                case 'precedingVehicleSpeed'
                    VehicleMessageFieldDefInputVec(19) = 1;
                case 'signalLightId'
                    VehicleMessageFieldDefInputVec(20) = 1;
                case 'signalLightHeadId'
                    VehicleMessageFieldDefInputVec(21) = 1;
                case 'signalLightDistance'
                    VehicleMessageFieldDefInputVec(22) = 1;
                case 'signalLightColor'
                    VehicleMessageFieldDefInputVec(23) = 1;
                case 'speedLimit'
                    VehicleMessageFieldDefInputVec(24) = 1;
                case 'speedLimitNext'
                    VehicleMessageFieldDefInputVec(25) = 1;
                case 'speedLimitChangeDistance'
                    VehicleMessageFieldDefInputVec(26) = 1;
                case 'linkIdNext'
                    VehicleMessageFieldDefInputVec(27) = 1;
                case 'grade'
                    VehicleMessageFieldDefInputVec(28) = 1;
                case 'activeLaneChange'
                    VehicleMessageFieldDefInputVec(29) = 1;
                otherwise
                    fprintf('\nERROR! Vehicle message id not supported! Check configuration yaml file!\n')
            end    
        end
    end
    
	% if use bitwise , this is the integer corresponding to the setup. NOT CURRENTLY used
	VehicleMessageFieldDefInputBitwise = uint32(bin2dec(sprintf('%d',VehicleMessageFieldDefInputVec)));
 

    %% parser simulink ip and port 
    
    % sanity check to make sure field XilSetup and ApplicationSetup exist
    if ~isfield(Config, 'XilSetup')
        Config.XilSetup.EnableXil = 0;
    end
    if ~isfield(Config, 'ApplicationSetup')
        Config.ApplicationSetup.EnableApplicationLayer = 0;
    end
    if ~isfield(Config.XilSetup, 'EnableXil')
        Config.XilSetup.EnableXil = 0;
    end
    if ~isfield(Config.ApplicationSetup, 'EnableApplicationLayer')
        Config.ApplicationSetup.EnableApplicationLayer = 0;
    end
    
    % get input arguments to the s-function
    if Config.XilSetup.EnableXil
        if length(Config.XilSetup.VehicleSubscription) <= 0
            fprintf('\nERROR! must specify one vehicle subscription for XilSetup!\n')
            Status = -1;
            return;
        elseif length(Config.XilSetup.VehicleSubscription) == 1
            TrafficLayerIP = Config.XilSetup.VehicleSubscription{1,1}.ip{1};
            TrafficLayerPort = Config.XilSetup.VehicleSubscription{1,1}.port{1};
            EgoId = Config.XilSetup.VehicleSubscription{1,1}.attribute.id{1};
            CarMakerPort = TrafficLayerPort;
        elseif length(Config.XilSetup.VehicleSubscription) == 2
            if ~isfield(Config, 'CarMakerSetup')
                fprintf('\nERROR! if two vehicle subscription are defined, CarMakerSetup must be specified and EnableEgoSimulink must be true!\n')
                Status = -1;
                return;
            end
            
            if ~isfield(Config.CarMakerSetup, 'EnableEgoSimulink')
                Config.CarMakerSetup.EnableEgoSimulink = 0;
            end
            %             if ~(Config.CarMakerSetup.EnableEgoSimulink)
            %                 fprintf('\nERROR! if two vehicle subscription are defined, CarMakerSetup must be specified and EnableEgoSimulink must be true!\n')
            %                 Status = -1;
            %                 return;
            %             end
            
            if isfield(Config.CarMakerSetup, 'CarMakerIP') && isfield(Config.CarMakerSetup, 'CarMakerPort')
                for i = 1:numel(Config.XilSetup.VehicleSubscription)
                    if ~(strcmpi(Config.XilSetup.VehicleSubscription{1,i}.ip{1}, Config.CarMakerSetup.CarMakerIP) && ...
                            Config.XilSetup.VehicleSubscription{1,i}.port{1} == Config.CarMakerSetup.CarMakerPort)
                        TrafficLayerIP = Config.XilSetup.VehicleSubscription{1,i}.ip{1};
                        TrafficLayerPort = Config.XilSetup.VehicleSubscription{1,i}.port{1};
                    end
                    if Config.XilSetup.VehicleSubscription{1,i}.port{1} == Config.CarMakerSetup.CarMakerPort
                        CarMakerPort = Config.XilSetup.VehicleSubscription{1,i}.port{1};
                        EgoId = Config.XilSetup.VehicleSubscription{1,i}.attribute.id{1};
                    end
                end
            else
                fprintf('\nERROR! must specify CarMaker IP and Port when 2 vehicle subscriptions are defined!\n')
                Status = -1;
                return;
            end
            
        else
            fprintf('\nERROR! currently only support at one vehicle subscription for Simulink, and at most two for CarMaker Simulink!\n')
            Status = -1;
            return;
        end
    elseif Config.ApplicationSetup.EnableApplicationLayer && ~Config.XilSetup.EnableXil
        if length(Config.ApplicationSetup.VehicleSubscription) <= 0
            fprintf('\nERROR! must specify one vehicle subscription for ApplicationSetup!\n')
        elseif length(Config.XilSetup.VehicleSubscription) > 1
            fprintf('\nERROR! currently only support one vehicle subscription for ApplicationSetup for Simulink!\n')
        else
            TrafficLayerIP = Config.ApplicationSetup.VehicleSubscription{1,1}.ip{1};
            TrafficLayerPort = Config.ApplicationSetup.VehicleSubscription{1,1}.port{1};
        end
    else
        error('Real-Sim: must at least enable Application layer or Xil layer!')
        Status = -1;
        return;
    end
    
    %% write to a CarMaker text file 
    if isfield(Config, 'CarMakerSetup')              
        % if CarMaker, then write a plain text file with configurations
        % so that CarMaker scripts can be compiled on dSPACE without
        % need of special YAML parser libraries

        % handle default values if not defined in config.yaml
        if ~isfield(Config.CarMakerSetup, 'EnableCosimulation'); Config.CarMakerSetup.EnableCosimulation = 0; end
        if ~isfield(Config.CarMakerSetup, 'EnableEgoSimulink'); Config.CarMakerSetup.EnableEgoSimulink = 1; end
        if ~isfield(Config.CarMakerSetup, 'EgoId'); Config.CarMakerSetup.EgoId = EgoId; end
        if ~isfield(Config.CarMakerSetup, 'EgoType'); Config.CarMakerSetup.EgoType = ''; end
        if ~isfield(Config.CarMakerSetup, 'TrafficLayerIP'); Config.CarMakerSetup.TrafficLayerIP = TrafficLayerIP; end
        if ~isfield(Config.CarMakerSetup, 'CarMakerPort'); Config.CarMakerSetup.CarMakerPort = CarMakerPort; end
        if ~isfield(Config.CarMakerSetup, 'TrafficRefreshRate'); Config.CarMakerSetup.TrafficRefreshRate = 0.001; end
        
        % save to text file
        fid = fopen('RealSimCarMakerConfig.txt', 'w');
        VehMsgStrTmp = Config.SimulationSetup.VehicleMessageField{1};
        for i = 2:numel(Config.SimulationSetup.VehicleMessageField)
            VehMsgStrTmp = [VehMsgStrTmp,',',Config.SimulationSetup.VehicleMessageField{i}];
        end
        fprintf(fid, 'VehicleMessageField=%s\n', VehMsgStrTmp);
        fprintf(fid, 'EnableCosimulation=%d\n', Config.CarMakerSetup.EnableCosimulation);
        fprintf(fid, 'EnableEgoSimulink=%d\n', Config.CarMakerSetup.EnableEgoSimulink);
        fprintf(fid, 'EgoId=%s\n', Config.CarMakerSetup.EgoId);
        fprintf(fid, 'EgoType=%s\n', Config.CarMakerSetup.EgoType);
        fprintf(fid, 'TrafficLayerIP=%s\n', Config.CarMakerSetup.TrafficLayerIP);
        fprintf(fid, 'CarMakerPort=%d\n', Config.CarMakerSetup.CarMakerPort);
        fprintf(fid, 'TrafficRefreshRate=%f\n', Config.CarMakerSetup.TrafficRefreshRate);
        fclose(fid);
    end     
    
    %% create Simulink Bus
    temp = struct('id', uint8(zeros(50,1)), 'idLength', 0, 'type', uint8(zeros(50,1)), 'typeLength', 0, ...
                    'vehicleClass', uint8(zeros(50,1)), 'vehicleClassLength', 0, ...
                    'speed', 0, 'acceleration', 0, 'positionX', 0, 'positionY', 0, 'positionZ', 0, ...
                    'heading', 0, 'color', 0, 'linkId', uint8(zeros(50,1)), 'linkIdLength', 0, 'laneId', 0, ...
                    'distanceTravel', 0, 'speedDesired', 0, 'accelerationDesired', 0, ...
                    'hasPrecedingVehicle', 0, 'precedingVehicleId', uint8(zeros(50,1)), 'precedingVehicleIdLength', 0, 'precedingVehicleDistance', 0, 'precedingVehicleSpeed', 0, ...
                    'signalLightId', uint8(zeros(50,1)), 'signalLightIdLength', 0, 'signalLightHeadId', 0, 'signalLightDistance', 0, 'signalLightColor', 0, ...
                    'speedLimit', 0, 'speedLimitNext', 0, 'speedLimitChangeDistance', 0, ...
                    'linkIdNext', uint8(zeros(50,1)), 'linkIdNextLength', 0, 'grade', 0, 'activeLaneChange', 0);
    tempnames = fieldnames(temp);

    for i = 1:numel(tempnames)
        ele=Simulink.BusElement;
        ele.Name = tempnames{i};
        ele.Dimensions = max(numel(temp.(ele.Name)),1);
        ele.DataType = class(temp.(ele.Name));
        VehDataBus.Elements = [VehDataBus.Elements; ele];
    end
    
    %% clean up
    


 
    
    