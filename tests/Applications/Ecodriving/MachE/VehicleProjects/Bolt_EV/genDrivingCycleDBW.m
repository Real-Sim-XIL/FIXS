clear all

%%
% fileName = 'Bolt_30-APP_Request_to_DBW_System_10ms';
% fileName = 'Bolt_30-BPP_Request_to_DBW_System_10ms'; 

load(fileName); % load the required .mat file
drv_accel_dmd_simu =(test_data{2,1})/100; % (0, 1)
drv_brk_dmd_simu = (test_data{2,2})/100; % (0, 1)
Axle_torque = (test_data{2,3}); 
DBW_Accel_Pedal_Position_Feedback = (test_data{2,4})/100;
DBW_brake_Pedal_Position_Feedback = (test_data{2,5})/100;
Battery_Current = (test_data{2,6});
veh_spd = (test_data{2,7});
veh_spd = veh_spd/3.6; % convert speed from kph to m/s
Time = (test_data{2,8});

battSoc = zeros(size(Time));
battVolt = zeros(size(Time));

spare1 = zeros(size(Time));
spare2 = zeros(size(Time));
spare3 = zeros(size(Time));
spare4 = zeros(size(Time));
spare5 = zeros(size(Time));
spare6 = zeros(size(Time));

RefData.Name = fileName;
RefData.time = Time;
RefData.accelPedalCmd = drv_accel_dmd_simu;
RefData.brakePedalCmd = drv_brk_dmd_simu;
RefData.accelPedalDBW = DBW_Accel_Pedal_Position_Feedback;
RefData.brakePedalDBW = DBW_brake_Pedal_Position_Feedback;
RefData.vehSpd = veh_spd;
RefData.battVolt = battVolt;
RefData.battSoc = battSoc;
RefData.battCurr = Battery_Current;
RefData.axleTorq = Axle_torque;
RefData.spare1 = spare1;
RefData.spare2 = spare2;
RefData.spare3 = spare3;
RefData.spare4 = spare4;
RefData.spare5 = spare5;
RefData.spare6 = spare6;
InitData.VehSpd = RefData.vehSpd(1);
InitData.AxleTorq = RefData.axleTorq(1);
InitData.BattSoc = 0.9;
InitData.BattCurr = RefData.battCurr(1);

%% driving cycle
DdDriveCycle = Simulink.data.dictionary.open('DdDriveCycleSrc.sldd');
DdSection = DdDriveCycle.getSection('Design Data');

CurCycle = getValue(getEntry(DdSection, 'TestScnrioDriveCycleStructs'));
indRefCycle = numel(CurCycle)+1;
for i = 1:numel(CurCycle)
    curName = CurCycle(i).Name;
    if strcmpi(curName, fileName)
        indRefCycle = i;
        break
    end
end

if numel(CurCycle) < indRefCycle
    CurCycle = [CurCycle CurCycle(end)];
end

CurCycle(indRefCycle).Name = fileName;
CurCycle(indRefCycle).VelRef.Bpts = Time;
CurCycle(indRefCycle).VelRef.Tbl = veh_spd;

setValue(getEntry(DdSection, 'TestScnrioDriveCycleStructs'), CurCycle);
saveChanges(DdDriveCycle)

%% Experimental Reference Sources
DdDriveCycle = Simulink.data.dictionary.open('DdExpRefSrc.sldd');
DdSection = DdDriveCycle.getSection('Design Data');

CurExpRefStructs = getValue(getEntry(DdSection, 'ExpRefStructs'));
CurInitStates = getValue(getEntry(DdSection, 'InitStates'));
% !!!! use 'indRefCycle', make sure both data field has the same index
CurExpRefStructs(indRefCycle) = RefData;
CurInitStates(indRefCycle) = InitData;

setValue(getEntry(DdSection, 'ExpRefStructs'), CurExpRefStructs);
setValue(getEntry(DdSection, 'InitStates'), CurInitStates);
saveChanges(DdDriveCycle)

