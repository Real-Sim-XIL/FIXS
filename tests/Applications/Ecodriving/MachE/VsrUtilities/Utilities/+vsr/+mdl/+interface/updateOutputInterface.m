function updateOutputInterface(Blk)
% updateOutputInterface() creates an output bus for the selected Simulink 
% block (see gcb) if a output bus block does not exist or updates output 
% bus block if one already exists. The output bus signal names match the 
% names of the selected block port names.
% updateOutputInterface(Blk) creates an output bus for Simulink block Blk

%% Setting values
InterfaceBlkTag = 'BlockOutputInterface';
PortSpaceFactor = 30;
HorzSpacing = 30;
InportHorzSpacing = 200;
InterfaceBlkWidth = 20;
%% Set up 
if nargin < 1
    Blk = gcb;
end
MainBlkPortHdls = get_param(Blk, 'PortHandles');
PortNames = vsr.mdl.getBlockPortNames(Blk);
PortNames = PortNames.Outport;
NumPorts = length(PortNames);
%% Find existing interface block
CurrInterfaceBlk = -1;
for i = 1:length(MainBlkPortHdls.Outport)
    LineHdl = get_param(MainBlkPortHdls.Outport(i), 'Line');
    Tag = '';
    if ishandle(LineHdl)
        DstBlockHandles = get_param(LineHdl, 'DstBlockHandle');
        if length(DstBlockHandles) == 1
            Tag = get_param(DstBlockHandles(1), 'Tag');
        end
    end
    
    if strcmp(Tag, InterfaceBlkTag)
        CurrInterfaceBlk = DstBlockHandles(1);
        break;
    end
end

%% Create new interface block if does not exist
if ~ishandle(CurrInterfaceBlk)
    InterfaceBlk = add_block('built-in/SubSystem', [get_param(Blk,'Parent'), '/OutputInterface'], 'MakeNameUnique', 'on'); 
    set_param(InterfaceBlk, 'Tag', InterfaceBlkTag);
    Simulink.Mask.create(InterfaceBlk);
    set_param(InterfaceBlk, 'MaskDisplay', 'disp('''')');
    set_param(InterfaceBlk, 'ShowName', 'off');
    CurrInterfaceBlk = get_param(InterfaceBlk, 'Handle');
end

%% Update inerface outport and bus creator

% Outport
Outport = find_system(CurrInterfaceBlk, 'SearchDepth', '1', 'BlockType', 'Outport');
if isempty(Outport)
        Outport = add_block('simulink/Ports & Subsystems/Out1',...
            [get_param(CurrInterfaceBlk,'Parent'), '/', get_param(CurrInterfaceBlk, 'Name'), '/Out'], 'MakeNameUnique', 'on'); 
elseif length(Outport) > 1
    for i = 2:length(Outport)
        delete_block(Outport(i))
    end
    Outport = Outport(1);
end
    
% Bus creator
BusCreator = find_system(CurrInterfaceBlk, 'SearchDepth', '1', 'BlockType', 'BusCreator');
if isempty(BusCreator)
        BusCreator = add_block('simulink/Signal Routing/Bus Creator',...
            [get_param(CurrInterfaceBlk,'Parent'), '/', get_param(CurrInterfaceBlk, 'Name'),...
            '/Bus Creator'], 'MakeNameUnique', 'on'); 
elseif length(BusCreator) > 1
    for i = 2:length(BusCreator)
        delete_block(BusCreator(i))
    end
    BusCreator = BusCreator(1);
end
set_param(BusCreator, 'Inputs', num2str(NumPorts))
BusPos = get_param(BusCreator, 'Position');
BusPos(4) = BusPos(2) + max(1, NumPorts)*PortSpaceFactor;
set_param(BusCreator, 'Position', BusPos)

% Connect outport to bus creator
BusCreatorPortHdls = get_param(BusCreator, 'PortHandles');
OutportPortHdls = get_param(Outport, 'PortHandles');
connectPorts(BusCreatorPortHdls.Outport, OutportPortHdls.Inport, 'Inport', HorzSpacing);

%% Remove existing lines
% Block interface
InterfacePortHdls = get_param(CurrInterfaceBlk, 'PortHandles');
for i = 1:length(InterfacePortHdls.Inport)
    Line = get_param(InterfacePortHdls.Inport(i), 'Line');
    if ishandle(Line)
        delete_line(Line);
    end
end

% Bus creator
BusCreatorPortHdls = get_param(BusCreator, 'PortHandles');
for i = 1:length(BusCreatorPortHdls.Inport)
    Line = get_param(BusCreatorPortHdls.Inport(i), 'Line');
    if ishandle(Line)
        delete_line(Line);
    end
end

%% Update interface block inputs
CurrInportBlks = find_system(CurrInterfaceBlk, 'SearchDepth', '1', 'BlockType', 'Inport');
for i = 1:length(CurrInportBlks)
    delete_block(CurrInportBlks(i));
end

for i = 1:length(PortNames)
    InportBlk = add_block('simulink/Ports & Subsystems/In1',...
            [get_param(CurrInterfaceBlk,'Parent'), '/', get_param(CurrInterfaceBlk, 'Name'), '/', PortNames{i}],...
            'MakeNameUnique', 'on'); 
    InportBlkPortHdls = get_param(InportBlk, 'PortHandles');
    LineHdl = connectPorts(InportBlkPortHdls.Outport, BusCreatorPortHdls.Inport(i), 'Outport', InportHorzSpacing);
    set_param(LineHdl, 'Name', PortNames{i})
end

%% Update size and position
MainBlockPos = get_param(Blk, 'Position');
InterfaceBlkPos = MainBlockPos;
InterfaceBlkPos(1) = InterfaceBlkPos(3) + HorzSpacing;
InterfaceBlkPos(3) = InterfaceBlkPos(1) + InterfaceBlkWidth;
set_param(CurrInterfaceBlk, 'Position', InterfaceBlkPos)

%% Update interface block connections
InterfacePortHdls = get_param(CurrInterfaceBlk, 'PortHandles');
for i = 1:length(PortNames)
    connectPorts(MainBlkPortHdls.Outport(i), InterfacePortHdls.Inport(i), 'Inport', HorzSpacing);
end
end
