clear 

fileName = './osm_out.csv';
DataRaw = readtable(fileName);

%%
Data.t = (0:0.1:300)';
Data.id = [];
Data.idStr = [];
Data.pos = [];
Data.spd = [];

allVehId = unique(DataRaw.vehicle_id); 

for iV = 2:numel(allVehId) 
    
    curId = allVehId{iV};
%     id = str2double(curId(13:end));
    temp = DataRaw(strcmp(DataRaw.vehicle_id, curId),:);
    
    if ~isempty(temp)
        time = temp.timestep_time;
        pos = temp.vehicle_pos;
        link = temp.edge_id;
        spd = temp.vehicle_speed;
%         id = str2double(curId(13:end))*ones(size(link));

        allLinkId = unique(link);
    
        if numel(time) > 1
            if contains(curId, 'flow')
                Data.id = [Data.id, str2double(curId(8:end))*ones(size(Data.t))];
            elseif contains(curId, 'vehicle')
                Data.id = [Data.id, 10000+str2double(curId(9:end))*ones(size(Data.t))];
            end
            Data.idStr = [Data.idStr, {curId}];
            Data.pos = [Data.pos, interp1(time, pos, Data.t)];
            Data.spd = [Data.spd, interp1(time, spd, Data.t)];
        end

    end
end

%%
% ind = find(Data.id(1,indMain) == 10000); 
% figure; 
% plot(Data.t, Data.spd(:,indMain(ind)));
ind = find(strcmpi(Data.idStr, '1')); 
figure; 
plot(Data.t, Data.spd(:,ind));

% %%
% ind = find(contains(Data.idStr, 'flow_0.0')); 
% figure; 
% plot(Data.t, Data.spd(:,ind));

%%
% ind = find(Data.id(1,indRamp) == 10001); 
% figure; 
% plot(Data.t, Data.spd(:,indRamp(ind)));
ind = find(contains(Data.idStr, 'flow_1.9')); 
figure; 
plot(Data.t, Data.spd(:,ind));
ylim([0 60]/3.6)