function [varargout]= vdynblksdrvrconfig(varargin)
%   Copyright 2018-2019 The MathWorks, Inc.
block = varargin{1};
varargout{1} = {};
simStopped = autoblkschecksimstopped(block,true);
driverType = get_param(block,'driverType');
if ~isempty(find_system(bdroot(block),'Name','Reference Generator'))
    manType = get_param([bdroot(block) '/Reference Generator'],'manType');
    useDefValues = get_param([bdroot(block) '/Reference Generator'],'manOverride');
else
    manType = '';
    useDefValues = 'off';
end
drvPath = [block '/Driver Commands/Predictive Driver/Linear Predictive Driver'];
if simStopped
    switch driverType
        case 'Longitudinal Driver'
            set_param([block '/Driver Commands'],'OverrideUsingVariant','0');
            SwitchInport(block,'Steer','Ground',[]);
            SwitchInport(block,'Accel','Ground',[]);
            SwitchInport(block,'Brake','Ground',[]);
            SwitchInport(block,'Gear','Ground',[]);            
        case 'Predictive Driver'
            set_param([block '/Driver Commands'],'OverrideUsingVariant','1');
            SwitchInport(block,'Steer','Ground',[]);
            SwitchInport(block,'Accel','Ground',[]);
            SwitchInport(block,'Brake','Ground',[]);
            SwitchInport(block,'Gear','Ground',[]);
            if strcmp(useDefValues,'on')
                if strcmp('Double Lane Change',manType)                    
                    set_param(drvPath,'tau','0.03')
                    set_param(drvPath,'L','6')
                    set_param(drvPath,'theta','30*pi/180')
                else
                    set_param(drvPath,'tau','0.03')
                    set_param(drvPath,'L','1.0')
                    set_param(drvPath,'theta','40*pi/180')
                end
            end    
        case 'Open Loop'
            set_param([block '/Driver Commands'],'OverrideUsingVariant','2');
            SwitchInport(block,'Steer','Inport',[]);
            SwitchInport(block,'Accel','Inport',[]);
            SwitchInport(block,'Brake','Inport',[]);
            SwitchInport(block,'Gear','Inport',[]);
        otherwise
    end
end
end
function SwitchInport(Block, PortName, UsePort,Param)
%% Switch inport
InportOption  = {'built-in/Constant', [PortName 'Constant'];...
    'built-in/Inport', PortName;...
    'simulink/Sinks/Terminator',[PortName 'Terminator'];...
    'simulink/Sinks/Out1', PortName;...
    'built-in/Ground',[PortName 'Ground']};
switch UsePort
    case 'Constant'
        Newblock = autoblksreplaceblock(Block, InportOption, 1);
        set_param(Newblock, 'Value',Param);
    case 'Terminator'
        autoblksreplaceblock(Block, InportOption, 3);
    case 'Outport'
        autoblksreplaceblock(Block, InportOption, 4);
    case 'Inport'
        autoblksreplaceblock(Block, InportOption, 2);
    case 'Ground'
        autoblksreplaceblock(Block, InportOption, 5);
end

end