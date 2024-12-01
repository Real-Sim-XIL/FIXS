%% File description
% Name : eng_plant_si_6000_226						                          				
% Description : The model year 2001 GM 6.0L V-8 engine map was generated in 2015 by SwRI under funding from the US DOT NHTSA
%Proprietary : public
%Protected: false


% Model : eng_plant_hot_map_si													
% Technology : si																		
% Vehicle Type : Light

%% Conversion constants
pxfUnitConversion.rpm2radps = pi/30;
pxfUnitConversion.gallon2liter = 3.78541;

%% File content
eng.plant.init.technology                   = 'si';
eng.plant.init.type                         = 'Baseline 6.0L';
eng.plant.init.cyl_config                   = 'V engine';
eng.plant.init.cam_shaft_config             = 'sohc'; %Assumption
eng.plant.init.num_cyl_init                 = 8;
eng.plant.init.num_cyl_banks                = 2;
eng.plant.init.has_variable_valve_timing    = true;
eng.plant.init.has_variable_valve_lift      = false;
eng.plant.init.valvetrain                   = 'VVT'; %Assumption
eng.plant.init.has_direct_injec             = false;
eng.plant.init.injection_type               = 'PFI'; %PFI or DI
eng.plant.init.has_turbo                    = false;
eng.plant.init.boost_level                  = 0;
eng.plant.init.has_EGR                      = false;
eng.plant.init.EGR_type                     = 'No EGR'; %Assumption
eng.plant.init.has_cyl_deac                 = false;
eng.plant.init.material                     = 'Aluminum'; %Assumption
eng.plant.init.has_hcci                     = false;
eng.plant.init.has_fmep_imprvmt             = false;
eng.plant.init.fmep_imprvmt                 = '0%';

eng.plant.init.fuel_density_val           = 0.741;          % kg/L
eng.plant.init.fuel_heating_val         = 43051000;    % (J/kg)Specific LHV
eng.plant.init.fuel_carbon_ratio          = 0.8696;      % (kg/kg) ref:Dr. Rob Thring

eng.plant.init.mass.eng                   = 140;%estimated eng.plant.init.mass_block + eng.plant.init.mass_radiator + eng.plant.init.mass_vol + eng.plant.init.tank_mass;
eng.plant.init.mass.tank                  = 20;
eng.plant.init.mass.fuel                  = (30.*pxfUnitConversion.gallon2liter)*eng.plant.init.fuel_density_val;%Capacity of tank in kg - approximate value
				
