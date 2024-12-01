%% File description
% Name : eng_cng_8100_186_JohnDeere					
% Author : A.Rousseau - ANL		                          				
% Description : Initialize the John Deere 8.1L (186kW) CNG engine

%Proprietary : Public
%Protected: false


% Model : eng_plant_hot_map													
% Technology : cng, cng																		
% Vehicle Type : Light, Heavy

%% Conversion constants
pxfUnitConversion.rpm2radps = pi/30;
pxfUnitConversion.gallon2liter = 3.78541;

%% File content
eng.plant.init.technology                       = 'cng';
eng.plant.init.num_cyl_init                     = 6;
eng.plant.init.num_cyl_banks                    = 1;
eng.plant.init.material                         = 'CastIron'; %Assumption
eng.plant.init.cyl_config                       ='inline';
eng.plant.init.cam_shaft_config                 ='dohc';
eng.plant.init.has_cyl_deac                     = true;
eng.plant.init.has_variable_valve_lift          = false;
eng.plant.init.has_variable_valve_timing        = true;
eng.plant.init.has_hcci                         = false;
eng.plant.init.has_direct_injec                 = false;
eng.plant.init.has_boost                        = false;
eng.plant.init.cng_tank_type                    = 'type4';%Assumption

eng.plant.init.mass.tank         = 100;
eng.plant.init.mass.fuel		= 50;
eng.plant.init.pwr_max			= 188.8269*1000;			% Watts
eng.plant.init.mass.eng             = 793-1.8*eng.plant.init.pwr_max/1000 + 1.8*eng.plant.init.pwr_max/1000; %(kg), Block Mass + Radiator Mass

eng.plant.init.time_response                  = 0.3; 						
eng.plant.init.spd_idle		 				= (650.*pxfUnitConversion.rpm2radps);
eng.plant.init.spd_idle_offset		 	= 0;  % this can be used to implement a higher idle speed for performance tests
eng.plant.init.warmup_init 				    = 0;						% This should normally by 0

eng.plant.init.fuel_density_val               = 0.217;          % kg/L
eng.plant.init.fuel_heating_val      			= 47000000;             % (J/kg)Specific LHV
eng.plant.init.fuel_carbon_ratio 			    = 12/13.8;        % (kg/kg) ref:Dr. Rob Thring

eng.plant.init.displ_init						= 8100;		% cc
eng.plant.init.inertia						= 0.2;		% kg-m^2 - approximate value
eng.plant.init.spd_str						= 10; % speed level (rad/s) the engine crank has to reach in order to start 

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% maximum curves at each speed (closed and wide open throttle)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% hot wide open throttle curves
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
eng.plant.init.trq_max_hot.idx1_spd= ([0 600 800 1000 1200 1400 1600 1800 2000 2200    2400    2500].*pxfUnitConversion.rpm2radps);
eng.plant.init.trq_max_hot.map= [0 0.5423    0.6779    0.9321    1.0643    1.0711    1.0338    0.9694    0.9016    0.8094  0   0].*1000;        

% Mid speed is used in logic to limit closed and wide open torque curves
eng.plant.init.spd_avg	= 0.5 * (eng.plant.init.trq_max_hot.idx1_spd(1)+ eng.plant.init.trq_max_hot.idx1_spd(length(eng.plant.init.trq_max_hot.idx1_spd)));
      
% % hot min torque curves
% %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% eng.plant.init.trq_min_hot.idx1_spd 	= eng.plant.init.trq_max_hot.idx1_spd;
% eng.plant.init.bmep_min_hot.map = -1.*[5.6000  101.4028  210.2150  232.7318  256.0895  280.2881  305.3274  331.2077  357.9288  385.4907  413.8935  443.1344]; % Based on tests conducted on a HD engine at ANL
% eng.plant.init.trq_min_hot.map   = eng.plant.init.bmep_min_hot.map .*eng.plant.init.displ_init/1000 / 4/ pi;
%  
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% consumption table
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

eng.plant.init.fuel_hot.idx1_spd	= ([600 800 1000 1200 1400 1600 1800 2000 2200].*pxfUnitConversion.rpm2radps);
eng.plant.init.fuel_hot.idx2_trq = [0.0014    0.0678    0.1356    0.2712    0.4067    0.5423    0.6779 0.8135    0.9490    1.0846].*1000; % (Eqn B)

