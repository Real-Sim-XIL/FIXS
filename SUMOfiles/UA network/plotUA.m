clear 

fileName = 'UAdemoRealSim_Orig_as0p1.csv';
DataRaw = readtable(fileName);

%%
Data.t = (0:0.1:300)';
Data.id = [];
Data.idStr = [];
Data.pos = [];
Data.spd = [];
Data.mainOrRamp = [];

allVehId = unique(DataRaw.vehicle_id); 

for iV = 2:numel(allVehId) 
    
    curId = allVehId{iV};
%     id = str2double(curId(13:end));
    temp = DataRaw(strcmp(DataRaw.vehicle_id, curId),:);
    
    if ~isempty(temp)
        time = temp.timestep_time;
%         pos = temp.vehicle_pos;
        link = temp.edge_id;
        spd = temp.vehicle_speed;

        if numel(time) > 1
            Data.idStr = [Data.idStr, {curId}];
            Data.spd = [Data.spd, interp1(time, spd, Data.t)];
        end
%         if size(Data.id, 2) ~= size(Data.pos, 2)
%             keyboard
%         end
        
    end
end


%%
% ind = find(contains(Data.idStr, '0_2')); 
ind = find(strcmpi(Data.idStr, '0_2'));
figure; 
plot(Data.t, Data.spd(:,ind));
% ylim([0 60]/3.6)
% xlim([40 110])

% %%
% Veh.t = Data.t;
% Veh.speed = Data.spd(:,ind);
% 
% save('UA_veh0_2_orig_as0p1', 'Veh')

