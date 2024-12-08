classdef BlockOutputInterface < vsr.mdl.BlockInterface
    
    %% Public properties
    properties
        OutputBusName char % Name of output bus
    end
    
    %% Private properties
    properties (Dependent, Access = private)
        BusCreatorName
    end
    
    %% Public methods
    methods
        
        %% isPortsUpdated
        function flag = isPortsUpdated(obj)
            import vsr.mdl.*
            flag = false;
            AllPortNames = getBlockPortNames(obj.ConnectedBlock);
            ConnectedBlockPortNames = cellstr(AllPortNames.Outport);
            Lines = obj.getBusCreatorInputLines;
            if all(ishandle(Lines))
                LineNames = cellstr(get_param(Lines, 'Name'));
                
                if isequal(ConnectedBlockPortNames, LineNames)
                    flag = true;
                end
            end
        end
        
        %% updatePorts
        function updatePorts(obj)
            
            %% Set up
            import vsr.mdl.*
            BusCreator = obj.BusCreatorName;
            
            % Remove lines connected to bus inports
            Lines = obj.getBusCreatorInputLines;
            for i = 1:length(Lines)
                if ishandle(Lines(i))
                    delete_line(Lines(i));
                end
            end
            
            % Update bus creator
            ConnectedBlockPortNames = getBlockPortNames(obj.ConnectedBlock);
            ConnectedBlockPortNames = ConnectedBlockPortNames.Outport;
            set_param(BusCreator, 'Inputs', num2str(length(ConnectedBlockPortNames)));
            set_param(BusCreator, 'Position', obj.getBlockPositionFromNumPorts(BusCreator));
            
            %% Update port blocks
            CurrPortBlockHdls = getSystemPortBlockHdls(obj.InterfaceBlock);
            CurrPortBlockNames = get_param(CurrPortBlockHdls.Inport, 'Name');
            
            % Remove blocks with signal names that do not match
            for i = 1:length(CurrPortBlockNames)
                if ~ismember(CurrPortBlockNames{i}, ConnectedBlockPortNames)
                    delete_block(CurrPortBlockHdls.Inport(i))
                end
            end
            
            % Add blocks
            for i = 1:length(ConnectedBlockPortNames)
                PortBlockName = [obj.InterfaceBlock, '/', ConnectedBlockPortNames{i}];
                if ~ismember(ConnectedBlockPortNames{i}, CurrPortBlockNames)
                    add_block('simulink/Ports & Subsystems/In1', PortBlockName)
                end
                set_param(PortBlockName, 'Port', num2str(i))
            end
            
            %% Connect blocks and update line names
            CurrPortBlockHdls = getSystemPortBlockHdls(obj.InterfaceBlock);
            BusPortHandles = obj.getBusCreatorInports;
%             Lines = obj.getBusCreatorInputLines;
            for i = 1:length(BusPortHandles)
                CurrPortBlockPortHandles = get_param(CurrPortBlockHdls.Inport(i), 'PortHandles');
                
                alignBlockPorts(BusPortHandles(i), CurrPortBlockPortHandles.Outport(1));
                NewLineHdl = add_line(obj.InterfaceBlock, CurrPortBlockPortHandles.Outport(1), BusPortHandles(i));
                set_param(NewLineHdl, 'Name', ConnectedBlockPortNames{i})
            end
            
            %% Update outputs
            BusPortHandles = get_param(obj.BusCreatorName, 'PortHandles');
            BusOutport = BusPortHandles.Outport(1);
            LineOut = get_param(BusOutport, 'Line');
            set_param(LineOut, 'Name', obj.OutputBusName);
            set_param(CurrPortBlockHdls.Outport(1), 'Name', obj.OutputBusName)
            CurrPortBlockPortHandles = get_param(CurrPortBlockHdls.Outport(1), 'PortHandles');
            alignBlockPorts(BusOutport, CurrPortBlockPortHandles.Inport(1));
            
        end
        
        %% connectsPorts
        
        
        %% getCurrentSignalNames
        function CurrSignalNames = getCurrentSignalNames(obj)
            Lines = obj.getBusCreatorInputLines;
            CurrSignalNames = cellstr(get_param(Lines, 'Name'));
        end
        %% get.BusCreatorName
        function Name = get.BusCreatorName(obj)
            Name = [obj.InterfaceBlock, '/Bus Creator'];
        end
        
    end
    
    %% Private methods
    methods (Access = private)
        %% getBusCreatorInputLines
        function Lines = getBusCreatorInputLines(obj)
            BusPortHandles = obj.getBusCreatorInports;
            Lines = get_param(BusPortHandles, 'Line');
            if iscell(Lines)
                Lines = cell2mat(Lines);
            end
        end
        
        %% getBusCreatorInports
        function BusPortHandles = getBusCreatorInports(obj)
            BusPortHandles = get_param(obj.BusCreatorName, 'PortHandles');
            BusPortHandles = BusPortHandles.Inport;
        end
    end
    
    
end