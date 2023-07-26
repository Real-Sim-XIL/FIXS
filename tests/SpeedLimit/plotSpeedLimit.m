% plot trajectories of main and ramp lanes
% check if there is collision
% estimate energy and power consumption
% 

% time:
%   t_arr
% data: vehID-by-timeStep
%   pos_veh_data, spd_veh_data, acc_veh_data, spd_d_veh_data, 
%   Lane_no_data, Link_no_veh_data, sf_data, tf_data, type_veh_data
% type: 
%   610-CA LDV 620-H LDV 630-CA HGV 640-H HGV

%% compare controlled versus uncontrolled
% clear all

% DataVisRaw = dlmread('./ACM_SpeedHarm_test_vis_v4_lead_011.fzp', ';', 22, 0);
DataVisRaw = dlmread('./speedLimit_001.fzp', ';', 28, 0);

% $VEHICLE:1 SIMSEC;2 NO; 3 LANE\LINK\NO; 4 LANE\INDEX; 5 POS; 6 DESSPEED; 7 SPEED; 8 ACCELERATION; 9 VEHTYPE
DataVisRawS = sortrows(DataVisRaw, 2); % sort by vehicle number
 
nVeh = max(DataVisRawS(:, 2)); 50; 
nT = max(DataVisRawS(:, 1));
 
DataVisCell = cell(nVeh, 1);
DataVisStruct = struct('Link_no_veh_data', [], 'Lane_no_data', [], ...
    'pos_veh_data', [], 'spd_d_veh_data', [], 'spd_veh_data', [], ...
    'acc_veh_data', [], 'type_veh_data', []);
% indSel = 2:1000; 
indSel = 2:(max(DataVisRawS(:,1))*10-10);
for i = 1:nVeh-2
    
    fprintf('cur veh %d/%d\n', i, nVeh)
    
    data = DataVisRawS(DataVisRawS(:, 2) == i, :);
    preTime = (0:0.1:(data(1,1)-0.1))';
    pre = [preTime i*ones(numel(preTime), 1) zeros(numel(preTime), size(DataVisRawS,2)-2)];
    sufTime = ((data(end,1)+0.1):0.1:nT)';
    suf = [sufTime i*ones(numel(sufTime), 1) zeros(numel(sufTime), size(DataVisRawS,2)-2)];
    DataVisCell{i} = [pre; data; suf];
    
    DataVisStruct.Link_no_veh_data = [DataVisStruct.Link_no_veh_data; DataVisCell{i}(indSel,3)'];
    DataVisStruct.Lane_no_data = [DataVisStruct.Lane_no_data; DataVisCell{i}(indSel,4)'];
    DataVisStruct.pos_veh_data = [DataVisStruct.pos_veh_data; DataVisCell{i}(indSel,5)'];
    DataVisStruct.spd_d_veh_data = [DataVisStruct.spd_d_veh_data; DataVisCell{i}(indSel,7)'];
    DataVisStruct.spd_veh_data = [DataVisStruct.spd_veh_data; DataVisCell{i}(indSel,6)'];
    DataVisStruct.acc_veh_data = [DataVisStruct.acc_veh_data; DataVisCell{i}(indSel,8)'];
    DataVisStruct.type_veh_data = [DataVisStruct.type_veh_data; DataVisCell{i}(indSel,9)'];
end

DataVisStruct.t_arr = DataVisCell{1}(indSel,1)-DataVisCell{1}(indSel(1),1);
DataVisStruct.time_step = 10;
data = DataVisStruct;

%%%%%%%%%
%%%%%%%%%
% ExtDriver = DataVisStruct; save('ExtDriverData.mat', 'ExtDriver');
%% constants
KMH_2_MPS = 1/3.6;

data.pos_veh_data_f = cumtrapz(data.t_arr, data.spd_veh_data'/3.6)';

%%
figure
plot(DataVisStruct.t_arr, DataVisStruct.spd_veh_data(6,:))