eng.plant.init.time_response              = 0.3; 						
eng.plant.init.spd_idle		 			= 80;
eng.plant.init.spd_idle_offset		 	= 0;  % this can be used to implement a higher idle speed for performance tests
eng.plant.init.warmup_init				= 0;							% This should normally by 0
eng.plant.init.pwr_max				    = 226000;					% Watts
eng.plant.init.ex_gas_heat_cap            = 1089;            % J/kgK  ave sens heat cap of exh gas (SAE #890798)

eng.plant.init.displ_init					= 6000;		% cc
eng.plant.init.inertia					= 0.35	;		% kg-m^2 - approximate value
eng.plant.init.spd_str					= 10; % speed level (rad/s) the engine crank has to reach in order to start 

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% maximum curves at each speed (closed and wide open throttle)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% hot max wide open throttle curves
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
eng.plant.init.trq_max_hot.idx1_spd= pi/30.*[0 584	999	1500	2000	2499	2999	3499	3998	4498	4998	5498];
eng.plant.init.trq_max_hot.map     =   [0 326 358 410	448	444	476	486	482	436	432	335];        

% Mid speed is used in logic to limit closed and wide open torque curves
eng.plant.init.spd_avg	= 0.5 * (eng.plant.init.trq_max_hot.idx1_spd(1) + eng.plant.init.trq_max_hot.idx1_spd(length(eng.plant.init.trq_max_hot.idx1_spd)));
      
% % hot max closed throttle curves
% %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% eng.plant.init.trq_min_hot.idx1_spd 	= eng.plant.init.trq_max_hot.idx1_spd;
% eng.plant.init.trq_min_hot.map = [0 -5*ones(1,size(eng.plant.init.trq_max_hot.idx1_spd,2)-1)];
%  
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% consumption table
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

eng.plant.init.fuel_hot.idx1_spd	= 3.14/30.*[500	816	1132	1448	1764	2080	2396	2712	3028	3344	3660	3976	4292	4608	4924	5240	5556];
eng.plant.init.fuel_hot.idx2_trq    = [-129 -97 -65 -32 0 36 72 108 143 179 215 250 286 322 358 393 429 465 500]; % (Eqn B)

% Rows represent speed (rad/s).  Columns represent torque (N-m).  Table is fuel rate (kg/s)
eng.plant.init.fuel_hot.map = [...
0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0.44;...
0	0	0	0	0	0	0	0	0	0	0.09	0.34	0.71	1.07	1.49	2.00	3.19;...
0	0	0	0.08	0.25	0.44	0.69	0.97	1.28	1.68	2.12	2.59	3.13	3.73	4.45	5.23	6.67;...
0.29	0.66	0.89	1.16	1.46	1.82	2.24	2.71	3.21	3.81	4.45	5.12	5.86	6.65	7.53	8.46	9.87;...
1.01	1.47	1.89	2.28	2.76	3.33	4.00	4.67	5.34	6.11	6.96	7.86	8.80	9.75	10.74	11.75	13.03;...
1.38	2.14	2.86	3.49	4.22	5.09	6.11	6.96	7.80	8.76	9.87	11.07	12.25	13.39	14.47	15.72	17.40;...
1.75	2.64	3.61	4.58	5.58	6.69	7.90	9.01	10.10	11.34	12.75	14.27	15.68	17.04	18.39	19.84	21.72;...
2.22	3.22	4.41	5.64	6.89	8.20	9.55	10.92	12.33	13.87	15.58	17.42	19.09	20.73	22.45	24.24	26.38;...
2.73	3.87	5.25	6.72	8.20	9.73	11.30	12.91	14.59	16.44	18.41	20.47	22.53	24.50	26.53	29.02	31.92;...
3.20	4.53	6.16	7.85	9.55	11.29	13.11	14.93	16.85	19.02	21.30	23.65	26.23	28.60	30.76	34.21	37.88;...
3.80	5.21	7.03	8.95	10.90	12.89	14.93	16.99	19.18	21.63	24.26	27.13	30.31	33.20	36.07	40.51	45.10;...
4.43	5.95	7.98	10.12	12.31	14.55	16.82	19.09	21.52	24.27	27.24	30.63	34.79	38.30	41.31	47.48	53.61;...
5.07	6.87	9.27	11.52	13.87	16.32	18.79	21.21	23.80	26.92	30.19	33.69	39.29	43.84	45.43	54.43	63.10;...
6.09	8.46	11.17	13.30	15.57	18.10	20.74	23.36	26.16	29.46	33.12	37.31	42.52	47.69	52.92	61.09	68.16;...
8.99	11.28	13.73	15.29	17.49	20.12	23.06	25.83	28.64	32.29	36.37	40.79	45.82	51.52	58.68	65.72	70.77;...
12.00	13.97	16.08	17.87	19.96	22.68	26.15	29.21	32.08	36.17	40.81	45.83	51.79	57.57	64.08	68.98	72.10;...
14.75	16.57	18.64	20.81	22.74	25.60	30.10	33.48	36.66	40.97	46.29	52.62	60.37	66.96	69.32	71.80	73.54;...
17.37	19.17	21.21	23.37	25.61	28.69	33.32	37.05	40.59	45.36	51.12	57.80	64.51	69.60	71.89	73.50	74.59;...
19.90	21.73	23.81	26.11	28.72	31.99	35.93	39.87	43.94	48.83	54.46	60.53	65.84	69.95	72.53	74.14	75.11;...

]./3600;

eng.plant.init.fuel_hot.map = eng.plant.init.fuel_hot.map';


% locate the minimum torque with fuel based on Willans lines


eng.plant.init.trq_min_hot.idx1_spd = 3.14/30.*[600 1000 1500 2000 2500 3000 3500 4000 4500 5000 5500 6000 6500];
eng.plant.init.trq_min_hot.map      = 0.5*[-50.44 -55.5 -62.23 -69.55 -77.01 -84.06 -91.79 -99.52 -106.97 -114.16 -121.48 -129.34 -137.89];

eng.plant.init.trq_zero_fuel_hot.idx1_spd       = eng.plant.init.trq_max_hot.idx1_spd;
eng.plant.init.trq_zero_fuel_hot.map            = zeros(1,length(eng.plant.init.trq_max_hot.idx1_spd));

%Emissions in percentage of fuel rate (kg/s)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
eng.plant.init.co_hot.idx1_spd = eng.plant.init.fuel_hot.idx1_spd;
eng.plant.init.co_hot.idx2_trq = eng.plant.init.fuel_hot.idx2_trq;
eng.plant.init.co_hot.map       = zeros(length(eng.plant.init.co_hot.idx1_spd),length(eng.plant.init.co_hot.idx2_trq)); 

eng.plant.init.hc_hot.idx1_spd = eng.plant.init.fuel_hot.idx1_spd;
eng.plant.init.hc_hot.idx2_trq = eng.plant.init.fuel_hot.idx2_trq;
eng.plant.init.hc_hot.map       = zeros(length(eng.plant.init.hc_hot.idx1_spd),length(eng.plant.init.hc_hot.idx2_trq));

eng.plant.init.nox_hot.idx1_spd = eng.plant.init.fuel_hot.idx1_spd;
eng.plant.init.nox_hot.idx2_trq = eng.plant.init.fuel_hot.idx2_trq;
eng.plant.init.nox_hot.map       = zeros(length(eng.plant.init.nox_hot.idx1_spd),length(eng.plant.init.nox_hot.idx2_trq));

eng.plant.init.pm_hot.idx1_spd = eng.plant.init.fuel_hot.idx1_spd;
eng.plant.init.pm_hot.idx2_trq = eng.plant.init.fuel_hot.idx2_trq;
eng.plant.init.pm_hot.map       = zeros(length(eng.plant.init.hc_hot.idx1_spd), length(eng.plant.init.nox_hot.idx2_trq));

% O2
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
eng.plant.init.o2_hot.idx1_spd = eng.plant.init.fuel_hot.idx1_spd;
eng.plant.init.o2_hot.idx2_trq = eng.plant.init.fuel_hot.idx2_trq;
eng.plant.init.o2_hot.map       = zeros(length(eng.plant.init.fuel_hot.idx1_spd),length(eng.plant.init.fuel_hot.idx2_trq));

% exhaust table
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
eng.plant.init.equiv_hot.idx1_spd = eng.plant.init.fuel_hot.idx1_spd;
eng.plant.init.equiv_hot.idx2_trq = eng.plant.init.fuel_hot.idx2_trq;
eng.plant.init.equiv_hot.map       =  ones(length(eng.plant.init.equiv_hot.idx1_spd),length(eng.plant.init.equiv_hot.idx2_trq));

% Heat rejection variable Presid data table
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
eng.plant.init.htrej_hot.idx1_spd = eng.plant.init.fuel_hot.idx1_spd;
eng.plant.init.htrej_hot.idx2_trq = eng.plant.init.fuel_hot.idx2_trq;
eng.plant.init.htrej_hot.map       = zeros(length(eng.plant.init.fuel_hot.idx1_spd),length(eng.plant.init.fuel_hot.idx2_trq));

% Heat Transfer
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%5
eng.plant.init.ex_gas_flow_hot.idx1_spd =   eng.plant.init.fuel_hot.idx1_spd;
eng.plant.init.ex_gas_flow_hot.idx2_trq =   eng.plant.init.fuel_hot.idx2_trq;
eng.plant.init.ex_gas_flow_hot.map       =   eng.plant.init.fuel_hot.map *(1+20);                  % g/s  ex gas flow map:  for CI engines, exflow=(fuel use)*[1 + (ave A/F ratio)]

%eng.plant.init.v0x\fuel use, thermal and emissions\thermal\fc heat net calculation
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
eng.plant.init.ex_pwr.map = eng.plant.init.fuel_hot.idx1_spd'*eng.plant.init.fuel_hot.idx2_trq;
eng.plant.init.ex_temp.map = eng.plant.init.ex_pwr.map./(eng.plant.init.ex_gas_flow_hot.map *1089) + 20;  % W   EO ex gas temp = Q/(MF*cp) + Tamb (assumes engine tested ~20 C)
eng.plant.init.ex_temp.idx1_spd = eng.plant.init.fuel_hot.idx1_spd; 
eng.plant.init.ex_temp.idx2_trq = eng.plant.init.fuel_hot.idx2_trq;
eng.plant.init.temp_operating = 90;
eng.plant.init.ex_temp_operating = mean(mean(eng.plant.init.ex_temp.map));

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%Calculations
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% maximum and minimum calculations
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
eng.plant.init.trq_max_hot.trq_max	= max(eng.plant.init.trq_max_hot.map); % N-m
[eng.plant.init.pwr_max_hot.pwr_max,Idx]  = max(eng.plant.init.trq_max_hot.idx1_spd.* eng.plant.init.trq_max_hot.map); % W
eng.plant.init.pwr_max_hot.map = eng.plant.init.trq_max_hot.idx1_spd.* eng.plant.init.trq_max_hot.map; % W

%Code to compute maximum speed of the engine. Speed at 80% of peak power.
eng.plant.init.spd_max =   eng.plant.init.trq_max_hot.idx1_spd(Idx);
if Idx < length(eng.plant.init.pwr_max_hot.map)
    eng.plant.init.spd_max = min(interp1(eng.plant.init.pwr_max_hot.map(Idx:end)+1e-6*(1:length(eng.plant.init.pwr_max_hot.map(Idx:end))),eng.plant.init.trq_max_hot.idx1_spd(Idx:end),eng.plant.init.pwr_max_hot.pwr_max * 0.80,'linear','extrap'),max(eng.plant.init.trq_max_hot.idx1_spd));
end

% Calculate the max engine efficiency in within the max torque curve
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
eng.plant.init.eff_hot_pwr.map    = eng.plant.init.fuel_hot.idx1_spd'*eng.plant.init.fuel_hot.idx2_trq/eng.plant.init.fuel_heating_val./(eng.plant.init.fuel_hot.map);
eng.tmp.max_trq         = interp1(eng.plant.init.trq_max_hot.idx1_spd,eng.plant.init.trq_max_hot.map,eng.plant.init.fuel_hot.idx1_spd);
eng.tmp.max_trq         = eng.tmp.max_trq(:)*ones(1,length(eng.plant.init.fuel_hot.idx2_trq));
eng.tmp.max_trq         = (eng.plant.init.fuel_hot.idx2_trq(:) * ones(1,length(eng.plant.init.fuel_hot.idx1_spd)))' > eng.tmp.max_trq;
eng.plant.init.eff_hot_pwr.map(eng.tmp.max_trq) = 0;
eng.plant.init.eff_max        = max(max(eng.plant.init.eff_hot_pwr.map));


%GASOLINE ENGINE zero fuel friction losses 

%some explanation - total losses are called
%'trq_zero_fuel_total_friction' or
%'mep_zero_fuel_total_friction'.

% total friction is made of two parts - 
% Part 1 
   %non pumping losses ( indepdent of
   % throttle position, function of speed only) , given by
   %mep_zero_fuel_non_pumping 

% and
% Part 2 - pumping losses , given by total_zero_fuel_pumping_friction_mep
% pumping losses have two sub categories 
   % Part 2a - throttle related losses given by mep_zero_fuel_throttle_pumping
   % which vary with throttle and speed ( look up table)
% and 
   % Part 2b - valve related losses given by mep_zero_fuel_valve_pumping
   % which also varies with speed and throttle position



% Engine pumping losses when fuel cut -off ; function of throttle and speed
eng.plant.init.mep_zero_fuel_throttle_pumping.idx1_spd = [100 200 300 400 500 600];
eng.plant.init.mep_zero_fuel_throttle_pumping.idx2_throttle = [0:5:55]/100;  % in the [0-1] range

% Pumping throttling  mep from steady state data of the 2.2 L engine , with intake and
% exhaust pressure  measured in psia. Data file: 2008-11_24_isobut20 with
% no EGR

eng.plant.init.mep_zero_fuel_throttle_pumping.map = [...
   13.9408   10.4203    7.3257    4.6568    2.4139    0.5967    0.5967    0.5967    0.5967    0.5967    0.5967    0.5967;...
   16.7960   13.2012   10.0322    7.2891    4.9719    3.0804    1.6148    0.5750    0.5750    0.5750    0.5750    0.5750;...
   18.9311   15.2620   12.0188    9.2014    6.8098    4.8441    3.3042    2.1901    1.5019    1.2395    1.4029    1.9921;...
   20.3462   16.6029   13.2854   10.3937    7.9278    5.8878    4.2736    3.0852    2.3227    1.9860    2.0751    2.5901;...
   21.0413   17.2237   13.8319   10.8659    8.3258    6.2115    4.5230    3.2603    2.4235    2.0125    2.0274    2.4680;...
   21.0164   17.1245   13.6584   10.6182    8.0037    5.8151    4.0523    2.7154    1.8043    1.3190    1.2596    1.6260].*6.89475; % psi to kpa

% End of throttle pumping mep calculation

% pumping valve flow mep calculation Step 1 - Using intake manifold pressure (imep) from 2008-11-24_isobut20.imepc stands for
% imep-cold; 
%imep-cold is calculated from  imep Using equation 13.8 from Heywood page 727
% 

eng.plant.init.imepc_zero_fuel_valve_pumping.idx1_spd = eng.plant.init.mep_zero_fuel_throttle_pumping.idx1_spd ;
eng.plant.init.imepc_zero_fuel_valve_pumping.idx2_throttle = eng.plant.init.mep_zero_fuel_throttle_pumping.idx2_throttle;


eng.plant.init.imepc_zero_fuel_valve_pumping.map = [...
    0.009   0.009   0.0175   0.0370   0.0494   0.0547   0.0547   0.0547   0.0547   0.0547   0.0547   0.0547;
    0.007   0.0109  0.0522   0.0865  0.1137   0.1338   0.1469   0.1529   0.1529   0.1529   0.1529    0.1529;
    0.03    0.0466  0.1028   0.1519  0.1939   0.2288   0.2575   0.2913   0.2980   0.2977   0.3069    0.3069;
    0.05    0.0988  0.1692   0.2331  0.2900   0.3397   0.3824   0.4181   0.4467   0.4682   0.4826    0.4900;
    0.0730  0.1659  0.2516   0.3303  0.4020   0.4665   0.5241   0.5745   0.6179   0.6542   0.6835    0.7057;
    0.1417  0.2943  0.3449   0.4434  0.5299   0.6092   0.6816   0.7468   0.8051   0.8562   0.9003    0.9373];


% pumping valve flow mep calculation Step 2 - Calculation of intake valve diameter based on cylinder volume and number
% of engine cylinders.
% Assumptions - bore to stroke (b/s) ratio for I-4 engines is 0.925 and V engines
% is 1.07 [ both Heywood, et.al SAE paper # 2009-01-1892] ; so knowing
% volume per cylinder and b/s ratio, solve for bore (b).
% Also , from page 171, Figure 7-11 of book - Colin R Fergusson,
% Kirkpatrick,'Internal Combustion engines' , intake valve diameter for 2
% intake valves is 0.33*bore, and 0.47 * bore for single intake valve.

eng_intake_valve_dia_in_mm = (0.33 * ((eng.plant.init.displ_init/eng.plant.init.num_cyl_init)/0.8496)^1/3)*1000;

eng_num_intake_valve_per_cylinder =2;

% pumping valve flow mep calculation step 3 - then , using equation 14 from SAE paper # 840807 ( Heywood equation
% 13.13) with 'F' calculated from equation 12 , SAE paper#840807 ( Heywood
% equation 13.13)

