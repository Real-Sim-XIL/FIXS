% classdef ExpRefBlkCtrl < vsr.mdl.BlkMaskCtrl
% %     % Use to control 'VsrLibDriveCycleSrc/Drive Cycle Lookup' mask
% %     % parameters and init.
% %     
% %     %% Public methods
%     methods
% %         %% Init
%         function MaskData = Init(obj)
            %% Setup
%             % Get drive cycle objects
%             DriveCycleStructArray = obj.getParamValue('DriveCycleStructArray');
%             DistCompensationCheckbox = obj.getParamValue('DistCompensationCheckbox');
            
            obj = vsr.mdl.BlkMaskCtrl('EvBoltCalib_Veh_v0/ExperimentReferenceMeasured1');
            DdDriveCycle = Simulink.data.dictionary.open('DdExpRefSrc.sldd');
            DdSection = DdDriveCycle.getSection('Design Data');
            ExpRefStructs = getValue(getEntry(DdSection, 'ExpRefStructs'));

            %% Update lookup subsystems
            DriveCycleLookupSubsystem = obj.FullBlkName;
            CurrLookupSubsystems = find_system(DriveCycleLookupSubsystem, 'SearchDepth', 1, 'LookUnderMasks', 'on', 'BlockType','SubSystem');
            CurrNumSubsystems = length(CurrLookupSubsystems)-1;
            NumDriveCycles = max(length(ExpRefStructs), 1);
            LookupBlkFcn = @(i) [DriveCycleLookupSubsystem, '/ExpRefLookup', num2str(i)];
            
            %%
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
            
            %%
            % Update subsystems and connections
            if UpdateLookupTbls
                % Setup 
                CycleTimeInportBlk = [DriveCycleLookupSubsystem, '/CycleTime'];
                CycleTimeInportHdl = get_param(CycleTimeInportBlk, 'PortHandles');

                LookupInportHdls = [CycleTimeInportHdl.Outport];
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
                set_param(MuxBlk, 'Inputs', num2str(NumDriveCycles));
                MuxPortHdls = get_param(MuxBlk, 'PortHandles');
                MuxPortHdls = MuxPortHdls.Inport(1:end);
                set_param(MuxBlk, 'Position', [370, 0, 375, 120*(NumDriveCycles)])
                
                % Update 
                for i = 1:NumDriveCycles
                    NewBlkName = LookupBlkFcn(i);
                
                    % Add new subsystems
                    add_block('LibExpRefLookup/ExpRefLookup', NewBlkName)
                    MuxPortPos = get_param(MuxPortHdls(i), 'Position');
                    BlkH = 60;
                    set_param(NewBlkName, 'Position',  [0, MuxPortPos(2)-BlkH/2, 160, MuxPortPos(2)+BlkH/2]);
                                       
                    % Connect blocks
                    NewLookupPortHdls = get_param(NewBlkName, 'PortHandles');
                    for j = 1:length(LookupInportHdls)
                        add_line(DriveCycleLookupSubsystem, LookupInportHdls(j), NewLookupPortHdls.Inport(j));
                    end
                    add_line(DriveCycleLookupSubsystem, NewLookupPortHdls.Outport(1), MuxPortHdls(i));
                end
            end
%             %% Calculate mask data 
%             MaskData.EnableDistCompensation = DistCompensationCheckbox;
%             MaskData.NumCycleVals = 6;
%             MaskData.NumCycles = NumDriveCycles;
%             % Add drive cyle data to MaskData
%             for i = 1:length(DriveCycleStructArray)
%                 MaskData.(['DriveCycleData', num2str(i)]) = DriveCycleStructArray(i);
%             end

%         end
        
%         %% getDriveCycleSummary
%         function SummaryTbl = getDriveCycleSummary(obj)
%             % SummaryTbl = getDriveCycleSummary(obj) returns a summary of
%             % the drive cycle information entered in to the 'VSR Drive Cycle
%             % Source' block.
%             DriveCycleStructArray = obj.getParamValue('DriveCycleStructArray');
%             
%             CycleNum = 1:1:length(DriveCycleStructArray);
%             CycleNum = CycleNum(:);
%             CycleName = {DriveCycleStructArray.Name}';
%             
%             SummaryTbl = table(CycleNum, CycleName);
%         end
%     end
% end
