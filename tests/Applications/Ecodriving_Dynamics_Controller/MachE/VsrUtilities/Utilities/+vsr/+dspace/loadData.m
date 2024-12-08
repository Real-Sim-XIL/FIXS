function [TestData, SignalNames, FileName, SignalPaths] = loadData(varargin)
% TestData = loadData() allows user to select .mat file exported from dSpace
%   ControlDesk and returns data TestData as timeseries cell array
%
% TestData = loadData(FilePath) allows user to select file from file path
%   to load data
%
% TestData = loadData(FileName) loads data contained in FileName
% 
% TestData = loadData(FileName, ReturnStruct) loads data contained in FileName
%   and outputs TestData as structure
%   The signal hierarchy can be maintained according to model and dSpace 
%   structure by setting the second argument (KeepSignalTree) to true. 


if nargin < 1
    [FileName,FilePath,indx] = uigetfile('*.mat');
    if indx ~= 1
        return;
    end
    FullFileName = fullfile(FilePath, FileName);
elseif isempty(varargin{1})
    [FileName,FilePath,indx] = uigetfile('*.mat');
    if indx ~= 1
        return;
    end
    FullFileName = fullfile(FilePath, FileName); 
else
    if isdir(varargin{1})
        [FileName,FilePath,indx] = uigetfile(varargin{1});
        if indx ~= 1
            return;
        end
        FullFileName = fullfile(FilePath, FileName);
    else
        FullFileName = varargin{1};
    end
    
end

Data = load(FullFileName);
DataFieldNames = fieldnames(Data);
DataName = DataFieldNames{1};
Data = Data.(DataName);
[~,FileName] = fileparts(FullFileName);

%% Create time series array from data
XDataRaster = {Data.X.Raster};

for i = 1:length(Data.Y)
    TimeArray = Data.X(strcmp(Data.Y(i).Raster, XDataRaster)).Data;
    TimeSeriesData{i} = timeseries(Data.Y(i).Data, TimeArray);
    SignalNames{i} = Data.Y(i).Name;
    SignalPaths{i} = Data.Y(i).Path;
    TimeSeriesData{i}.Name = Data.Y(i).Name;
end

%% Arrange in tree form if option selected
if nargin < 2
    ReturnStruct = false;
else
    ReturnStruct = varargin{2};
end

if ReturnStruct
    AllPaths = {Data.Y.Path};
    TestData = createDataStruct(TimeSeriesData, AllPaths);
    TestData = reduceStruct(TestData);
    SignalNames = {'dSpaceData'};
else
    TestData = TimeSeriesData;
end

end

%% createDataStruct
function StructOut = createDataStruct(TimeSeriesData, Paths)
    % Set up
    
    FirstPathName = cell(size(Paths));
    RemainingPathName = FirstPathName;
    
    % Get current path names
    for i = 1:length(Paths)
        StrLoc = strfind(Paths{i}, '/');
        if ~isempty(StrLoc)
            FirstPathName{i} = Paths{i}(1:(StrLoc-1));
            RemainingPathName{i} = Paths{i}((StrLoc+1):end);
        else
            if ~isempty(Paths{i})
                FirstPathName{i} = Paths{i};
            else
                FirstPathName{i} = '';
            end
        end
    end
    [UniquePathName, ~, UniqueDataIdx] = unique(FirstPathName);
    UniquePathName = cellstr(UniquePathName);
    
    for i = 1:length(UniquePathName)
        if isempty(UniquePathName{i})
            LeafFieldData = TimeSeriesData(UniqueDataIdx == i);
            for j = 1:length(LeafFieldData)
                CurrFieldName = matlab.lang.makeValidName(LeafFieldData{j}.Name);
                StructOut.(CurrFieldName) = LeafFieldData{j};
            end
        else
            CurrTimeSeriesData = TimeSeriesData(UniqueDataIdx == i);
            CurrPath = RemainingPathName(UniqueDataIdx == i);
            CurrFieldName = matlab.lang.makeValidName(UniquePathName{i});
            StructOut.(CurrFieldName) = createDataStruct(CurrTimeSeriesData, CurrPath);
        end
        
    end
end

%% reduceStruct
function StructOut = reduceStruct(StructIn)

CurrFieldNames = cellstr(fieldnames(StructIn));

if length(CurrFieldNames) == 1
    if isstruct(StructIn.(CurrFieldNames{1}))
        CurrStruct = StructIn.(CurrFieldNames{1});
        StructOut = reduceStruct(CurrStruct);
    else
        StructOut = StructIn;
    end
else
    for i = 1:length(CurrFieldNames)
        if isstruct(StructIn.(CurrFieldNames{i}))
            StructOut.(CurrFieldNames{i}) = reduceStruct(StructIn.(CurrFieldNames{i}));
        else
            StructOut.(CurrFieldNames{i}) = StructIn.(CurrFieldNames{i});
        end
    end
end

end