F = (eng_num_intake_valve_per_cylinder * (eng_intake_valve_dia_in_mm*0.03937)^2 *eng.plant.init.num_cyl_init)./(eng.plant.init.displ_init *0.0610);

eng.plant.init.mep_zero_fuel_valve_pumping.idx1_spd = eng.plant.init.imepc_zero_fuel_valve_pumping.idx1_spd;
eng.plant.init.mep_zero_fuel_valve_pumping.idx2_throttle = eng.plant.init.imepc_zero_fuel_valve_pumping.idx2_throttle;
eng.plant.init.mep_zero_fuel_valve_pumping.map = eng.plant.init.imepc_zero_fuel_valve_pumping.map./F^1.28;


eng.plant.init.mep_zero_fuel_valve_pumping.map = eng.plant.init.mep_zero_fuel_valve_pumping.map.*6.89475; % psi to kpa

% End of valve pumping mep calculation.

eng.plant.init.mep_zero_fuel_pumping.idx1_spd = eng.plant.init.mep_zero_fuel_valve_pumping.idx1_spd;
eng.plant.init.mep_zero_fuel_pumping.idx2_throttle = eng.plant.init.mep_zero_fuel_valve_pumping.idx2_throttle;
eng.plant.init.mep_zero_fuel_pumping.map = -(eng.plant.init.mep_zero_fuel_throttle_pumping.map + eng.plant.init.mep_zero_fuel_valve_pumping.map);

