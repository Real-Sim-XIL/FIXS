clear all

fileName = 'Bolt 61910021 Test Data_ACM_Testing_RAW';
load(fileName)

%%
RefData.Name = fileName;
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
RefData.grade = zeros(size(RefData.time));
RefData.brakeTorq = zeros(size(RefData.time));
RefData.spare3 = zeros(size(RefData.time));
RefData.spare4 = zeros(size(RefData.time));
RefData.spare5 = zeros(size(RefData.time));
RefData.spare6 = zeros(size(RefData.time));
InitData.VehSpd = RefData.vehSpd(1);
InitData.AxleTorq = RefData.axleTorq(1);
InitData.BattSoc = RefData.battSoc(1);
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
CurExpRefStructs(indRefCycle) = RefData;
CurInitStates(indRefCycle) = InitData;

setValue(getEntry(DdSection, 'ExpRefStructs'), CurExpRefStructs);
setValue(getEntry(DdSection, 'InitStates'), CurInitStates);
saveChanges(DdDriveCycle)

% %%
% spd = dynoSpd;
% save('CalibBolt_61910021', 't', 'spd')