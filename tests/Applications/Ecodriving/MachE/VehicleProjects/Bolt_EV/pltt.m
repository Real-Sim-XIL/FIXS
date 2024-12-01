%close all;
figure, hold on; grid on;
plot(out.tout, out.rotoOmega1(:,1), 'b'); plot(out.tout, out.rotoOmega1(:,3), 'r');
xlabel('Time (s)'); ylabel('RotoTest speed (rad/s)');
legend('Front', 'Rear');

figure, hold on; grid on;
plot(out.tout, out.Omega(:,1), 'b');
plot(out.tout, out.rotoOmega1(:,3), 'r');
xlabel('Time (s)'); ylabel('Wheel speed (rad/s)');
legend('Vehicle dynamics', 'RotoTest');

figure, hold on; grid on;
plot(out.tout, out.Speed, 'r--');
plot(out.tout, out.Omega(:,1)*9.5492968*0.3135*2*pi/60, 'b');
plot(out.tout, out.rotoOmega1(:,3)*9.5492968*0.3135*2*pi/60, 'r');
xlabel('Time (s)'); ylabel('Wheel speed (rad/s)');
legend('Veh Speed', 'Vehicle dynamics', 'RotoTest');
%legend('Dynamics model', 'RotoTest');