% End of total ( valve + throtle) pumping mep calculation


% Non - pumping Friction Torque for a 1.7 L Engine Based on Heywood. "Internal Combustion Fundamentals,"
% Section 13.6.1 Figure 13-14 (a) P726 
%Code for fmep (non pumping) calculation Gasoline

eng.plant.init.mep_zero_fuel_non_pumping.idx1_spd =(100:100:600);
eng.plant.init.mep_zero_fuel_non_pumping.map =  polyval([4.2337e-4  -3.41045e-3 62.831],(100:100:max(eng.plant.init.mep_zero_fuel_non_pumping.idx1_spd)));

% end of non pumping mep calculation
% begin adding non pumping and pumping mep to form total frictional mep 
eng.plant.init.mep_zero_fuel_total_friction.idx1_spd = eng.plant.init.mep_zero_fuel_pumping.idx1_spd;
eng.plant.init.mep_zero_fuel_total_friction.idx1_throttle = eng.plant.init.mep_zero_fuel_pumping.idx2_throttle;
eng.plant.init.mep_zero_fuel_total_friction.map = zeros(size(eng.plant.init.mep_zero_fuel_pumping.map));

for (i=1:1:length(eng.plant.init.mep_zero_fuel_total_friction.idx1_throttle))
      eng.plant.init.mep_zero_fuel_total_friction.map(:,i) = eng.plant.init.mep_zero_fuel_pumping.map(:,i)- eng.plant.init.mep_zero_fuel_non_pumping.map';
