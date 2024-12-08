clear all

% load('calib_v1_200318')
load('calib_v2')

%%
figure;
SUB_ROW = 4; SUB_COL = 1; subNum = 1; ax = [];
FONT_SIZE = 14;
LINE_WIDTH = 1;
if strcmpi(getenv('computername'), 'PC0120295')
    set(gcf, 'position', ...
        [1          41        1280        607.333333333333])
else
    set(gcf, 'position', ...
        [1          41        1920        1083])
end
AxisPos = tightPlotAxes(SUB_ROW, SUB_COL, [0.1, 0, 0.1, 0]);
c = genColorCodes(); colorList = {[1 0 0], [0 0.8 0], 0.2*ones(1,3)}; {c.o, c.b, c.y}; 
lineList = {'-', '-', '--'};

signalNames = {'Speed (m/s)', 'SOC', 'Axle Torque (Nm)', 'Battery Current (A)'};
ylimList = {[0 40], [0.85 0.97], [-1000 600], [-150, 250]};
for i = 1:4
    ax(subNum) = axes('OuterPosition', AxisPos(subNum, :));
    subNum = subNum + 1; hold on
    plot(out.EvBoltCalib.time, out.EvBoltCalib.signals.values(:,i), ...
        lineList{1}, 'color', colorList{1}, 'linewidth', LINE_WIDTH)
    plot(out.EvBoltCalib.time, out.EvBoltCalib.signals.values(:,i+5), ...
        lineList{2}, 'color', colorList{2}, 'linewidth', LINE_WIDTH)
%     plot(EV_Bolt.time ,EV_Bolt.signals.values(:,i), ...
%         lineList{3}, 'color', colorList{3}, 'linewidth', LINE_WIDTH)
    grid on
    xlabel('time (sec)','FontSize',FONT_SIZE,'interpreter','latex')
    ylabel(signalNames{i},'FontSize',FONT_SIZE,'interpreter','latex')
    set(gca, 'FontSize',FONT_SIZE,'ticklabelinterpreter','latex')
%     if i == 1
%         legend('Chassis Dyno Data', 'Calibrated VSRT Model', 'Calibrated CarMaker Model', 'location', 'north', 'orientation', 'horizontal','interpreter','latex')
%     end
%     if i == 2
%         aa = out.EvBoltCalib.signals.values(end,i);
%         bb = out.EvBoltCalib.signals.values(end,i+5);
%         cc = EV_Bolt.signals.values(end,i);
%         text(3800, 0.93, [sprintf('Dyno SOC final: %.4f\nVSRT SOC final: %.4f, error: %.4f', aa, bb, (aa-bb)/aa*100), ...
%             '\%', sprintf('\nCarMaker SOC final: %.4f, error: %.4f', cc, (aa-cc)/aa*100), ...
%             '\%'], 'interpreter','latex','FontSize',FONT_SIZE)
%     end
    ylim(ylimList{i})
    box on
end
linkaxes(ax, 'x')
