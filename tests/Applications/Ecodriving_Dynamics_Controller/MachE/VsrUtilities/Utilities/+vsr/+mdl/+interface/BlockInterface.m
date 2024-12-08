classdef BlockInterface < handle
    % Class definition handles actions for interfacing with a model
    % component such as an engine controller or engine plant model. Use
    % methods to create ports, create bus, and connect to desired block.
    %% Public properties
    properties
        ConnectedBlock
        InterfaceBlock
    end
    
    %% Private properties
    properties (Access = private)
        ConnectedBlockHdl
        InterfaceBlockHdl
    end
    
    %% Public methods
    methods
        %% listBlocksToInterface
        % Method returns possible blocks that can be connected to block
        % output interface
        function FoundBlocks = listBlocksToInterface(obj)
            FoundBlocks = findComponentsInSystem(get_param(obj.InterfaceBlock, Parent));
        end
        
        %% updateBlockHdls
        function updateBlockHdls(obj)
            obj.ConnectedBlockHdl = get_param(obj.ConnectedBlock, 'Handle');
            obj.InterfaceBlockHdl = get_param(obj.InterfaceBlock, 'Handle');
        end
    end
    
    %% Abstract methods
    methods (Abstract)
        %% isPortsUpdated
        % Returns true if all ports have been updated
        isPortsUpdated(obj)
        
        %% updatePorts
        updatePorts(obj)
        
        %% connectsPorts
        %         connectPorts(obj)
    end
    
    %% More public methods
    methods
        %% isPortsUpdated
        % Returns true if all ports have been updated
        
        %% updateAll
        % Updates the block ports, resizes interface block, resizes
        % connected block, and connects ports
        function updateAll(obj)
            obj.updatePorts;
            obj.resizeBlocks;
            obj.connectPorts;
        end
        
        %% resizeBlocks
        function resizeBlocks(obj)
            
        end
        %% resizeInterfaceBlock
        % Update interface block size and position
        
        %% resizeConnectedBlock
        % update connected block size to make signals visible
        
    end
    
    %% get/set methods
    methods
        %% get.ConnectedBlock
        function BlockName = get.ConnectedBlock(obj)
            if ishandle(obj.ConnectedBlockHdl)
                obj.ConnectedBlock = getBlockName(obj.ConnectedBlockHdl);
            end
            BlockName = obj.ConnectedBlock;
        end
        
        %% get.InterfaceBlock
        function BlockName = get.InterfaceBlock(obj)
            if ishandle(obj.InterfaceBlockHdl)
                obj.InterfaceBlock = getBlockName(obj.InterfaceBlockHdl);
            end
            BlockName = obj.InterfaceBlock;
        end
        
        %% set.ConnectedBlock
        function set.ConnectedBlock(obj, BlockName)
            if ~isempty(obj.InterfaceBlock)
                FoundBlocks = obj.listBlocksToInterface;
                if ~any(strcmp(BlockName, FoundBlocks))
                    error('''%s'' is not a valid block name. Provide a block in the same system as the interface block ''%s''.', BlockName, obj.InterfaceBlock)
                end
            end
            obj.ConnectedBlock = BlockName;
        end
        
    end
    
    %% Protected methods
    methods (Access = protected)
        %% getBlockPositionFromNumPorts
        % Calculate block position from original position and number of
        % ports. The method attempts to equal space out signals and
        % maintain previous width of block.
        function NewPos = getBlockPositionFromNumPorts(obj, Block)
            OldPos = get_param(Block, 'Position');
            PortNames = vsr.mdl.getBlockPortNames(Block);
            MaxNumPorts = max(length(PortNames.Inport), length(PortNames.Outport));
            Height = max(MaxNumPorts*30, 20);
            Center = (OldPos(4) - OldPos(2))/2;
            NewPos = OldPos;
            NewPos(2) = round(Center - Height/2);
            NewPos(4) = NewPos(2) + round(Height);
        end

    end
end