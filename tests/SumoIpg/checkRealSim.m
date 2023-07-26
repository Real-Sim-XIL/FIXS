clear 

%% get SUMO data
fileName = './coordMerge.csv';
Sumo.DataRaw = readtable(fileName);

Sumo.Data.t = (0:0.1:200)';
Sumo.Data.idStr = [];
Sumo.Data.spd = [];
Sumo.Data.x = [];
Sumo.Data.y = [];
Sumo.Data.heading = [];

allVehId = unique(Sumo.DataRaw.vehicle_id); 

for iV = 1:numel(allVehId) 
    curId = allVehId{iV};
    temp = Sumo.DataRaw(strcmp(Sumo.DataRaw.vehicle_id, curId),:);
    
    if ~isempty(temp)
        time = temp.timestep_time;
        spd = temp.vehicle_speed;
        x = temp.vehicle_x;
        y = temp.vehicle_y;
        heading = temp.vehicle_angle*pi/180;
        
        if numel(time) > 1
            Sumo.Data.idStr = [Sumo.Data.idStr, {curId}];
            Sumo.Data.spd = [Sumo.Data.spd, interp1(time, spd, Sumo.Data.t)];
            Sumo.Data.x = [Sumo.Data.x, interp1(time, x, Sumo.Data.t)];
            Sumo.Data.y = [Sumo.Data.y, interp1(time, y, Sumo.Data.t)];
            Sumo.Data.heading = [Sumo.Data.heading, interp1(time, heading, Sumo.Data.t)];
        end

    end
end

%% get IPG data
Ipg.Data = cmread('coordMerge_sumo_simulink.erg');

%% plotting 
figure;
SUB_ROW = 2; SUB_COL = 3; subNum = 1; ax = [];
% set(gcf, 'position', ...
%     [1          41        1920        1083])
AxisPos = tightPlotAxes(SUB_ROW, SUB_COL, [0, 0, 0, 0]);
c = genColorCodes(); colorList = {c.b, c.r};
lineList = {'-', '--'};

%%% Preceding vehicle of ego
ind = find(contains(Sumo.Data.idStr, 'flow_0.9')); 
traLen = 4.89;


ax(subNum) = axes('OuterPosition', AxisPos(subNum, :));
subNum = subNum + 1; hold on
plot(Sumo.Data.t, Sumo.Data.spd(:,ind));
plot(Ipg.Data.Time.data, Ipg.Data.Traffic_RS_C009_LongVel.data )
ylim([0 60]/3.6)
grid on

ax(subNum) = axes('OuterPosition', AxisPos(subNum, :));
subNum = subNum + 1; hold on
plot(Sumo.Data.t, Sumo.Data.x(:,ind));
plot(Ipg.Data.Time.data, Ipg.Data.Traffic_RS_C009_tx.data + interp1(Sumo.Data.t, traLen*sin(Sumo.Data.heading(:,ind)), Ipg.Data.Time.data))
grid on

ax(subNum) = axes('OuterPosition', AxisPos(subNum, :));
subNum = subNum + 1; hold on
plot(Sumo.Data.t, Sumo.Data.y(:,ind));
plot(Ipg.Data.Time.data, Ipg.Data.Traffic_RS_C009_ty.data + interp1(Sumo.Data.t, traLen*cos(Sumo.Data.heading(:,ind)), Ipg.Data.Time.data))
grid on

%%% ego
ind = find(contains(Sumo.Data.idStr, 'egoCm')); 

ax(subNum) = axes('OuterPosition', AxisPos(subNum, :));
subNum = subNum + 1; hold on
plot(Sumo.Data.t, Sumo.Data.spd(:,ind));
plot(Ipg.Data.Time.data, Ipg.Data.Vhcl_v.data)
ylim([0 60]/3.6)
grid on

ax(subNum) = axes('OuterPosition', AxisPos(subNum, :));
subNum = subNum + 1; hold on
plot(Sumo.Data.t, Sumo.Data.x(:,ind));
plot(Ipg.Data.Time.data, 2*Ipg.Data.Vhcl_PoI_x.data-Ipg.Data.Vhcl_Fr1_x.data)
grid on

ax(subNum) = axes('OuterPosition', AxisPos(subNum, :));
subNum = subNum + 1; hold on
plot(Sumo.Data.t, Sumo.Data.y(:,ind));
plot(Ipg.Data.Time.data, 2*Ipg.Data.Vhcl_PoI_y.data-Ipg.Data.Vhcl_Fr1_y.data)
grid on

linkaxes(ax, 'x')
