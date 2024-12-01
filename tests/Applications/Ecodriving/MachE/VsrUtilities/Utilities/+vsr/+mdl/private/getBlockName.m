function FullBlockName = getBlockName(BlockHdl)
% Function returns full block name from handle
ParentSystem = get_param(BlockHdl, 'Parent');
if ~isempty(ParentSystem)
    FullBlockName = [ParentSystem, '/', get_param(BlockHdl, 'Name')];
else
    FullBlockName = get_param(BlockHdl, 'Name');
end

