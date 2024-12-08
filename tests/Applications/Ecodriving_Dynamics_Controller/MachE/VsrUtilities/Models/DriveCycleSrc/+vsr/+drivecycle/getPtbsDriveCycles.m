function DriveCycleObjs = getPtbsDriveCycles
%GETPTBSDRIVECYCLES Returns Powertrain Blockset drivecycles
PtbsCycleFileLoc = [matlabshared.supportpkg.getSupportPackageRoot, '\toolbox\autoblks\autodata\drivecycledata'];
FileInfo = dir(PtbsCycleFileLoc);
FileNames = {FileInfo.name};
DriveCycleObjs = vsr.drivecycle.DriveCycle.empty;
for i = 1:length(FileNames)
    [~, CurrFileName, Ext] = fileparts(FileNames{i});
    if strcmp(Ext, '.mat') && length(CurrFileName) > 5
        if strcmp(CurrFileName(1:5), 'cycle')
            try
                NewCycle = loadPtbsCycleData(CurrFileName);
                DriveCycleObjs = [DriveCycleObjs, NewCycle];
            catch
                
            end
        end
    end
    
end
end

%% loadPtbsCycleData
        function obj = loadPtbsCycleData(FileName)
            % loadPtbsCycleData function loads cycle data from file
            obj = vsr.drivecycle.DriveCycle;
            
            PtbsCycleFileLoc = [matlabshared.supportpkg.getSupportPackageRoot, '\toolbox\autoblks\autodata\drivecycledata\'];
            FullFileName = [PtbsCycleFileLoc, FileName];
            FileData = load(FullFileName);
            DataNames = fieldnames(FileData);
            for i = 1:length(DataNames)
                CurrTimeSeries = FileData.(DataNames{i});
                TimeseriesName = CurrTimeSeries.Name;
                SpaceLoc = strfind(TimeseriesName, ' ');
                if SpaceLoc(end) == length(TimeseriesName)
                    SpaceLoc = SpaceLoc(1:end-1);
                end
                KeyWord = TimeseriesName((SpaceLoc(end)+1):end);
                % Read timeseries data
                if strcmpi(KeyWord, 'Velocity')
                    obj.Name = TimeseriesName(1:(SpaceLoc(end)-1));
                    obj.VelRef = table(CurrTimeSeries.Time, CurrTimeSeries.Data, 'VariableNames', {'Time', 'Velocity'});
                    obj.VelRef.Properties.VariableUnits = {CurrTimeSeries.TimeInfo.Units, CurrTimeSeries.DataInfo.Units};
                elseif strcmpi(KeyWord, 'Gear')
                    obj.Gear = table(CurrTimeSeries.Time, CurrTimeSeries.Data, 'VariableNames', {'Time', 'Gear'});
                    obj.Gear.Properties.VariableUnits = {CurrTimeSeries.TimeInfo.Units, CurrTimeSeries.DataInfo.Units};
                elseif strcmpi(KeyWord, 'Grade')
                    obj.Grade = table(CurrTimeSeries.Time, CurrTimeSeries.Data, 'VariableNames', {'Time', 'Grade'});
                    obj.Grade.Properties.VariableUnits = {CurrTimeSeries.TimeInfo.Units, CurrTimeSeries.DataInfo.Units};
                end
            end
            
            % Check velocity
            if isempty(obj.VelRef) || isempty(obj.Name)
                error('Velocity and cycle name not loaded correctly for ''%s''. Check file format.', FullFileName)
            end
        end