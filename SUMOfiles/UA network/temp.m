temp = load('UA_veh0_2_tf0p05_3');
Orig = temp.VehicleOut;
Mod = VehicleOut;

figure; 
plot(Orig.spdDes.time, Orig.spdDes.signals.values(:));
hold on
plot(Mod.spdDes.time, Mod.spdDes.signals.values(:));
plot(Mod.spdAct.time, Mod.spdAct.signals.values(:));
% plot(Mod.spdDesInterp.time, Mod.spdDesInterp.signals.values(:));
legend('orig', 'des', 'act')
grid on