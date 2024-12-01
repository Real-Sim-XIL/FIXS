classdef DriveCycle < matlab.mixin.Copyable &  matlab.mixin.Heterogeneous
    % DriveCycle class contains timeseries properties for longitudinal drive
    % cycle and methods for loading and displaying values.
    
    %% Public properties
    properties
        Name    char
        VelRef  table
        Grade   table
        Gear    table
    end
    
    %% Public methods
    methods
        
        %         %% plotCycleData
        %         function plotCycleData(obj, FigHdl)
        %             % plotCycleData plot cycle data in single figure
        %             if nargin < 2
        %                 FigHdl = figure;
        %             end
        %             set(0, 'CurrentFigure', FigHdl)
        %             if isempty(obj.Grade)
        %
        %             else
        %
        %             end
        %             yyaxis left
        %             plot(obj.RefVel)
        %
        %             yyaxis right
        %             plot(obj.RefVel)
        %
        %         end
        
        %% plotPropTbl
        function plotPropTbl(obj, PropName, AxisHdl)
            % plotPropTbl plot PropName table in current axis
            if nargin < 3
                AxisHdl = gca;
            end
            Tbl = obj.(PropName);
            plot(AxisHdl, Tbl{:,1}, Tbl{:,2})
            AxisNames = cell(2,1);
            for i = 1:2
                AxisNames{i} = [Tbl.Properties.VariableNames{i}, ' (', Tbl.Properties.VariableUnits{i}, ')'];
            end
            xlabel(AxisHdl, AxisNames{1})
            ylabel(AxisHdl, AxisNames{2})
        end
        %% getDataStruct
        function DataStruct = getCycleDataStruct(obj)
            if length(obj) > 1
                for i = 1:length(obj)
                    DataStruct(i) = obj(i).getCycleDataStruct;
                end
            else
                
                % Name 
                DataStruct.Name = obj.Name;
                
                % Velocity
                DataStruct.VelRef.Tbl = obj.VelRef{:,2};
                DataStruct.VelRef.Bpts = obj.VelRef{:,1};
                DataStruct.VelRef.TblUnits = obj.VelRef.Properties.VariableUnits{2};
                DataStruct.VelRef.BptUnits = obj.VelRef.Properties.VariableUnits{1};
                
                % Grade
                DataStruct.Grade.UseTimeGradeBpts = 1;
                if isempty(obj.Grade)
                    DataStruct.Grade.Tbl = [0,0];
                    DataStruct.Grade.Bpts = [0,1];
                    DataStruct.Grade.TblUnits = 'rad';
                    DataStruct.Grade.BptUnits = 's';
                else
                    DataStruct.Grade.Tbl = obj.Grade{:,2};
                    DataStruct.Grade.Bpts = obj.Grade{:,1};
                    if ~strcmpi(obj.Grade.Properties.VariableNames{1}, 'Time')
                        DataStruct.Grade.UseTimeGradeBpts = 0;
                    end
                    DataStruct.Grade.TblUnits = obj.Grade.Properties.VariableUnits{2};
                    DataStruct.Grade.BptUnits = obj.Grade.Properties.VariableUnits{1};
                end
                
                % Gear
                if isempty(obj.Gear)
                    DataStruct.Gear.Tbl = [1,1];
                    DataStruct.Gear.Bpts = [0,1];
                else
                    DataStruct.Gear.Tbl = obj.Gear{:,2};
                    DataStruct.Gear.Bpts = obj.Gear{:,1};
                end
            end
        end
    end
    
    
end