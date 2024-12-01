function PortNames = getBlockPortNames(Block)
% Get block port names. The block can be a subsystem, built-in block, or
% model reference.
%
% Outputs:
%
%% Set up
Block = get_param(Block, 'Handle');
Type = get_param(Block, 'Type');

%% Create output struct
PortHandles = get_param(Block, 'PortHandles');
PortNames.Inport = {};
PortNames.Outport = {};


%% Get names from ports if possible
BlockType = get_param(Block, 'BlockType');
if strcmp(BlockType, 'ModelReference')
    MdlRefName = get_param(Block, 'ModelName');
    if ~bdIsLoaded(MdlRefName)
        load_system(MdlRefName)
    end
    PortBlkHdls = getSystemPortBlockHdls(MdlRefName);
    PortNames.Inport = cellstr(get_param(PortBlkHdls.Inport, 'Name'));
    PortNames.Outport = cellstr(get_param(PortBlkHdls.Outport, 'Name'));
elseif strcmp(BlockType, 'SubSystem')
    PortBlkHdls = getSystemPortBlockHdls(Block);
    PortNames.Inport = cellstr(get_param(PortBlkHdls.Inport, 'Name'));
    PortNames.Outport = cellstr(get_param(PortBlkHdls.Outport, 'Name'));
else
    for i = 1:length(PortHandles.Inport)
        PortNames.Inport{i} = ['In', num2str(i)];
    end
    for i = 1:length(PortHandles.Outport)
        PortNames.Outport{i} = ['Out', num2str(i)];
    end
    
end