% Fuel rate tables.
% Rows are x input (speed, rad/s).  Columns are y input (torque, N-m).  Table is fuel rate (kg/s)

eng.plant.init.fuel_hot.map = 1e-3*[...
   0.497    0.6971    0.8631    1.2719    1.8432    2.0422    2.5527    3.0207    3.4759    3.9725;...
   0.642    0.8055    1.2083    1.6111    2.0714    2.5915    3.2136    3.8359    4.4990    5.1831;...
   0.702    0.8885    1.3731    1.9489    2.5173    3.1797    3.9539    4.7077    5.4923    6.2769;...
   0.759    0.9062    1.3679    2.3770    3.0547    3.9082    4.8332    5.7538    6.6425    7.4167;...
   0.9264   1.0573    1.7996    2.5476    3.5738    4.5719    5.5940    6.5907    7.5519    8.5652;...
   0.969    1.1788    2.1014    2.9835    4.1309    5.1970    6.3762    7.5323    8.6307    9.7887;...
   1.0294   1.2946    2.2750    3.4305    4.6739    5.9425    7.2305    8.5403    9.7844   11.0684;...
   1.2318   1.4249    2.5173    3.8977    5.2535    6.7503    8.0985    9.5644   11.0419   12.6193;...
   1.4350   1.6209    3.1646    4.4304    5.9336    7.3636    8.9806   10.8059   12.4052   14.1774]*1.0195;



% % Expanding the map at low torque (0)
% for i=1:length(eng.plant.init.fuel_hot.idx1_spd)
%     tmp_map(i,:)=interp1(eng.plant.init.fuel_hot.idx2_trq,eng.plant.init.fuel_hot.map(i,:),[0 eng.plant.init.fuel_hot.idx2_trq],'linear','extrap');
% end
% eng.plant.init.fuel_hot.idx2_trq=[0 eng.plant.init.fuel_hot.idx2_trq];
% 
% %Maps negative values are replaced by the average of non negative closest values
% for i =1:length(eng.plant.init.fuel_hot.idx1_spd)
%     for j=1:length(eng.plant.init.fuel_hot.idx2_trq)
%         if tmp_map(i,j)<0 && i>1
%             tmp_map(i,j)=abs(0.5*(tmp_map(i-1,j)+tmp_map(i+1,j)));
%         end
%         if tmp_map(i,j)<0 && i==1
%            tmp_map(i,j)=abs(0.5*(tmp_map(i,j+1)+tmp_map(i+1,j)));
%         end
%     end
% end                   
% 
% eng.plant.init.fuel_hot.map=tmp_map;
% clear tmp_map 

% locate the minimum torque with fuel based on Willans lines

for i = 1:1:length(eng.plant.init.fuel_hot.idx1_spd)
    pp = spline(eng.plant.init.fuel_hot.map(i,:),eng.plant.init.fuel_hot.idx2_trq);
    eng.temp.plant.init.trq_min_hot.map(i) = ppval(pp,0.5/10000); 
end

pp1 = spline(eng.plant.init.fuel_hot.idx1_spd,eng.temp.plant.init.trq_min_hot.map);

for i=1:1:length(eng.plant.init.trq_max_hot.idx1_spd)
    eng.plant.init.trq_min_hot.map(i) = ppval(pp1,eng.plant.init.trq_max_hot.idx1_spd(i));
end

eng.plant.init.trq_min_hot.idx1_spd = eng.plant.init.trq_max_hot.idx1_spd;

clear pp pp1 ;

% Extend fuel map to below idle speed


fuel_rate_at_500_rpm= zeros(1,length(eng.plant.init.fuel_hot.idx2_trq));
for i =1:1:length(eng.plant.init.fuel_hot.idx2_trq)
    fuel_rate_at_500_rpm(1,i)= interp1(eng.plant.init.fuel_hot.idx1_spd,eng.plant.init.fuel_hot.map(:,i),500*pxfUnitConversion.rpm2radps,'linear','extrap');
end

