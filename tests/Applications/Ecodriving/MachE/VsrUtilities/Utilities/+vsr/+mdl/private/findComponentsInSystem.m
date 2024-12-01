function FoundBlocks = findComponentsInSystem(SystemName)
% Function returns blocks in a given system named "SystemName" that are not interface blocks.
%
% Inputs:
%
% Outputs:

%% Setup 
ExcludeBlockRefs = {'VsrLibMdlInterface/Output Interface', 'VsrLibMdlInterface/Input Interface'};
ExcludeBlockTypes = {'Inport', 'Outport', 'PMIOPort'};
ExcludeNames = cellstr(SystemName);

%% Find components
FoundBlocks = find_system(SystemName, 'SearchDepth', 1);
BlockRefs = get_param(FoundBlocks, 'ReferenceBlock');
BlockTypes = get_param(FoundBlocks, 'BlockType');
RemoveRefs= ismember(BlockRefs(:), ExcludeBlockRefs(:));
RemoveTypes = ismember(BlockTypes(:), ExcludeBlockTypes(:));
RemoveNames = ismember(FoundBlocks(:), ExcludeNames(:));
KeepBlocks = ~(RemoveRefs | RemoveTypes | RemoveNames);
FoundBlocks = FoundBlocks(KeepBlocks);