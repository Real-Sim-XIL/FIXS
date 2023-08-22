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
clear all

% DataVisRaw = dlmread('./ACM_SpeedHarm_test_vis_v4_lead_011.fzp', ';', 22, 0);
DataVisRaw = dlmread('./coordMerge_001.fzp', ';', 22, 0);

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
for i = 1:nVeh
    
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
    DataVisStruct.spd_d_veh_data = [DataVisStruct.spd_d_veh_data; DataVisCell{i}(indSel,6)'];
    DataVisStruct.spd_veh_data = [DataVisStruct.spd_veh_data; DataVisCell{i}(indSel,7)'];
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

% number of vehicle, number of time steps
[nVeh, nT] = size(data.spd_veh_data);

% find CAV and HV
type = nan(nVeh, 1); % 2-CAV, 1-HV
for iVeh = 1:nVeh-2
    ind = find(data.type_veh_data(iVeh,:) > 1e-5, 1, 'first');
    type(iVeh) = data.type_veh_data(iVeh,ind);
end
% IS_CAV = type == 100 | type == 200;
% IS_HV = type == 300 | type == 600;
IS_CAV = type == 100 | type == 200;
IS_HV = type == 300 | type == 600;

% get short name for variables
link = data.Link_no_veh_data;
lane = data.Lane_no_data;
dt = 1/data.time_step;

%% find main road trajectories
MainRoad.Specs.len = [799.5; 0.8; 3.902; 0.8; 155.324; 3.418; 112];
MainRoad.Specs.lenOffset = [0; 0; 0; 0; 0; 0; 0];
MainRoad.Specs.lenCumsum = cumsum(MainRoad.Specs.len);
MainRoad.Specs.link = [1; 10000; 7; 10007; 3; 10002; 4];
MainRoad.Specs.lane = [1; 1; 1; 1; 2; 1; 1];


Ramp.Specs.len = [802.296; 0.803; 4.305; 0.795; 155.324; 3.418; 112];
Ramp.Specs.lenOffset = [0; 0; 0; 0; 0; 0; 0];
Ramp.Specs.lenCumsum = cumsum(Ramp.Specs.len);
Ramp.Specs.link = [2; 10001; 6; 10006; 3; 10002; 4];
Ramp.Specs.lane = [1; 1; 1; 1; 1; 1; 1];

% linkRampLen = [408.216; 0.985; 7.009; 0.925; 111.659];
% linkRampLenSum = cumsum(linkRampLen);
% linkRampList = [7; 10000; 9; 10001; 3];
% laneRampList = [1; 1; 1; 1; 1];
% 
% lenRampDiff = sum(linkRampLen(1:end-1)) - sum(linkMainLen(1:4));

% simply find any vehicle on any part of the main road at any time
IS_MAIN = false(size(link));

MainRoad.t = data.t_arr;
% the pos_veh_data is based on relative position to each link. need to
% cumulatively sum the length of previous links to find the 'distance',
% which has 0 meter defined at the beginning of link 1.
MainRoad.dist = data.pos_veh_data;
% MainRoad.dist = data.pos_veh_data_f;
for i = 1:numel(MainRoad.Specs.link)
	selInd = (link == MainRoad.Specs.link(i) & lane == MainRoad.Specs.lane(i));
    IS_MAIN = IS_MAIN | selInd;
    if i > 1
        MainRoad.dist(selInd) = MainRoad.dist(selInd)+MainRoad.Specs.lenOffset(i);
        MainRoad.dist(selInd) = MainRoad.dist(selInd)+MainRoad.Specs.lenCumsum(i-1);
    end
end
MainRoad.dist(~IS_MAIN) = nan;
MainRoad.spd = data.spd_veh_data*KMH_2_MPS;
MainRoad.spd(~IS_MAIN) = nan;
MainRoad.accel = data.acc_veh_data;
MainRoad.accel(~IS_MAIN) = nan;
MainRoad.spdDes = data.spd_d_veh_data;
MainRoad.spdDes(~IS_MAIN) = nan;


POS_THLD = 300; % only find data before this position on combined road
MergeOne.t = data.t_arr;
MergeOne.dist = nan(nVeh, nT); % through cumsum of link positions
MergeOne.distInt = nan(nVeh, nT); % through numerical integration of vehicle speed
MergeOne.spd = nan(nVeh, nT);
MergeOne.accel = nan(nVeh, nT);
MergeOne.linkInit = nan(nVeh, 1);
MergeOne.distDes = nan(nVeh, nT);
MergeOne.spdDes = nan(nVeh, nT);
for iVeh = 1:nVeh
    curLink = data.Link_no_veh_data(iVeh, :);
    curLane = data.Lane_no_data(iVeh, :);
    indBeg = find(curLink > 1e-5, 1, 'first');
    if iVeh == 2
        indBeg = find(abs(curLink-Ramp.Specs.link(1))<1e-5, 1, 'first');
    end
    if (iVeh == 2 && ~isempty(indBeg)) || ( ~isempty(indBeg) && ((curLink(indBeg) == MainRoad.Specs.link(1) && curLane(indBeg) == MainRoad.Specs.lane(1)) ...
            || (curLink(indBeg) == Ramp.Specs.link(1) && curLane(indBeg) == Ramp.Specs.lane(1))) )
%         indEnd = find(abs(curLink-linkNoComb)<1e-5 & data.pos_veh_data(iVeh, :) > POS_THLD, 1, 'first');
        indEnd = find(abs(curLink-MainRoad.Specs.link(end)) < 1e-5, 1, 'first');
        if isempty(indEnd) 
            indEnd = nT;
        end
        indSel = indBeg:indEnd;
        MergeOne.dist(iVeh,indSel) = data.pos_veh_data(iVeh,indSel);
        MergeOne.distInt(iVeh,indSel) = data.pos_veh_data_f(iVeh,indSel);
        MergeOne.spd(iVeh,indSel) = data.spd_veh_data(iVeh,indSel)*KMH_2_MPS;
        MergeOne.accel(iVeh,indSel) = data.acc_veh_data(iVeh,indSel);
        MergeOne.spdDes(iVeh,indSel) = data.spd_d_veh_data(iVeh,indSel)*KMH_2_MPS;
        MergeOne.distDes(iVeh,indSel) = MergeOne.dist(iVeh,indBeg) ...
            +dt*[0 cumsum(MergeOne.spdDes(iVeh,indSel(1:end-1)))];
        MergeOne.linkInit(iVeh) = curLink(indBeg);
    end
end
    
IS_LINK_ONE = MergeOne.linkInit == MainRoad.Specs.link(1);
IS_LINK_TWO = MergeOne.linkInit == Ramp.Specs.link(1);

for i = 2:numel(MainRoad.Specs.link)
    selInd = link == MainRoad.Specs.link(i) & lane == MainRoad.Specs.lane(i);
    MergeOne.dist(selInd) = MergeOne.dist(selInd)+MainRoad.Specs.lenCumsum(i-1);
end
for i = 2:numel(Ramp.Specs.link)
    selInd = link == Ramp.Specs.link(i) & lane == Ramp.Specs.lane(i);
    MergeOne.dist(selInd) = MergeOne.dist(selInd)+Ramp.Specs.lenCumsum(i-1);
end
% selInd = ismember(link, linkMainList(6:end)) | (link == linkMainList(5) & lane == laneMainList(5));
% selInd(IS_LINK_ONE, :) = 0;
% MergeOne.dist(selInd) = MergeOne.dist(selInd) + lenRampDiff;

%%
yMax = 900;
TXT_FONT_SIZE = 8;
tOffset = 0.5;

figure
SUB_ROW = 1; SUB_COL = 1; subNum = 1; ax = [];
set(gcf, 'position', ...
    [1          41        1920        1083])
colorList = {'b', 'r'};
lineList = {'-', '--'};

ax(subNum) = subplot(SUB_ROW,SUB_COL,subNum);
subNum = subNum + 1; hold on
if ~isempty(MergeOne.dist(IS_HV & IS_LINK_ONE,:))
    p1 = plot(MergeOne.t, MergeOne.dist(IS_HV & IS_LINK_ONE,:), lineList{1}, 'color', colorList{1});
else
    p1 = plot(nan,nan, lineList{1}, 'color', colorList{1});
end
if ~isempty(MergeOne.dist(IS_CAV & IS_LINK_ONE,:))
    p2 = plot(MergeOne.t, MergeOne.dist(IS_CAV & IS_LINK_ONE,:), lineList{1}, 'color', colorList{2});
else
    p2 = plot(nan,nan, lineList{1}, 'color', colorList{2});
end
if ~isempty(MergeOne.dist(IS_HV & IS_LINK_TWO,:))
    p3 = plot(MergeOne.t, MergeOne.dist(IS_HV & IS_LINK_TWO,:), lineList{2}, 'color', colorList{1});
else
    p3 = plot(nan,nan, lineList{2}, 'color', colorList{1});
end
if ~isempty(MergeOne.dist(IS_CAV & IS_LINK_TWO,:))
    p4 = plot(MergeOne.t, MergeOne.dist(IS_CAV & IS_LINK_TWO,:), lineList{2}, 'color', colorList{2});
else
    p4 = plot(nan,nan, lineList{2}, 'color', colorList{2});
end
grid on
legend([p1(1),p2(1),p3(1),p4(1)], {'HV Link 1', 'CAV Link 1', 'HV Link 2', 'CAV Link 2'})

ind = find(IS_HV);
for i = 1:numel(ind)
    [~,ind2] = min(abs(MergeOne.dist(ind(i),:)-yMax));
    text(MergeOne.t(ind2)-tOffset, MergeOne.dist(ind(i),ind2), num2str(ind(i)), 'color', colorList{1}, 'FontSize', TXT_FONT_SIZE)
end
ind = find(IS_CAV);
for i = 1:numel(ind)
    [~,ind2] = min(abs(MergeOne.dist(ind(i),:)-yMax));
    text(MergeOne.t(ind2)-tOffset, MergeOne.dist(ind(i),ind2), num2str(ind(i)), 'color', colorList{2}, 'FontSize', TXT_FONT_SIZE)
end

ylim([0 yMax])
linkaxes(ax, 'xy')

%%
yMax = 30;
TXT_FONT_SIZE = 8;
tOffset = 0.5;

selInd = 1:5; 

figure
SUB_ROW = 1; SUB_COL = 1; subNum = 1; ax = [];
set(gcf, 'position', ...
    [1          41        1920        1083])
colorList = {'b', 'r'};
lineList = {'-', '--'};

ax(subNum) = subplot(SUB_ROW,SUB_COL,subNum);
subNum = subNum + 1; hold on
temp = find(IS_HV & IS_LINK_ONE);
if ~isempty(temp)
    p1 = plot(MergeOne.t, MergeOne.spd(temp(selInd),:), lineList{1}, 'color', colorList{1});
end
temp = find(IS_CAV & IS_LINK_ONE);
if ~isempty(temp)
    p2 = plot(MergeOne.t, MergeOne.spd(temp(selInd),:), lineList{1}, 'color', colorList{2});
end
temp = find(IS_HV & IS_LINK_TWO);
if ~isempty(temp)
    p3 = plot(MergeOne.t, MergeOne.spd(temp(selInd),:), lineList{2}, 'color', colorList{1});
end
temp = find(IS_CAV & IS_LINK_TWO);
if ~isempty(temp)
    p4 = plot(MergeOne.t, MergeOne.spd(temp(selInd),:), lineList{2}, 'color', colorList{2});
end
grid on
legend([p1(1),p2(1),p3(1),p4(1)], {'HV Link 1', 'CAV Link 1', 'HV Link 2', 'CAV Link 2'})

ylim([0 yMax])
linkaxes(ax, 'xy')

%%
yMax = 30;
TXT_FONT_SIZE = 8;
tOffset = 0.5;

selInd = 1:5; 

figure
SUB_ROW = 1; SUB_COL = 1; subNum = 1; ax = [];
set(gcf, 'position', ...
    [1          41        1920        1083])
colorList = {'b', 'r'};
lineList = {'-', '--'};

ax(subNum) = subplot(SUB_ROW,SUB_COL,subNum);
subNum = subNum + 1; hold on
temp = find(IS_HV & IS_LINK_ONE);
if ~isempty(temp)
    p1 = plot(MergeOne.dist(temp(selInd),:)', MergeOne.spd(temp(selInd),:)', lineList{1}, 'color', colorList{1});
end
temp = find(IS_CAV & IS_LINK_ONE);
if ~isempty(temp)
    p2 = plot(MergeOne.dist(temp(selInd),:)', MergeOne.spd(temp(selInd),:)', lineList{1}, 'color', colorList{2});
end
temp = find(IS_HV & IS_LINK_TWO);
if ~isempty(temp)
    p3 = plot(MergeOne.dist(temp(selInd),:)', MergeOne.spd(temp(selInd),:)', lineList{2}, 'color', colorList{1});
end
temp = find(IS_CAV & IS_LINK_TWO);
if ~isempty(temp)
    p4 = plot(MergeOne.dist(temp(selInd),:)', MergeOne.spd(temp(selInd),:)', lineList{2}, 'color', colorList{2});
end
grid on
legend([p1(1),p2(1),p3(1),p4(1)], {'HV Link 1', 'CAV Link 1', 'HV Link 2', 'CAV Link 2'})

ylim([0 yMax])
linkaxes(ax, 'xy')

%%

DataField = fieldnames(DataVisStruct);
for i = 1:numel(DataField)
    eval([DataField{i}, '=', 'DataVisStruct.', DataField{i}, ';']);
end
    
%% 
figure;
plot(MergeOne.t, MergeOne.spd(21,:))
