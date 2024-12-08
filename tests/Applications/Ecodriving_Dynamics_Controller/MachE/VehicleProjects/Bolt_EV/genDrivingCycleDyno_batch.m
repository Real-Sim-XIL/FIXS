clear all

%%
FileList = dir(fullfile('./TestScenario/Bolt_Dyno_data/','*.csv'));
nFile = numel(FileList);

%%

for iFile = 1:nFile;
    fprintf('processing %d of %d files.....\n', iFile, nFile)
    
    clearvars -except FileList iFile nFile
    fileName = FileList(iFile).name;
    saveName = ['Bolt ', fileName];
    
    [num, ~, ~] = xlsread(fileName);
    
    MPH_2_MPS = 0.44704;
    KPH_2_MPS = 1000/3600;
    MILE_2_METER = 1609.33999997549;
    
    t = num(:,22);
    ind = find(t>=0);
    % ind = ind(1:102500);
    t = t(ind);
    dynoSpdMph = num(ind,3);
    distMile = num(ind,6);
    dist = distMile*MILE_2_METER;
    dist = dist-dist(1);
    
    dynoTracFrc = num(ind,4);
    dynoLoadFrc = num(ind,5);
    
    dynoSpdFrontMph = num(ind,7);
    dynoTracFrcFront = num(ind,8);
    dynoLoadFrcFront = num(ind,9);
    
    dynoSpdRearMph = num(ind,10);
    dynoLoadFrcRear = num(ind,11);
    dynoTracFrcRear = num(ind,12);
    
    gradePct = num(ind,13);
    gradeRad = atan(gradePct);
    
    battVolt = num(ind,54);
    battCur = num(ind,55);
    battPwrKw = num(ind,56);
    
    if size(num,2) >= 62
        axleTrq = num(ind,62);
        brakePos = num(ind,63);
        brakeTrq = num(ind,65);
        
        battSoc = num(ind,73);
        battSocDis = num(ind,74);
        
        pedalPos2 = num(ind,76);
        pedalPos = num(ind,77);
    else
        axleTrq = zeros(size(t));
        brakePos = zeros(size(t));
        brakeTrq = zeros(size(t));
        
        battSoc = zeros(size(t));
        battSocDis = zeros(size(t));
        
        pedalPos2 = zeros(size(t));
        pedalPos = zeros(size(t));
    end
    
%     axleTrqReq = num(ind,81);
%     
%     whlSpdKphFL = num(ind,87);
%     whlSpdKphFR = num(ind,88);
%     whlSpdKphRL = num(ind,89);
%     whlSpdKphRR = num(ind,90);
%     
%     whlSpdFL = whlSpdKphFL*KPH_2_MPS;
%     whlSpdFR = whlSpdKphFR*KPH_2_MPS;
%     whlSpdRL = whlSpdKphRL*KPH_2_MPS;
%     whlSpdRR = whlSpdKphRR*KPH_2_MPS;
    
    dynoSpdFront = dynoSpdFrontMph*MPH_2_MPS;
    dynoSpdRear = dynoSpdRearMph*MPH_2_MPS;
    dynoSpd = dynoSpdMph*MPH_2_MPS;

    
    %% check Hioki battery data VS HVBatt data
    Hioki.volt = num(ind,54);
    Hioki.cur = num(ind,55);
    Hioki.pwr = num(ind,56);
    HV.volt = num(ind,75);
    HV.curHi = num(ind,67);
    HV.curLow = num(ind,68);
    
    %%% these two volt pretty much the same
%     figure;
%     hold on
%     plot(Hioki.volt);
%     plot(HV.volt);
        
    %%% Hioki current is inverse of HV current
%     figure;
%     hold on
%     plot(Hioki.cur);
%     plot(-HV.curHi);
%     plot(HV.curLow);
    
%     figure;
%     hold on
%     plot(Hioki.cur.*Hioki.volt);
%     plot(-HV.curHi.*HV.volt);
% %     plot(axleTrq.*dynoSpd/0.3215)
%     plot(axleTrq.*(num(ind,87)+num(ind,88))/2)

    %%
    RefData.Name = saveName;
    RefData.time = t;
    RefData.accelPedalCmd = (pedalPos+pedalPos2)/200;
    RefData.brakePedalCmd = brakePos/100;
    RefData.accelPedalDBW = zeros(size(RefData.time));
    RefData.brakePedalDBW = zeros(size(RefData.time));
    RefData.vehSpd = dynoSpd;
    RefData.battVolt = battVolt;
    RefData.battSoc = battSoc/100;
    RefData.battCurr = battCur;
    RefData.axleTorq = axleTrq;
    RefData.grade = gradeRad;
    RefData.brakeTorq = brakeTrq;
    RefData.spare3 = zeros(size(RefData.time));
    RefData.spare4 = zeros(size(RefData.time));
    RefData.spare5 = zeros(size(RefData.time));
    RefData.spare6 = zeros(size(RefData.time));
    InitData.VehSpd = RefData.vehSpd(1);
    InitData.AxleTorq = RefData.axleTorq(1);
    if RefData.battSoc(1)<0.1
        InitData.BattSoc = 0.9;
    else
        InitData.BattSoc = RefData.battSoc(1);
    end
    InitData.BattCurr = RefData.battCurr(1);
    
    %% driving cycle
    DdDriveCycle = Simulink.data.dictionary.open('DdDriveCycleSrc.sldd');
    DdSection = DdDriveCycle.getSection('Design Data');
    
    CurCycle = getValue(getEntry(DdSection, 'TestScnrioDriveCycleStructs'));
    indRefCycle = numel(CurCycle)+1;
    for i = 1:numel(CurCycle)
        curName = CurCycle(i).Name;
        if strcmpi(curName, saveName)
            indRefCycle = i;
            break
        end
    end
    
    if numel(CurCycle) < indRefCycle
        CurCycle = [CurCycle CurCycle(end)];
    end
    
    CurCycle(indRefCycle).Name = saveName;
    CurCycle(indRefCycle).VelRef.Bpts = RefData.time;
    CurCycle(indRefCycle).VelRef.Tbl = RefData.vehSpd;
    
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
% %%
% spd = dynoSpd;
% save('CalibBolt_61910021', 't', 'spd')