end
% end of total frictional mep calcualation
% begin conversion of mep to torque based on cylinder displacement assume 4
% stroke cumbustion cycle

eng.plant.init.trq_zero_fuel_total_friction.idx1_spd =[0:100:600];
eng.plant.init.trq_zero_fuel_total_friction.idx2_throttle = [0:5:55]/100;  % in the [0-1] range
eng.plant.init.trq_zero_fuel_total_friction.map = zeros(size(eng.plant.init.mep_zero_fuel_total_friction.map));
eng.plant.init.trq_zero_fuel_total_friction.map((2:end+1),:) = eng.plant.init.mep_zero_fuel_total_friction.map.*eng.plant.init.displ_init/1000 / 4/ pi;
%eng.plant.init.trq_zero_fuel_total_friction.map(1,:)=interp1(eng.plant.init.trq_zero_fuel_total_friction.idx1_spd(2:end),eng.plant.init.trq_zero_fuel_total_friction.map(2:end,:),eng.plant.init.trq_zero_fuel_total_friction.idx1_spd(1),'linear','extrap');
eng.plant.init.trq_zero_fuel_total_friction.map(1,:)=0;
% end of engine friction torque at zero fuel calculation

% Engine off torque is the engine total friction torque at WOT
eng.plant.init.trq_eng_off.idx1_spd =[0:100:600];
eng.plant.init.trq_eng_off.map      = eng.plant.init.trq_zero_fuel_total_friction.map(:,end);

% End of all fuel off trq calculations

%% Throttle position with fuel off as a fraction, i.e. 5% throttle will be 0.05
eng.plant.init.throttle_position_fuel_off = 0.05; % this can be a lookup table as a function of speed.

%%
clear F;   

eng = rmfield(eng,'tmp');