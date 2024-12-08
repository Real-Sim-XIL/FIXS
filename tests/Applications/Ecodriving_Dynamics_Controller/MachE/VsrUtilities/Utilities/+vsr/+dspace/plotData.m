function [SdiRunID, TestData] = plotData(varargin)
% plotData() allows user to select .mat file exported from dSpace
% ControlDesk and displays data in Simulink Data Inspector
%
% plotData(FilePath) allows user to select file from file path
%
% plotData(FileName) plots data contained in FileName

%% Load data
if nargin < 1
    FileInput = [];
else
    FileInput = varargin{1};
end


[TestData, SignalNames, FileName] = vsr.dspace.loadData(FileInput, true);
if ~iscell(TestData)
   TestData = {TestData}; 
end
%% Show in SDI

SdiRunID = Simulink.sdi.createRun(FileName, 'namevalue', SignalNames, TestData);
Simulink.sdi.view
end

