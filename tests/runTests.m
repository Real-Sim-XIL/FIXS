clear all

%% initialize testsResult.log
fileID = fopen('testsResults.log','w');

fprintf(fileID, "=============================================\n");
fprintf(fileID, "===> Run Tests At %s\n", datestr(now, 'yyyy/mm/dd/ HH:MM:SS'));
fprintf(fileID, "=============================================\n");

fclose(fileID);

%% compile codes
[status, cmdout] = system('compileCodes.bat','-echo');
if status == -1
    fprintf("\n\n=============================================\n");
    fprintf('ERROR: compiling code failed!\n');
    fprintf("=============================================\n");
    return
end

%% Run CoordMerge
run(fullfile('.\CoordMerge', 'runMatlabClient_VISSIM.m'));
run(fullfile('.\CoordMerge', 'runMatlabClient_SUMO.m'));

%% Run SpeedLimit
run(fullfile('.\SpeedLimit', 'runSpeedLimit_test1.m'));
run(fullfile('.\SpeedLimit', 'runSpeedLimit_test2.m'));
run(fullfile('.\SpeedLimit', 'runSpeedLimit_test3.m'));

%% Run MultipleVissim
cd('.\MultipleVissim')
system(fullfile('runRealSim.bat'));
cd('..\')

%% Run SumoDriver
run(fullfile('.\SumoDriver', 'runSUMOdriver.m'));

%% clean up
close all

fileID = fopen('testsResults.log','a');
fprintf(fileID, "All tests done\n");
fprintf(fileID, "=============================================\n");
fclose(fileID);

