%% File description
% Name : eng_plant_ci_Cummins_ISX450_CAEC				
% Author : D. Deter - ORNL		                          				
% Description : Initialize the EPA 2010 Cummins ISX 450 (rated power: 336kW) diesel engine
% Max torque curve and speed range and fuel consumption at max trq  
% created from data from ORNL tests.        
% Data has some discrepancies therfore the fuel map has been multiplied by
% 0.9
%
% Proprietary : ORNL
% Model : eng_plant_hot_map													
% Technology : ci																		
% Vehicle Type : Heavy

%% File content
%% File content

eng.plant.init.technology                 = 'ci';
eng.plant.init.num_cyl_init               = 6;
eng.plant.init.material                   = 'CastIron'; %Assumption

eng.plant.init.eng_mass 					= 1197;				    %eng.plant.init.mass_block + eng.plant.init.mass_radiator + eng.plant.init.mass_vol + eng.plant.init.tank_mass;
eng.plant.init.tank_mass                  = 60;
eng.plant.init.fuel_mass 					= 965;						% Capacity of tank in kg - approximate value

eng.plant.init.time_response				= 0.4; 						
eng.plant.init.time_response_fast				= 0.1; 						
eng.plant.init.spd_idle                       = 600/30*pi(); %600rpm
eng.plant.init.warmup_init				    = 0;						% This should normally by 0
eng.plant.init.pwr_max                        = 336000;		  % Watts

eng.plant.init.fuel_density_val           = 0.84;          % kg/L Autonomie usual is .835
eng.plant.init.fuel_heating_val      		= 42500000;       % (J/kg)Specific LHV
eng.plant.init.fuel_carbon_ratio  	    = 12/13.8;        	% (kg/kg) ref:Dr. Rob Thring

eng.plant.init.displ_init					= 15000;					% cc
eng.plant.init.inertia					= 0.45;				% kg-m^2 - approximate value
eng.plant.init.spd_str					= 10; 					% speed level (rad/s) the engine crank has to reach in order to start 

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% maximum curves at each speed (closed and wide open throttle)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% hot max wide open throttle curves
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% eng.plant.init.trq_max_hot.idx1_spd=[0 600:100:2100]*0.104719755; 
% eng.plant.init.trq_max_hot.map = [0 1364.0	1411.5	1538.9	1647.8	1725.0	2157.6	2193.4	2165.1	2151.5	2143.8	2108.1	1974.9	1859.6	1679.9	1492.7  1350]; % N-m (Eqn B)
load TorqueCurve_ISX450_20141121_002.mat
input_trqcrv_spd_ISX = [600 700 800 900 1000 1100 1200 1300 1400 1500 1600 1700 1800 1900 2000 2100];     %% RPM
input_trqcrv_trq_ISX = interp1(TorqueCurve(:,1),TorqueCurve(:,2),input_trqcrv_spd_ISX);
input_trqcrv_spd_ISX=[0 input_trqcrv_spd_ISX];
input_trqcrv_trq_ISX=[input_trqcrv_trq_ISX(1) input_trqcrv_trq_ISX];
eng.plant.init.trq_max_hot.idx1_spd= input_trqcrv_spd_ISX*pi()/30; % rad/s;
eng.plant.init.trq_max_hot.map = input_trqcrv_trq_ISX;
clear TorqueCurve input_trqcrv_spd_ISX input_trqcrv_trq_ISX

% Mid speed is used in logic to limit closed and wide open torque curves
eng.plant.init.spd_avg	= 0.5 * (eng.plant.init.trq_max_hot.idx1_spd(1)+ eng.plant.init.trq_max_hot.idx1_spd(length(eng.plant.init.trq_max_hot.idx1_spd)));
      
% hot max closed throttle curves
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% eng.plant.init.trq_min_hot.idx1_spd 	= eng.plant.init.trq_max_hot.idx1_spd;
% eng.plant.init.trq_min_hot.map= [0 -5*ones(1,size(eng.plant.init.trq_min_hot.idx1_spd,2)-1)];
 load TorqueCurve_ISX450_motoring_20141118_004.mat