eng.plant.init.fuel_hot.map         = [fuel_rate_at_500_rpm; eng.plant.init.fuel_hot.map];
eng.plant.init.fuel_hot.idx1_spd	= ([500 600 800 1000 1200 1400 1600 1800 2000 2200].*pxfUnitConversion.rpm2radps);

% Expand the map to the minimum torque with fuel curve

eng.temp.plant.init.trq_step = eng.plant.init.fuel_hot.idx2_trq(2)-eng.plant.init.fuel_hot.idx2_trq(1);

eng.temp.plant.init.trq_first_idx = eng.plant.init.fuel_hot.idx2_trq(1) - eng.temp.plant.init.trq_step;

temp_negative_torque_index = [eng.temp.plant.init.trq_first_idx:-eng.temp.plant.init.trq_step: min(eng.plant.init.trq_min_hot.map)];

temp_map = zeros(length(eng.plant.init.fuel_hot.idx1_spd),length(temp_negative_torque_index));
for i = 1:1:length(eng.plant.init.fuel_hot.idx1_spd)
    temp1_map(i,:)= [0 eng.plant.init.fuel_hot.map(i,:)];
end

for i =1:1:length(eng.plant.init.fuel_hot.idx2_trq)
    temp_trq_index(i)= eng.plant.init.fuel_hot.idx2_trq(end-i+1);
end

for i=1:1:length(eng.plant.init.fuel_hot.idx1_spd)
    for j = 1:1:length(eng.plant.init.fuel_hot.idx2_trq)
        temp2_map(i,j)= eng.plant.init.fuel_hot.map(i,end-j+1);
    end
end

for i=1:1:length(eng.plant.init.fuel_hot.idx1_spd)
      
    fuel_vector = interp1([temp_trq_index eng.plant.init.trq_min_hot.map(i)],[temp2_map(i,:) temp1_map(i,1)],[eng.temp.plant.init.trq_first_idx:-eng.temp.plant.init.trq_step:eng.plant.init.trq_min_hot.map(i)],'pchip');
    length_fuel_vector = length(fuel_vector);
    temp_map(i,:) = [fuel_vector zeros(1,length(temp_negative_torque_index)-length(fuel_vector))];
    clear fuel_vector;
end

eng.plant.init.fuel_hot.idx2_trq = [temp_negative_torque_index(end):eng.temp.plant.init.trq_step:temp_negative_torque_index(1) eng.plant.init.fuel_hot.idx2_trq];

for i = 1:1: length(eng.plant.init.fuel_hot.idx1_spd)
    for j=1:1:length(temp_map(i,:))
        temp3_map(i,j) = temp_map(i,end+1-j);
    end
end

eng.plant.init.fuel_hot.map = [temp3_map eng.plant.init.fuel_hot.map];

eng.plant.init.trq_zero_fuel_hot.idx1_spd = eng.plant.init.trq_max_hot.idx1_spd;
eng.plant.init.trq_zero_fuel_hot.map      = zeros(1,length(eng.plant.init.trq_max_hot.idx1_spd));

clear temp_negative_torque_index temp_map temp1_map length_fuel_vector temp2_map i j fuel_rate_at_500_rpm temp_trq_index temp2_map temp3_map

eng.plant.init.trq_zero_fuel_hot.fmep_map = zeros(length(eng.plant.init.fuel_hot.idx1_spd),length(eng.plant.init.fuel_hot.idx2_trq));

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
eng.plant.init.temp_operating = 90;
eng.plant.init.ex_temp_operating = mean(mean(eng.plant.init.ex_temp.map));

%SI Engines ENGINE zero fuel friction losses 

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
eng.plant.init.trq_zero_fuel_total_friction.map(1,:) =0;

% end of engine friction torque at zero fuel calculation

% Engine off torque is the engine total friction torque at WOT
eng.plant.init.trq_eng_off.idx1_spd =[0:100:600];
eng.plant.init.trq_eng_off.map      = eng.plant.init.trq_zero_fuel_total_friction.map(:,end);

% End of all fuel off trq calculations

%% Throttle position with fuel off as a fraction, i.e. 5% throttle will be 0.05
eng.plant.init.throttle_position_fuel_off = 0.05; % this can be a lookup table as a function of speed.

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


%%
clear F i;

eng = rmfield(eng,'temp');
