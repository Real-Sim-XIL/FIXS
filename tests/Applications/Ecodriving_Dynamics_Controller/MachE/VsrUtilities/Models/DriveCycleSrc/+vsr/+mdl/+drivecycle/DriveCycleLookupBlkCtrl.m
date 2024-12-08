classdef DriveCycleLookupBlkCtrl < vsr.mdl.BlkMaskCtrl
    % Use to control 'VsrLibDriveCycleSrc/Drive Cycle Lookup' mask
    % parameters and init.
    
    %% Public methods
    methods 
        %% Init
        function MaskData = Init(obj)
            % Get drive cycle struct
            DriveCycleStruct = obj.getParamValue('DriveCycleStruct');
            
            % Set up ref vel
            set_param([obj.FullBlkName, '/VehRefSpd Units'], 'Unit',DriveCycleStruct.VelRef.TblUnits)
            MaskData = [];
            
            % Set up grade
            if DriveCycleStruct.Grade.UseTimeGradeBpts
                set_param([obj.FullBlkName, '/GradeBpt DistUnits'], 'Unit','m')
            else
                set_param([obj.FullBlkName, '/GradeBpt DistUnits'], 'Unit',DriveCycleStruct.Grade.BptUnits)
            end
            set_param([obj.FullBlkName, '/GradeTbl Units'], 'Unit',DriveCycleStruct.Grade.TblUnits)
        end
    end
    
end