input_brktrqcrv_spd_ISX = [600 700 800 900 1000 1100 1200 1300 1400 1500 1600 1700 1800 1900 2000 2100];     %% RPM
input_brktrqcrv_trq_ISX = interp1(TorqueCurve(:,1),TorqueCurve(:,2),input_brktrqcrv_spd_ISX);
input_brktrqcrv_spd_ISX=[0 input_brktrqcrv_spd_ISX];
input_brktrqcrv_trq_ISX=[0 input_brktrqcrv_trq_ISX];
eng.plant.init.trq_min_hot.idx1_spd = input_brktrqcrv_spd_ISX*pi()/30; % rad/s
eng.plant.init.trq_min_hot.map = input_brktrqcrv_trq_ISX;
clear TorqueCurve input_brktrqcrv_spd_ISX input_brktrqcrv_trq_ISX

% figure
% hold on
% plot(eng.plant.init.trq_max_hot.idx1_spd, eng.plant.init.trq_max_hot.map,'r+-')
% plot(eng.plant.init.trq_min_hot.idx1_spd, eng.plant.init.trq_min_hot.map,'b+-')


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% consumption table
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
eng.plant.init.fuel_hot.idx1_spd= [600	615	715	830	945	1061	1176	1291	1406	1521	1637	1751	1866	1982  2100]*0.104719755;
eng.plant.init.fuel_hot.idx2_trq= [25	216.35	432.7	649.05	865.4	1081.75	1298.1	1514.45	1730.8	1947.15	2163.5];

% Rows represent speed (rad/s).  Columns represent torque (N-m).  Table is fuel rate (kg/s)
eng.plant.init.fuel_hot.map = 1e-3*[1060.08	3354.65	5656.03	8181.41	10914.43	13768.51	16622.33	16622.33	16622.33	16622.33	16622.33
948.79	3354.65	5656.03	8181.41	10914.43	13768.51	16622.33	16622.33	16622.33	16622.33	16622.33
1333.71	4061.69	6737.60	9556.61	12483.36	15710.69	18882.93	20064.22	20064.22	20064.22	20064.22
1689.79	4763.73	7769.61	11050.38	14436.49	18065.50	21877.36	25429.93	26692.68	26692.68	26692.68
2133.89	5528.74	8874.92	12547.45	16509.71	20450.56	24477.43	28497.52	31445.44	31445.44	31445.44
2625.61	6380.52	10010.75	14094.91	18482.19	22926.74	27553.70	32379.28	37405.99	43496.60	43496.60
3067.38	7385.58	11127.18	15797.26	20661.83	25341.83	30548.79	35724.83	41045.82	46297.94	50609.78
3613.52	8119.58	12715.90	17803.52	23134.66	28386.42	33879.44	39440.45	45132.60	50932.62	56112.27
4426.46	9167.30	14032.23	19466.31	25255.06	31117.12	37186.49	43367.89	49781.61	55784.11	59879.80
5246.00	10586.58	16037.85	21895.54	27945.24	34532.10	41237.16	48684.57	55705.53	61166.82	64795.04
6503.57	12492.99	18426.36	24412.12	31318.34	38207.66	45146.99	52390.89	59827.29	66921.01	68576.46
7607.40	13565.25	20404.27	27314.40	35094.48	42459.87	49048.26	56023.51	63870.77	69089.52	69089.52
8274.79	14662.96	22612.58	30754.45	39227.98	46804.54	53347.28	60879.42	68288.33	68288.33	68288.33
10008.55	16849.37	24539.45	33308.26	42098.70	48805.55	57456.25	64246.70	64246.70	64246.70	64246.70
10916.96	18040.09	26760.93	36287.29	46001.91	53305.08	61438.73	66583.76	66583.76	66583.76	66583.76]/3600;
%Expanding the map at low torque (0 10 20)
for i=1:length(eng.plant.init.fuel_hot.idx1_spd)
        tmp_map(i,:)=interp1(eng.plant.init.fuel_hot.idx2_trq,eng.plant.init.fuel_hot.map(i,:),[0 10 20 eng.plant.init.fuel_hot.idx2_trq],'linear','extrap');
