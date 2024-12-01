function DriveCycleObjs = getVsrDriveCycles()
%getVsrDriveCycles returns drive cycles contained in the
%'VsrDriveCycleData' folder in the form of drive cycle objects. Note that
%the function does not load all files in drive cycle folder. The function
%will need to be updated to support additional drive cycles.

%% Setup
idx = 1;

%% ARB Transient cycle
DriveCycleObjs(idx) = loadAutonomieFormat('CARB_HHDDT_Transient.mat');

%% World harmonized transient vehicle cycle
idx = idx + 1;
DriveCycleObjs(idx) = loadAutonomieFormat('UNECE_WHVC.mat');

%% HTUF6 cycle
idx = idx + 1;
DriveCycleObjs(idx) = loadAutonomieFormat('HTUF_Class6_Parcel.mat');

%% CILCC cycle
idx = idx + 1;
DriveCycleObjs(idx) = loadAutonomieFormat('NREL_CILCC.mat');

%% HHDDT 65 cycle with grade from Danville to Starkville route
idx = idx + 1;
DriveCycleObjs(idx) = loadAutonomieFormat('HHDDT65_DS_Final.mat');
DriveCycleObjs(idx).Name = 'HHDDT 65 cycle with grade from Danville to Starkville route';
%% HTDC drivecycle
idx = idx + 1;
DriveCycleObjs(idx) = loadAutonomieFormat('HTDC_cycle_Final.mat');
DriveCycleObjs(idx).Name = 'HTDC';

%% Cruise 55 and 65 GEM cycles
% load data and set ramp time
CruiseGrade_data = xlsread('EPA GEM Final Grade Profile.csv', 'EPA GEM Final Grade Profile', 'A2:B300');
CruiseGrade_data = table(CruiseGrade_data(:,1), CruiseGrade_data(:,2));
% Grade = CruiseGrade_data{:,1}
% Grade = Grade{:}
% CruiseGrade_data = 
SS_Cycle_RampUpTime = 120; %[s]
SS_Cycle_SettleTime = 30; %[s]
GradeTriggerTime = SS_Cycle_RampUpTime+SS_Cycle_SettleTime;
Cycle_distance = CruiseGrade_data{end, 1};

% 55 mph
idx = idx + 1;
NewCycle = vsr.drivecycle.DriveCycle;
NewCycle.Name = 'EPA GEM profile with 55 mph cruise';
Cruise55mph_EPAFinalGradeProfile_cycle_time=[0; SS_Cycle_RampUpTime;SS_Cycle_RampUpTime+SS_Cycle_SettleTime+Cycle_distance/(55*1609.34/3600)]; %
Cruise55mph_EPAFinalGradeProfile_cycle_spd=[0; 55; 55]*1609.34/3600; % Test environment expects m/s
Cruise55mph_EPAFinalGradeProfile_cycle_distance=CruiseGrade_data{:,1}; %[m]
Cruise55mph_EPAFinalGradeProfile_cycle_grade=CruiseGrade_data{:,2}; % [pct]

NewCycle.VelRef = table(Cruise55mph_EPAFinalGradeProfile_cycle_time(:), Cruise55mph_EPAFinalGradeProfile_cycle_spd(:), 'VariableNames', {'Time', 'Velocity'});
NewCycle.VelRef.Properties.VariableUnits = {'s', 'm/s'};

CruiseSpd = Cruise55mph_EPAFinalGradeProfile_cycle_spd(end);
GradeTriggerDist = (0+CruiseSpd)*SS_Cycle_RampUpTime/2 + SS_Cycle_SettleTime*CruiseSpd;

NewCycle.Grade = table([0;  GradeTriggerDist + Cruise55mph_EPAFinalGradeProfile_cycle_distance(:)],...
    [0; atan(Cruise55mph_EPAFinalGradeProfile_cycle_grade(:)/100)], 'VariableNames', {'Distance', 'Grade'});
NewCycle.Grade.Properties.VariableUnits = {'m', 'rad'};

DriveCycleObjs(idx) = NewCycle;

% 65 mph
idx = idx + 1;
NewCycle = vsr.drivecycle.DriveCycle;
NewCycle.Name = 'EPA GEM profile with 65 mph cruise';
Cruise65mph_EPAFinalGradeProfile_cycle_time=[0; SS_Cycle_RampUpTime;SS_Cycle_RampUpTime+SS_Cycle_SettleTime+Cycle_distance/(65*1609.34/3600)]; % 
Cruise65mph_EPAFinalGradeProfile_cycle_spd=[0;65; 65]*1609.34/3600; % Test environment expects m/s
Cruise65mph_EPAFinalGradeProfile_cycle_distance=CruiseGrade_data{:,1}; %[m]
Cruise65mph_EPAFinalGradeProfile_cycle_grade=CruiseGrade_data{:,2}; % [pct]

NewCycle.VelRef = table(Cruise65mph_EPAFinalGradeProfile_cycle_time(:), Cruise65mph_EPAFinalGradeProfile_cycle_spd(:), 'VariableNames', {'Time', 'Velocity'});
NewCycle.VelRef.Properties.VariableUnits = {'s', 'm/s'};

CruiseSpd = Cruise65mph_EPAFinalGradeProfile_cycle_spd(end);
GradeTriggerDist = (0+CruiseSpd)*SS_Cycle_RampUpTime/2 + SS_Cycle_SettleTime*CruiseSpd;

NewCycle.Grade = table([0;  GradeTriggerDist + Cruise65mph_EPAFinalGradeProfile_cycle_distance(:)],...
    [0; atan(Cruise65mph_EPAFinalGradeProfile_cycle_grade(:)/100)], 'VariableNames', {'Distance', 'Grade'});
NewCycle.Grade.Properties.VariableUnits = {'m', 'rad'};

DriveCycleObjs(idx) = NewCycle;

end

%% loadAutonomieFormat
function CycleObj = loadAutonomieFormat(FileName)

CycleData = load(FileName);
CycleData = CycleData.drive_cycle;
CycleObj = vsr.drivecycle.DriveCycle;
if ismember('name', fieldnames(CycleData))
    CycleObj.Name = CycleData.name;
end
CycleObj.VelRef = table(CycleData.cycle_time(:), CycleData.cycle_speed_mps(:), 'VariableNames', {'Time', 'Velocity'});
CycleObj.VelRef.Properties.VariableUnits = {'s', 'm/s'};

CycleObj.Grade = table(CycleData.grade_dist_m(:), atan(CycleData.grade_pct(:)/100), 'VariableNames', {'Distance', 'Grade'});
CycleObj.Grade.Properties.VariableUnits = {'m', 'rad'};
end



