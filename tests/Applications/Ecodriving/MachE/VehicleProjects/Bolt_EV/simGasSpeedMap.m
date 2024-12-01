%clear;
%Speed14dof = [];
%Speed7dof = [];
Speed1dof = [];
hws = get_param(bdroot, 'modelworkspace');
for gas = linspace(0,0.4,100)
%for gas = [0.2]
    hws.assignin('Gas', gas);
    simout = sim('EvBoltCalib_Lat_v1_comb_testEng');
    %open_system('EvBoltCalib_Lat_v1_comb_openControl');
    %set_param(gcs, 'SimulationCommand','start');
    disp(gas);
    %set_param(gcs,'SimulationCommand','stop');

    %Speed14dof = [Speed14dof; simout.Spd(end,1)];
    %Speed7dof = [Speed7dof; simout.Spd(end,1)];
    Speed1dof = [Speed1dof; simout.Spd(end,1)];
end
