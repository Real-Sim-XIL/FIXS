clear 

fileName = './ACM_coordMerge_v2.csv';
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
%         id = str2double(curId(13:end))*ones(size(link));
        pos = [0; cumsum(spd(1:end-1).*diff(time))];

        allLinkId = unique(link);
        if strcmpi(link(1), '-1007000.0.00')
            mainOrRamp = 2;
            

        elseif strcmpi(link(1), '-1008000.0.00')
            mainOrRamp = 1;
            
            pos = pos - 300;
        else
            keyboard
        end
        
        if numel(time) > 1
            if contains(curId, 'flow')
                Data.id = [Data.id, str2double(curId(8:end))*ones(size(Data.t))];
            elseif contains(curId, 'vehicle')
%                 Data.id = [Data.id, 10000+str2double(curId(9:end))*ones(size(Data.t))];
            end
            Data.idStr = [Data.idStr, {curId}];
            Data.pos = [Data.pos, interp1(time, pos, Data.t)];
            Data.spd = [Data.spd, interp1(time, spd, Data.t)];
            Data.mainOrRamp = [Data.mainOrRamp, mainOrRamp*ones(size(Data.t))];
        end
%         if size(Data.id, 2) ~= size(Data.pos, 2)
%             keyboard
%         end
        
    end
end

%%
figure;
SUB_ROW = 1; SUB_COL = 1; subNum = 1; ax = [];
set(gcf, 'position', ...
    [1          41        1920        1083])
AxisPos = tightPlotAxes(SUB_ROW, SUB_COL, [0, 0, 0, 0]);
c = genColorCodes(); colorList = {c.b, c.r};
lineList = {'-', '--'};

ax(subNum) = axes('OuterPosition', AxisPos(subNum, :));
subNum = subNum + 1; hold on
indMain = find(Data.mainOrRamp(1,:) == 1); % main index
plot(Data.t, Data.pos(:,indMain), '-', 'color', c.b);
for i = 1:numel(indMain)
    indTemp = find(~isnan(Data.pos(:,indMain(i))),1,'first');
%     text(Data.t(indTemp), Data.pos(indTemp, indMain(i)), num2str(Data.id(indTemp, indMain(i))),  ...
%         'color', c.b, 'fontsize', 8)
    text(Data.t(indTemp), Data.pos(indTemp, indMain(i)), Data.idStr(indMain(i)),  ...
        'color', c.b, 'fontsize', 8, 'interpreter', 'none')
end
indRamp = find(Data.mainOrRamp(1,:) == 2); % ramp index
plot(Data.t, Data.pos(:,indRamp), '--', 'color', c.r);
for i = 1:numel(indRamp)
    indTemp = find(~isnan(Data.pos(:,indRamp(i))),1,'first');
%     text(Data.t(indTemp), Data.pos(indTemp, indRamp(i))+10, num2str(Data.id(indTemp, indRamp(i))),  ...
%         'color', c.r, 'fontsize', 8)
    text(Data.t(indTemp), Data.pos(indTemp, indRamp(i))+10, Data.idStr(indRamp(i)),  ...
        'color', c.r, 'fontsize', 8, 'interpreter', 'none')
end
ylim([0 1200])
xlim([0 200])

%%
% % ind = find(Data.id(1,indRamp) == 10001); 
% % figure; 
% % plot(Data.t, Data.spd(:,indRamp(ind)));
% ind = find(contains(Data.idStr, 'flow_0.1')); 
% figure; 
% plot(Data.t, Data.spd(:,ind(1)));
% ylim([0 60]/3.6)

%%
% ind = find(Data.id(1,indMain) == 10000); 
% figure; 
% plot(Data.t, Data.spd(:,indMain(ind)));
ind = find(contains(Data.idStr, 'vehicle_0')); 
figure; 
plot(Data.t, Data.spd(:,ind));
ylim([0 60]/3.6)
xlim([40 110])
% 
% % %%
% % ind = find(contains(Data.idStr, 'flow_0.0')); 
% % figure; 
% % plot(Data.t, Data.spd(:,ind));
% 
% %%
% % ind = find(Data.id(1,indRamp) == 10001); 
% % figure; 
% % plot(Data.t, Data.spd(:,indRamp(ind)));
% ind = find(contains(Data.idStr, 'vehicle_1')); 
% figure; 
% plot(Data.t, Data.spd(:,ind));
% ylim([0 60]/3.6)

%%
% ind = find(contains(Data.idStr, 'flow_0.9')); 
% figure; 
% plot(Data.t, Data.spd(:,ind));


% %%
% k1 = 0.026548;
% k2 = -0.642366;
% 
% v0s = 12.98;
% t0 = 54.3;
% t_arr = 0:0.001:100;
% 
% vv = k1 / 2 * t_arr.^2 + k2 * t_arr + v0s;
% 
% figure;
% plot(t_arr+t0, vv)
% ylim([0 15])
