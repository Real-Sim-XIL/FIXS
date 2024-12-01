classdef MappedEngConfig < vsr.mdl.ComponentDatasetConfig
    % MappedEngConfig Use to manipulate data for a steady-state engine
    % model. 
    %% Engine specific properties
    properties
        % Base properties
        NumCyl = 4;           % Number of cylinders
        VolDisp = 0.002;      % Displaced volume [m^3]
        
        % Dynamics
        EngInertia          % Engine rotating intertia
        
        % Breakpoints
        SpdBpts % used for 1D and 2D tables [rpm]
        TrqBpts % used for 2D tables [Nm]
        
        % 1D torque lines
        MaxTrqLine % Maximum torue as a function of speed
        MinTrqLine % Motoring torque as a function of speed
        
        % 2D tables with engine torque (Bpt1) and speed breakpoints (Bpt2)
        FuelFlwTbl   % engine fuel flow [kg/s]
        
        AirFlwTbl = [];
        ExhTempTbl = [];
        
        CO2FlwTbl = [];    % CO2 emissions flow [kg/s]
        HCFlwTbl = [];
        COFlwTbl = [];
        NOxFlwTbl = [];
        PMFlwTbl = [];
        
%         TurboTimeCnstTbl % Time constant for modeling turbo lag
        
    end
    
    properties (Hidden)
        CrkRevPerStrk = 2;    % Crank revoltions per power stroke
    end
    %% Dependent engine properties
    properties (Dependent)
        BsfcTbl  % Brake specific fuel consumption table with  [g/Kwh]
        TrqTbl   % Engine brake torque [Nm]
        PwrTbl   % Power table with torque and speed breakpoints [W]
        MaxPwr   % Max power value [W]
    end
    
    %% Protected properties
    properties (Access = protected, Hidden)
        
        OptionalTbls = {'AirFlwTbl', 'ExhTempTbl', 'CO2FlwTbl', 'HCFlwTbl', 'COFlwTbl', 'NOxFlwTbl', 'PMFlwTbl'};
    end
    %% Public methods
    methods
        
        %% Constructor
        function obj = MappedEngConfig
            % Map parameter values to class properties
            obj.mapParam('NumCyl', 'EngNumCyl')
            obj.mapParam('CrkRevPerStrk', 'EngCrkRevPerStrk')
            obj.mapParam('VolDisp', 'EngVolDisp')
            
            obj.mapParam('TrqBpts', 'EngTrqBpts');
            obj.mapParam('SpdBpts', 'EngSpdBpts');
            
            obj.mapParam('TrqTbl', 'EngTrqTbl');
            obj.mapParam('AirFlwTbl', 'EngAirFlwTbl');
            obj.mapParam('FuelFlwTbl', 'EngFuelFlwTbl');
            obj.mapParam('ExhTempTbl', 'EngExhTempTbl');
            obj.mapParam('BsfcTbl', 'EngBsfcTbl');
            obj.mapParam('HCFlwTbl', 'EngHCFlwTbl');
            obj.mapParam('COFlwTbl', 'EngCOFlwTbl');
            obj.mapParam('NOxFlwTbl', 'EngNOxFlwTbl');
            obj.mapParam('CO2FlwTbl', 'EngCO2FlwTbl');
            obj.mapParam('PMFlwTbl', 'EngPMFlwTbl');
            
        end
        
        %% verifyParams
        function verifyParams(obj)
            % Fill optional tables with zeros
            TblSize = size(obj.TrqTbl);
            for i = 1:length(obj.OptionalTbls)
                if isempty(obj.(obj.OptionalTbls{i}))
                    obj.(obj.OptionalTbls{i}) = zeros(TblSize);
                end
            end
            
            % Engine inertia
            if isempty(obj.EngInertia)
                obj.EngInertia = 0;
            end
            
            % Exhaust temperature
            
        end
        %% get.TrqTbl
        function Tbl = get.TrqTbl(obj)
            Tbl = repmat(obj.TrqBpts(:), 1, length(obj.SpdBpts));
            for i = 1:length(obj.SpdBpts)
                Tbl(Tbl(:,i) == 0,i) =  obj.MinTrqLine(i);
                Tbl(Tbl(:,i) > obj.MaxTrqLine(i),i) =  obj.MaxTrqLine(i);
            end
        end
        
        %% pullFromDictionary
        function pullFromDictionary(obj)
            pullFromDictionary@vsr.mdl.ComponentDatasetConfig(obj)
            DdTrqTbl = obj.findParam('EngTrqTbl');
            obj.MinTrqLine =  min(DdTrqTbl.Value);
            obj.MaxTrqLine =  max(DdTrqTbl.Value);
        end
        
        %% get.BsfcTbl
        function Tbl = get.BsfcTbl(obj)
            Pwr = obj.PwrTbl/1000;
            Pwr(Pwr < 1e-6) = 1e-6;
            Tbl = obj.FuelFlwTbl./Pwr*1000*3600; % BSFC [g/Kwh]
            Tbl(Tbl > 1500) = 1500;
        end
        
        %% get.PwrTbl
        function Tbl = get.PwrTbl(obj)
            Tbl = obj.TrqTbl.*repmat(obj.SpdBpts(:)', length(obj.TrqBpts), 1)*2*pi/60; % power table [W]
        end
        
        %% get.MaxPwr
        function Val = get.MaxPwr(obj)
            Val = max(obj.PwrTbl(:));
        end
        
        %% getParamsFromAutonomieInitFile
        function getParamsFromAutonomieInitFile(obj, InitFileName)
            %% Run init file
            eval(InitFileName);
            InitData = eng.plant.init;
            
            %% General information
            obj.NumCyl = InitData.num_cyl_init;
            obj.EngInertia = InitData.inertia;
            obj.VolDisp = InitData.displ_init * 1e-6; % cc to m^3
            
            %% Fuel table and breakpoints
            FuelTbl = InitData.fuel_hot;
            obj.SpdBpts = [0, FuelTbl.idx1_spd*30/pi]; % [rpm]
            obj.TrqBpts = FuelTbl.idx2_trq;
            obj.FuelFlwTbl = [zeros(length(obj.TrqBpts), 1), FuelTbl.map'];
            
            % Motoring fuel flow
            obj.FuelFlwTbl(:,1) = 0;
            obj.FuelFlwTbl(1,:) = 0;
            
            % Breakpoint grid
            % Rows: SpdBpts Cols: TrqBpts
            SpdTbl = repmat(obj.SpdBpts(:)', length(obj.TrqBpts), 1);
            TrqBptTbl = repmat(obj.TrqBpts(:), 1, length(obj.SpdBpts));
            
            %% Torque
            obj.MaxTrqLine = [InitData.trq_max_hot.map(1), interp1(InitData.trq_max_hot.idx1_spd*30/pi, InitData.trq_max_hot.map, obj.SpdBpts, 'linear', 'extrap')];
            obj.MinTrqLine = interp1(InitData.trq_min_hot.idx1_spd*30/pi, InitData.trq_min_hot.map, obj.SpdBpts, 'linear', 'extrap');
            
            %% Emissions
            % Ignore for now
            
            %% Exhaust temperature
            % Assume Autonomie in C and output in K
            % Update later
            ExhTblStruct = eng.plant.init.ex_temp;
            if all(isfinite(ExhTblStruct.map))
                obj.ExhTempTbl = interp2(ExhTblStruct.idx2_trq, ExhTblStruct.idx1_spd*30/pi, ExhTblStruct.map, TrqBptTbl, SpdTbl) + 273.15;
                obj.ExhTempTbl(SpdTbl == 0) = 293.15;
            else
                obj.ExhTempTbl = 293.15*ones(size(obj.FuelFlwTbl));
            end
            

        end
        
    end
    
end