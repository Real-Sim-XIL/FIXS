function [varargout]= vdynblksrefconfig(varargin)
%   Copyright 2018 The MathWorks, Inc.
block = varargin{1};
maskMode = varargin{2};
varargout{1} = {};
simStopped = autoblkschecksimstopped(block,true);
manType = get_param(block,'manType');
vehSys = bdroot(block);
switch maskMode
    case 0
        if simStopped
            switch manType
                case 'Double Lane Change'
                    set_param([block '/Reference Generator'],'OverrideUsingVariant','0');
                    set_param([vehSys '/Visualization/Scope Type'],'OverrideUsingVariant','1');
                    set_param([vehSys '/Visualization/Vehicle XY Plotter'],'OverrideUsingVariant','1');
                case 'Increasing Steer'
                    set_param([block '/Reference Generator'],'OverrideUsingVariant','1');
                    set_param([vehSys '/Visualization/Scope Type'],'OverrideUsingVariant','0');
                    set_param([vehSys '/Visualization/Vehicle XY Plotter'],'OverrideUsingVariant','0');
                case 'Swept Sine'
                    set_param([block '/Reference Generator'],'OverrideUsingVariant','2');
                    set_param([vehSys '/Visualization/Scope Type'],'OverrideUsingVariant','0');
                    set_param([vehSys '/Visualization/Vehicle XY Plotter'],'OverrideUsingVariant','0');
                case 'Sine with Dwell'
                    set_param([block '/Reference Generator'],'OverrideUsingVariant','3');
                    set_param([vehSys '/Driver Commands'],'driverType','Longitudinal Driver')
                    set_param([vehSys '/Visualization/Scope Type'],'OverrideUsingVariant','0');
                    set_param([vehSys '/Visualization/Vehicle XY Plotter'],'OverrideUsingVariant','0');
                case 'Constant Radius'
                    set_param([block '/Reference Generator'],'OverrideUsingVariant','4');
                    set_param([vehSys '/Driver Commands'],'driverType','Predictive Driver')
                    set_param([vehSys '/Visualization/Scope Type'],'OverrideUsingVariant','2');
                    set_param([vehSys '/Visualization/Vehicle XY Plotter'],'OverrideUsingVariant','1');
                otherwise
            end
        end
    case 1
        manOverride = strcmp(get_param(block,'manOverride'),'on');
        switch manType
            case 'Double Lane Change'
                autoblksenableparameters(block, [], [],{'DLCGroup'},{'ISGroup';'CRGroup';'SSGroup';'SDGroup'});
                if manOverride
                    set_param([vehSys '/Driver Commands'],'driverType','Predictive Driver');
                    set_param(block,'simTime','25');
                end
            case 'Increasing Steer'
                autoblksenableparameters(block, [], [],{'ISGroup'},{'DLCGroup';'CRGroup';'SSGroup';'SDGroup'});
                if manOverride
                    set_param([vehSys '/Driver Commands'],'driverType','Longitudinal Driver');
                    set_param(block,'simTime','60');
                end
            case 'Swept Sine'
                autoblksenableparameters(block, [], [],{'SSGroup'},{'ISGroup';'DLCGroup';'CRGroup';'SDGroup'});
                if manOverride
                    set_param([vehSys '/Driver Commands'],'driverType','Longitudinal Driver');
                    set_param(block,'simTime','40');
                end
            case 'Sine with Dwell'
                autoblksenableparameters(block, [], [],{'SDGroup'},{'ISGroup';'DLCGroup';'CRGroup';'SSGroup'});
                if manOverride
                    set_param([vehSys '/Driver Commands'],'driverType','Longitudinal Driver');
                    set_param(block,'simTime','25');
                end
            case 'Constant Radius'
                autoblksenableparameters(block, [], [],{'CRGroup'},{'DLCGroup';'ISGroup';'SSGroup';'SDGroup'});
                if manOverride
                    set_param([vehSys '/Driver Commands'],'driverType','Predictive Driver');
                    set_param(block,'simTime','60');
                end
            otherwise
        end
        if simStopped && manOverride && ~strcmp(get_param(block,'prevType'),manType)
            update3DScene(block,manType);
            [~] = vdynblksmdlWSconfig(block,false);
%             simStopTime = get_param(block,'simTime');
%             set_param(vehSys,'StopTime',simStopTime);
        end
        set_param(block,'prevType',manType)
    case 2
        simStopTime = get_param(block,'simTime');
        set_param(vehSys,'StopTime',simStopTime);
end
end
function update3DScene(block,manType)
sim3DBlkPath = block;
if strcmp(manType,'Double Lane Change')
    set_param(sim3DBlkPath,'SceneDesc','Double lane change');
else
    set_param(sim3DBlkPath,'SceneDesc','Open surface');
end
end