end
eng.plant.init.fuel_hot.idx2_trq=[0 10 20 eng.plant.init.fuel_hot.idx2_trq];

%Maps negative values are replaced by the average of non negative closest values
for i =1:length(eng.plant.init.fuel_hot.idx1_spd)
    for j=1:length(eng.plant.init.fuel_hot.idx2_trq)
        if tmp_map(i,j)<0 && i>1
            tmp_map(i,j)=abs(0.5*(tmp_map(i-1,j)+tmp_map(i+1,j)));
        end
        if tmp_map(i,j)<0 && i==1
           tmp_map(i,j)=abs(0.5*(tmp_map(i,j+1)+tmp_map(i+1,j)));
        end
    end
end                   

eng.plant.init.fuel_hot.map=tmp_map;
clear tmp_map 

% eng.plant.init.trq_zero_fuel_hot.fmep_map = zeros(length(eng.plant.init.fuel_hot.idx1_spd),length(eng.plant.init.fuel_hot.idx2_trq));

% engine torque for fuel rate kg/s
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%DIESEL ENGINE
% Friction Torque for a 1.7 L Engine Based on Heywood. "Internal Combustion Fundamentals,"
% Section 13.6.1 Figure 13-14 (b) P726 Polynomial interp at low speed... Linear interp at high speed. Otherwise 6000 rpm equals 150 N-m of
% negative torque
% eng.plant.init.trq_zero_fuel_hot.idx1_spd =(0:20:max(eng.plant.init.trq_min_hot.idx1_spd));
% eng.tmp.half_way_idx = floor(length(eng.plant.init.trq_zero_fuel_hot.idx1_spd)/2);
% eng.tmp.half_way_idx(eng.tmp.half_way_idx<2)=2;
% eng.tmp.fmep_hot_map_part1 =  polyval([(60/2/pi)^2*1.5625e-005  (60/2/pi)*0.02875 86],eng.plant.init.trq_zero_fuel_hot.idx1_spd(1:eng.tmp.half_way_idx-1));
% eng.tmp.fmep_slope_part2 =  polyval([2*(60/2/pi)^2*1.5625e-005 (60/2/pi)*0.02875],eng.plant.init.trq_zero_fuel_hot.idx1_spd(eng.tmp.half_way_idx));
% eng.tmp.fmep_intercept_part2 = polyval([(60/2/pi)^2*1.5625e-005  (60/2/pi)*0.02875 86],eng.plant.init.trq_zero_fuel_hot.idx1_spd(eng.tmp.half_way_idx)) - eng.tmp.fmep_slope_part2.*eng.plant.init.trq_zero_fuel_hot.idx1_spd(eng.tmp.half_way_idx);
% eng.tmp.fmep_hot_map_part2 =  polyval([eng.tmp.fmep_slope_part2 eng.tmp.fmep_intercept_part2],eng.plant.init.trq_zero_fuel_hot.idx1_spd(eng.tmp.half_way_idx:end));
% eng.plant.init.trq_zero_fuel_hot.fmep_map = [eng.tmp.fmep_hot_map_part1 eng.tmp.fmep_hot_map_part2];
% eng.plant.init.trq_zero_fuel_hot.fmep_map(1) =0; 
% eng.plant.init.trq_zero_fuel_hot.map =  -eng.plant.init.trq_zero_fuel_hot.fmep_map * eng.plant.init.displ_init/1000 / 4/ pi;

