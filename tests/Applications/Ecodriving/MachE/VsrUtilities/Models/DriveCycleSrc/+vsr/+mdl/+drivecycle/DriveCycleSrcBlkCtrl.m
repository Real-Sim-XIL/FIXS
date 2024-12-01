classdef DriveCycleSrcBlkCtrl < vsr.mdl.BlkMaskCtrl
    % Use to control 'VsrLibDriveCycleSrc/Drive Cycle Lookup' mask
    % parameters and init.
    
    %% Public methods
    methods
        %% Init
        function MaskData = Init(obj)
            %% Setup
            % Get drive cycle objects
            DriveCycleStructArray = obj.getParamValue('DriveCycleStructArray');
            DistCompensationCheckbox = obj.getParamValue('DistCompensationCheckbox');
            
            %% Update lookup subsystems
            DriveCycleLookupSubsystem = [obj.FullBlkName, '/Drive Cycle Lookups'];
            CurrLookupSubsystems = find_system(DriveCycleLookupSubsystem, 'SearchDepth', 1, 'Mask', 'on');
            CurrNumSubsystems = length(CurrLookupSubsystems);
            NumDriveCycles = max(length(DriveCycleStructArray), 1);
            LookupBlkFcn = @(i) [DriveCycleLookupSubsystem, '/Lookup', num2str(i)];
            
            % Check if ports connected
            UpdateLookupTbls = false;
            if NumDriveCycles ~= CurrNumSubsystems
                UpdateLookupTbls = true;
            end
            if ~UpdateLookupTbls
                for i = 1:CurrNumSubsystems
                    NewLookupPortHdls = get_param(LookupBlkFcn(i), 'PortHandles');
                    PortHdls = [NewLookupPortHdls.Inport, NewLookupPortHdls.Outport];
                    for j = 1:length(PortHdls)
                        if ~ishandle(get_param(PortHdls(j), 'Line'))
                            UpdateLookupTbls = true;
                            break
                        end
                        if UpdateLookupTbls
                            break;
                        end
                    end
                end
            end
            
            
            % Update subsystems and connections
            if UpdateLookupTbls
                % Setup 
                TimeInportBlk = [DriveCycleLookupSubsystem, '/Time'];
                VehDistInportBlk = [DriveCycleLookupSubsystem, '/VehDist'];
                CycleDistImportHdls = [DriveCycleLookupSubsystem, '/DriveCycleDist'];
                TimeInportHdl = get_param(TimeInportBlk, 'PortHandles');
                VehDistInportHdls = get_param(VehDistInportBlk, 'PortHandles');
                CycleDistImportHdls = get_param(CycleDistImportHdls, 'PortHandles');
                LookupInportHdls = [TimeInportHdl.Outport, VehDistInportHdls.Outport, CycleDistImportHdls.Outport];
                MuxBlk = [DriveCycleLookupSubsystem, '/Mux'];
                
                % Remove lookup subsystems
                for i = 1:CurrNumSubsystems
                     CurrBlkName = LookupBlkFcn(i);
                     PortHdls = get_param(CurrBlkName, 'PortHandles');
                     PortHdls = [PortHdls.Inport, PortHdls.Outport];
                     for j = 1:length(PortHdls)
                         Line = get_param(PortHdls(j), 'Line');
                         if ishandle(Line)
                            delete_line(Line)
                         end
                     end
                     delete_block(CurrBlkName);
                end
                
                % Set up mux block
                set_param(MuxBlk, 'Inputs', num2str(NumDriveCycles+1));
                MuxPortHdls = get_param(MuxBlk, 'PortHandles');
                MuxPortHdls = MuxPortHdls.Inport(2:end);
                set_param(MuxBlk, 'Position', [370, 0, 375, 120*(NumDriveCycles+1)])
                
                % Update 
                for i = 1:NumDriveCycles
                    NewBlkName = LookupBlkFcn(i);
                
                    % Add new subsystems
                    add_block('VsrLibDriveCycleSrc/Drive Cycle Lookup', NewBlkName)
                    MuxPortPos = get_param(MuxPortHdls(i), 'Position');
                    BlkH = 60;
                    set_param(NewBlkName, 'Position',  [0, MuxPortPos(2)-BlkH/2, 160, MuxPortPos(2)+BlkH/2]);
                    
                    % Set parameter values
                    set_param(NewBlkName, 'DriveCycleStruct', ['MaskData.DriveCycleData', num2str(i)]);
                    set_param(NewBlkName, 'TimeLookAhead', 'TimeLookAhead');
                    
                    % Connect blocks
                    NewLookupPortHdls = get_param(NewBlkName, 'PortHandles');
                    for j = 1:length(LookupInportHdls)
                        add_line(DriveCycleLookupSubsystem, LookupInportHdls(j), NewLookupPortHdls.Inport(j));
                    end
                    add_line(DriveCycleLookupSubsystem, NewLookupPortHdls.Outport(1), MuxPortHdls(i));
                end
            end
            %% Calculate mask data 
            MaskData.EnableDistCompensation = DistCompensationCheckbox;
            MaskData.NumCycleVals = 6;
            MaskData.NumCycles = NumDriveCycles;
            % Add drive cyle data to MaskData
            for i = 1:length(DriveCycleStructArray)
                MaskData.(['DriveCycleData', num2str(i)]) = DriveCycleStructArray(i);
            end
        end
        
        %% getDriveCycleSummary
        function SummaryTbl = getDriveCycleSummary(obj)
            % SummaryTbl = getDriveCycleSummary(obj) returns a summary of
            % the drive cycle information entered in to the 'VSR Drive Cycle
            % Source' block.
            DriveCycleStructArray = obj.getParamValue('DriveCycleStructArray');
            
            CycleNum = 1:1:length(DriveCycleStructArray);
            CycleNum = CycleNum(:);
            CycleName = {DriveCycleStructArray.Name}';
            
            SummaryTbl = table(CycleNum, CycleName);
        end
    end
end
