function alignBlockPorts(StationaryBlockPortHdl, MoveBlockPortHdl)
% Function moves block to align signals between two blocks for connecting
% line.
MoveBlock = get_param(MoveBlockPortHdl, 'Parent');
MoveBlockPosition = get_param(MoveBlock, 'Position');
MoveBlockPortPos = get_param(MoveBlockPortHdl, 'Position');

AlignPortLoc = get_param(StationaryBlockPortHdl, 'Position');

PortOffset = AlignPortLoc(2) - MoveBlockPortPos(2);

MoveBlockPosition(2) = MoveBlockPosition(2) + PortOffset;
MoveBlockPosition(4) = MoveBlockPosition(4) + PortOffset;

set_param(MoveBlock, 'Position', MoveBlockPosition);