% % % %% temp
% % % for i = 1:8
% % %     RefData.Name = CurCycle(i).Name;
% % %     RefData.time = CurCycle(i).VelRef.Bpts;
% % %     RefData.accelPedalCmd = zeros(size(RefData.time));
% % %     RefData.brakePedalCmd = zeros(size(RefData.time));
% % %     RefData.accelPedalDBW = zeros(size(RefData.time));
% % %     RefData.brakePedalDBW = zeros(size(RefData.time));
% % %     RefData.vehSpd = zeros(size(RefData.time));
% % %     RefData.battVolt = zeros(size(RefData.time));
% % %     RefData.battSoc = zeros(size(RefData.time));
% % %     RefData.battCurr = zeros(size(RefData.time));
% % %     RefData.axleTorq = zeros(size(RefData.time));
% % %     RefData.spare1 = zeros(size(RefData.time));
% % %     RefData.spare2 = zeros(size(RefData.time));
% % %     RefData.spare3 = zeros(size(RefData.time));
% % %     RefData.spare4 = zeros(size(RefData.time));
% % %     RefData.spare5 = zeros(size(RefData.time));
% % %     RefData.spare6 = zeros(size(RefData.time));
% % %     RefData.vehSpdInit = RefData.vehSpd(1);
% % %     RefData.axleTorqInit = RefData.axleTorq(1);
% % %     RefData.battSocInit = RefData.battSoc(1);
% % %     RefData.battCurrInit = RefData.battCurr(1);
% % %     CurExpRefStructs(i) = RefData;
% % % end

% % % %% ONLY DO THIS ONCE
% % % NewDataName = 'DdExpRefSrc.sldd';
% % % DdRefObj = Simulink.data.dictionary.create(NewDataName);
% % % DdRefSecObj = DdRefObj.getSection('Design Data');
% % % 
% % % addEntry(DdRefSecObj, 'ExpRefStructs', RefData);
% % % addEntry(DdRefSecObj, 'InitStates', InitData);
% % % 
% % % saveChanges(DdRefObj)
% % % Simulink.data.dictionary.closeAll
% % % %%
% % % aa = Simulink.data.dictionary.open('DdPassengerEv.sldd');
% % % addDataSource(aa, 'DdExpRefSrc.sldd')
% % % saveChanges(aa)
% % % Simulink.data.dictionary.closeAll

%%
% aa = Simulink.data.dictionary.open('DdPassengerEv.sldd');
% saveChanges(aa)

% %%
% NewDataName = 'Dd_MTU_Meas.sldd';
% DdRefObj = Simulink.data.dictionary.create(NewDataName);
% DdRefSecObj = DdRefObj.getSection('Design Data');
% 
% addEntry(DdRefSecObj, 'drv_accel_dmd_simu', drv_accel_dmd_simu);
% addEntry(DdRefSecObj, 'drv_brk_dmd_simu', drv_brk_dmd_simu);
% addEntry(DdRefSecObj, 'Axle_torque', Axle_torque);
% addEntry(DdRefSecObj, 'DBW_Accel_Pedal_Position_Feedback', DBW_Accel_Pedal_Position_Feedback);
% addEntry(DdRefSecObj, 'DBW_brake_Pedal_Position_Feedback', DBW_brake_Pedal_Position_Feedback);
% addEntry(DdRefSecObj, 'Battery_Current', Battery_Current);
% addEntry(DdRefSecObj, 'veh_spd', veh_spd);
% addEntry(DdRefSecObj, 'Time', Time);
% 
% saveChanges(DdRefObj)
% Simulink.data.dictionary.closeAll

% aa = Simulink.data.dictionary.open('DdPassengerEv.sldd');
% % DdRefObj = Simulink.data.dictionary.open(NewDataName);
% % bb = aa.getSection('Design Data');
% addDataSource(aa, NewDataName)
% saveChanges(aa)
% Simulink.data.dictionary.closeAll

%%
% DdRefObj = Simulink.data.dictionary.open('Dd_MTU_Meas.sldd');
% DdRefSecObj = DdRefObj.getSection('Design Data');
% addEntry(DdRefSecObj, 'drv_accel_dmd_simu', drv_accel_dmd_simu);
% addEntry(DdRefSecObj, 'drv_brk_dmd_simu', drv_brk_dmd_simu);
% addEntry(DdRefSecObj, 'Axle_torque', Axle_torque);
% addEntry(DdRefSecObj, 'DBW_Accel_Pedal_Position_Feedback', DBW_Accel_Pedal_Position_Feedback);
% addEntry(DdRefSecObj, 'DBW_brake_Pedal_Position_Feedback', DBW_brake_Pedal_Position_Feedback);
% addEntry(DdRefSecObj, 'Battery_Current', Battery_Current);
% addEntry(DdRefSecObj, 'veh_spd', veh_spd);
% addEntry(DdRefSecObj, 'Time', Time);
% saveChanges(DdRefObj)