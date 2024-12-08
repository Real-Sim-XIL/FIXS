function PortBlkHdls = getSystemPortBlockHdls(SystemName)
% Returns port block (inport and outport) block handles in order of port
% number. There will be an error if SystemName is not a subsystem or
% system. 

%% Setup
SystemHdl = get_param(SystemName, 'Handle');

%% Inports
PortBlkHdls.Inport = [];
PortBlocks = find_system(SystemHdl, 'FollowLinks', 'on', 'LookUnderMasks', 'on', 'SearchDepth', 1, 'BlockType', 'Inport');
PortNums = get_param(PortBlocks, 'Port');
for i = 1:length(PortNums)
    PortBlkHdls.Inport(i) = PortBlocks(strcmp(PortNums, num2str(i)));
end
%% Outports
PortBlkHdls.Outport = [];
PortBlocks = find_system(SystemHdl, 'FollowLinks', 'on', 'LookUnderMasks', 'on', 'SearchDepth', 1, 'BlockType', 'Outport');
PortNums = get_param(PortBlocks, 'Port');
for i = 1:length(PortNums)
    PortBlkHdls.Outport(i) = PortBlocks(strcmp(PortNums, num2str(i)));
end

