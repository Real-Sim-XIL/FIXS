function TestResults = runAllVsrUtilTests(ResultsFolder)
%% Set file results path
if nargin < 1
    ResultsFolder = tempname;
else
    if isempty(ResultsFolder)
        ResultsFolder = tempname;
    end
end
ResultsFolder = fullfile(ResultsFolder, 'VsrUtilTestResults');
makeResultsFolder(ResultsFolder);
TestReportFolder = fullfile(ResultsFolder, 'TestReport');
makeResultsFolder(TestReportFolder);
CoverageReportFolder = fullfile(ResultsFolder, 'CodeCoverageReport');
makeResultsFolder(CoverageReportFolder);
SimulinkTestFileName = fullfile(ResultsFolder, 'VsrSimTestResults.mldatx');

%% Get tests
% Matlab Unit tests
UtilTestSuite = testsuite([fileparts(which(mfilename)), filesep], 'IncludeSubfolders', true);
TestRunner = matlab.unittest.TestRunner.withTextOutput;

% Report plugins
TestRunner.addPlugin(matlab.unittest.plugins.CodeCoveragePlugin.forPackage('vsr',...
    'IncludingSubpackages',true, 'Producing', matlab.unittest.plugins.codecoverage.CoverageReport(CoverageReportFolder)))
TestRunner.addPlugin(matlab.unittest.plugins.TestReportPlugin.producingHTML(TestReportFolder, ...
    'LoggingLevel', 4, 'IncludingCommandWindowText', true, 'IncludingPassingDiagnostics', true));

%% Run tests
try 
    TestResults = TestRunner.run(UtilTestSuite);
catch e
    dis(getReport(e, 'extended'))
end

%% Simulink test results
SimulinkTestResults = sltest.testmanager.getResultSets;
SimulinkTestResults = SimulinkTestResults(end);
sltest.testmanager.exportResults(SimulinkTestResults, SimulinkTestFileName)

%% Results location
fprintf('Results have been saved in ''%s''\n', ResultsFolder)

end
%% makeResultsFolder
function makeResultsFolder(FolderName)
    if exist(FolderName, 'dir') ~= 7
        mkdir(FolderName);
    end
end