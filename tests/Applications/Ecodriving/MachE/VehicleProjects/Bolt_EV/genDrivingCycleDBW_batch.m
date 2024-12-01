clear all

%%
FileList = dir(fullfile('./TestScenario/Bolt_DBW_data/**/','*.mat'));
nFile = numel(FileList);

%%
% fileName = 'Bolt_30-APP_Request_to_DBW_System_10ms';
% fileName = 'Bolt_30-BPP_Request_to_DBW_System_10ms';

for iFile = 1:nFile
    fprintf('processing %d of %d files.....\n', iFile, nFile)
    
    clearvars -except FileList iFile nFile
    fileName = FileList(iFile).name;
    
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
    RefData.grade = zeros(size(Time));
    RefData.brakeTorq = zeros(size(Time));
    RefData.spare3 = zeros(size(Time));
    RefData.spare4 = zeros(size(Time));
    RefData.spare5 = zeros(size(Time));
    RefData.spare6 = zeros(size(Time));
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
    if ~isequal(fieldnames(CurExpRefStructs), fieldnames(RefData))
       CurExpRefStructs = cell2struct(cell(numel(fieldnames(RefData)),1),fieldnames(RefData));
    end
    CurExpRefStructs(indRefCycle) = RefData;
    CurInitStates(indRefCycle) = InitData;
    
    setValue(getEntry(DdSection, 'ExpRefStructs'), CurExpRefStructs);
    setValue(getEntry(DdSection, 'InitStates'), CurInitStates);
    saveChanges(DdDriveCycle)
    
end