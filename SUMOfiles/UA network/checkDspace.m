%% check results
temp = load('UA_veh0_2_orig');
Orig = temp.Veh;

temp = load('SUMOdriverRec');
Mod = temp.SUMOdriverRec;

% %%
% figure; 
% hold on
% plot(Mod.X.Data, Mod.Y(5).Data);

%%
ind = find(Mod.X.Data >= 0 & Mod.X.Data <= 70);

figure; 
plot(Orig.t, Orig.speed);
hold on
plot(Mod.Y(5).Data(ind), Mod.Y(4).Data(ind));
plot(Mod.Y(5).Data(ind), Mod.Y(9).Data(ind));
% plot(Mod.spdDesInterp.time, Mod.spdDesInterp.signals.values(:));
legend('orig', 'des', 'act')
grid on


%% check results
temp = load('UA_veh0_2_tf0p05_4');
Orig = temp.VehicleOut;

temp = load('SUMOdriverRec5');
Mod = temp.SUMOdriverRec;

t = Mod.X.Data;
timeReceive = Mod.Y(3).Data;
speedReceive = Mod.Y(4).Data;
speedInterp = Mod.Y(5).Data;
timeSimulator = Mod.Y(6).Data;
timeStepSimulator = Mod.Y(7).Data;
timeStepTrigger = Mod.Y(8).Data;
speedSend = Mod.Y(10).Data;

% ind = find(Mod.X.Data >= 105 & Mod.X.Data <= 170);
ind = find(timeSimulator >= 0);

figure; 
hold on
plot(Orig.spdDes.time, Orig.spdDes.signals.values(:));
plot(timeSimulator, speedReceive);
plot(timeSimulator, speedSend);
% plot(Mod.spdDesInterp.time, Mod.spdDesInterp.signals.values(:));
legend('orig', 'des', 'act')
grid on



%%
temp = load('SUMOdriverRec2');
Mod = temp.SUMOdriverRec;

timeReceive = Mod.Y(3).Data;
speedReceive = Mod.Y(4).Data;
speedInterp = Mod.Y(5).Data;
timeSimulator = Mod.Y(6).Data;
timeStepSimulator = Mod.Y(7).Data;
timeStepTrigger = Mod.Y(8).Data;
speedSend = Mod.Y(10).Data;

t = Mod.X.Data;
% t = timeSimulator;

figure;
hold on
plot(t, timeReceive);
plot(t, timeSimulator);
plot(t, timeStepSimulator);
plot(t, timeStepTrigger);
grid on

yyaxis right
hold on
plot(t, speedReceive);
plot(t, speedInterp);
plot(t, speedSend);
% legend('speedReceive', 'speedInterp', 'speedSend')

legend('timeReceive', 'timeSimulator', 'timeStepSimulator', 'timeStepTrigger', 'speedReceive', 'speedInterp', 'speedSend')

%% delay calc
tDelay = Mod.Y(9).Data;

aa=t(diff(timeReceive)>1e-5); bb = t(diff(timeStepSimulator)>1e-5);
figure; 
hold on
plot(bb, aa(3:end)-bb)
plot(t, tDelay)

cc = find(diff(timeReceive)>1e-5)+1; 
figure; 
hold on
plot(t(diff(timeStepSimulator)>1e-5), timeSimulator(cc(3:end))+0.2-timeReceive(cc(3:end)))
plot(t, tDelay)