% use motoring table instead
eng.plant.init.trq_zero_fuel_hot.idx1_spd =(0:20:max(eng.plant.init.trq_min_hot.idx1_spd));
eng.plant.init.trq_zero_fuel_hot.map=interp1(eng.plant.init.trq_min_hot.idx1_spd,eng.plant.init.trq_min_hot.map,eng.plant.init.trq_zero_fuel_hot.idx1_spd);

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

eng.plant.init.NOX_Output.idx1_spd = eng.plant.init.fuel_hot.idx1_spd;
eng.plant.init.NOX_Output.idx2_trq = eng.plant.init.fuel_hot.idx2_trq;
eng.plant.init.NOX_Output.map = [6.12	6.46	6.79	6.96	12.98	23.53	35.67	99.68	194.37	256.03	350.27	350.27	350.27	350.27
6.12	6.45	6.79	6.96	12.95	23.51	35.27	98.56	190.92	250.62	350.27	350.27	350.27	350.27
5.62	5.68	5.75	5.78	6.51	18.36	53.38	110.10	151.84	287.47	318.77	318.77	318.77	318.77
8.69	8.65	8.62	8.60	8.14	14.15	31.93	99.13	113.85	76.54	398.85	398.85	398.85	398.85
13.38	13.28	13.18	13.12	12.83	22.13	40.24	68.34	64.14	71.25	107.95	156.74	156.74	156.74
19.74	19.41	19.09	18.92	15.89	47.81	61.58	38.18	25.50	18.47	8.69	5.67	69.56	87.07
23.33	23.83	24.32	24.57	34.38	66.81	78.94	83.63	56.00	30.28	22.00	15.49	39.43	167.42
9.38	11.57	13.76	14.86	56.65	89.89	105.27	120.69	87.82	41.10	30.25	33.73	70.39	188.50
8.68	11.27	13.87	15.17	64.81	96.99	103.99	124.94	121.97	73.32	26.57	23.00	17.55	108.82
13.05	15.16	17.27	18.32	58.66	100.70	119.82	143.80	151.96	121.25	90.70	49.80	35.42	111.16
38.51	38.19	37.87	37.71	35.13	93.64	139.63	163.77	159.33	107.95	62.79	72.50	186.70	279.92
39.73	38.78	37.84	37.37	27.50	74.33	110.87	121.55	105.61	32.32	33.77	63.71	104.76	104.76
21.84	22.85	23.86	24.37	46.61	80.01	102.24	114.13	89.60	31.30	44.09	118.34	124.59	124.59
121.54	115.17	108.80	105.62	35.39	67.28	92.05	103.14	91.10	90.28	156.44	156.44	156.44	156.44
121.54	114.17	106.80	103.11	38.57	76.41	98.76	101.53	85.87	156.44	156.44	156.44	156.44	156.44]/3600;

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
eng.plant.init.htrej_hot.map       = zeros(length(eng.plant.init.htrej_hot.idx1_spd),length(eng.plant.init.htrej_hot.idx2_trq));

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

eng.plant.init.temp_operating = 96;
eng.plant.init.ex_temp_operating = mean(mean(eng.plant.init.ex_temp.map));

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%Calculations
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% maximum and minimum calculations
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
eng.plant.init.trq_max_hot.trq_max	 = max(eng.plant.init.trq_max_hot.map); % N-m
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
% eng.plant.init.eff_hot_pwr.map(eng.tmp.max_trq) = 0;
% points outside of map are maxed out instead of zeroed out
for i=1:length(eng.plant.init.fuel_hot.idx1_spd)
    temparray=find(eng.tmp.max_trq(i,:)==1);
    eng.plant.init.eff_hot_pwr.map(i,temparray)=eng.plant.init.eff_hot_pwr.map(i,temparray(1)-1);
end
% figure
% surf(eng.plant.init.fuel_hot.idx1_spd,eng.plant.init.fuel_hot.idx2_trq,eng.plant.init.eff_hot_pwr.map')

    
eng.plant.init.eff_max        = max(max(eng.plant.init.eff_hot_pwr.map));

eng = rmfield(eng,'tmp');


