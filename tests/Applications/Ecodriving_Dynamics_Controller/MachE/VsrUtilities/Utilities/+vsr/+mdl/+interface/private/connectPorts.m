function LineHdl = connectPorts(OutportHdl, InportHdl, BlockAlign, PortSpacing)
% connectPorts(OutportHdl, InportHdl) adds a line to connect InportHdl and
% OutportHdl if a line does not already exist. 
% 
% connectPorts(OutportHdl, InportHdl, BlockAlign) connects
% ports and aligns block base on input. Inputs for BlockAlign can be
% 'off', 'Inport', or 'Outport'. 'Inport' moves block associated with
% InportHdl to align ports vertically. 'Outport' moves the block
% associated with the OutportHdl. Function currently does not support
% alignment if one of the blocks are rotated.
% 
% connectPorts(OutportHdl, InportHdl, BlockAlign, PortSpacing) applies 
% horizontal spacing PortSpacing between the blocks by moving the block called out by
% BlockAlign setting. If BlockAlign = 'off', the block associated with
% InportHdl is moved to meet the desired PortSpacing. Port spacing is not
% applied if PortSpacing < 0;

%% Check inputs
if nargin < 3
    BlockAlign = 'off';
end
if nargin < 4
    PortSpacing = -1;
end

%% Move block based inputs
InportPos = get_param(InportHdl, 'Position');
OutportPos = get_param(OutportHdl, 'Position');
PortDiff = InportPos - OutportPos;
if strcmpi(BlockAlign, 'Inport')
    MoveBlk = get_param(InportHdl, 'Parent');
    MoveBlkPos = get_param(MoveBlk, 'Position');
    MoveBlkPos([2,4]) = MoveBlkPos([2,4]) - PortDiff(2);
    SpacingDir = -1;
elseif strcmpi(BlockAlign, 'Outport')
    MoveBlk = get_param(OutportHdl, 'Parent');
    MoveBlkPos = get_param(MoveBlk, 'Position');
    MoveBlkPos([2,4]) = MoveBlkPos([2,4]) + PortDiff(2);
    SpacingDir = 1;
else
    MoveBlk = get_param(InportHdl, 'Parent');
    MoveBlkPos = get_param(MoveBlk, 'Position');
    SpacingDir = -1;
end

if PortSpacing > 0
    MoveBlkPos([1,3]) = MoveBlkPos([1,3]) + SpacingDir*(PortDiff(1) - PortSpacing);
end
set_param(MoveBlk, 'Position', MoveBlkPos)
%% Connect lines
LineHdl = get_param(InportHdl, 'Line');
ParentBlk = get_param(MoveBlk, 'Parent');
if ~ishandle(LineHdl)
    LineHdl = add_line(ParentBlk, OutportHdl, InportHdl, 'AutoRouting', 'on');
end
    
    


