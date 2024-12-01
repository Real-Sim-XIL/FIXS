% data out of file : MOVES Low Speed Transient Cycle.xlsx
disp('MOVES Low Speed Transient Cycle.xlsx')


NREL_data=xlsread('MOVES Low Speed Transient Cycle.xlsx', 'MOVES Low Speed Transient Cycle', 'A1:B724');

if(size(NREL_data)>800)
    disp('Data might be missing!!!!')
end


figure
clf
plot(NREL_data(:,1),NREL_data(:,2),'b')
grid
ylabel('Vehicle speed [mph]')
title('NREL cycle. MOVES Low Speed Transient Cycle.xlsx')
xlabel('Time [s]')
%grid



