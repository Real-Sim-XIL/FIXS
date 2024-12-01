classdef ExpRefBlkCtrl < vsr.mdl.BlkMaskCtrl
    % Use to control 'ExperimentReferenceMeasured'
    % parameters and init.
    
    %% Public methods
    methods
        %% Init
        function MaskData = Init(obj)
            %% Setup
%             % Get drive cycle objects
%             DriveCycleStructArray = obj.getParamValue('DriveCycleStructArray');
%             DistCompensationCheckbox = obj.getParamValue('DistCompensationCheckbox');
            
%             obj = vsr.mdl.BlkMaskCtrl('EvBoltCalib_Veh_v0/ExperimentReferenceMeasured');
%             DdDriveCycle = Simulink.data.dictionary.open('DdExpRefSrc.sldd');
%             DdSection = DdDriveCycle.getSection('Design Data');
%             ExpRefStructs = getValue(getEntry(DdSection, 'ExpRefStructs'));
            
            TestScnrioCycleNum = obj.getParamValue('TestScnrioCycleNum');
            ExpRefStructs = obj.getParamValue('ExpRefStructs');

            temp1 = ExpRefStructs(TestScnrioCycleNum);
            if isempty(temp1.Name)
                RefData.Name = 'N/A';
                RefData.time = [0 1];
                RefData.accelPedalCmd = [0 0];
                RefData.brakePedalCmd = [0 0];
                RefData.accelPedalDBW = [0 0];
                RefData.brakePedalDBW = [0 0];
                RefData.vehSpd = [0 0];
                RefData.battVolt = [0 0];
                RefData.battSoc = [0 0];
                RefData.battCurr = [0 0];
                RefData.axleTorq = [0 0];
                RefData.grade = [0 0];
                RefData.brakeTorq = [0 0];
                RefData.spare3 = [0 0];
                RefData.spare4 = [0 0];
                RefData.spare5 = [0 0];
                RefData.spare6 = [0 0];
                MaskData = RefData;
            else
                MaskData = ExpRefStructs(TestScnrioCycleNum);
            end
           
        end
        
    end
end
