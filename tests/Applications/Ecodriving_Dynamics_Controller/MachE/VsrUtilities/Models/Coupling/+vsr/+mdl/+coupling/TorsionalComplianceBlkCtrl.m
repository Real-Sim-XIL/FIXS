classdef TorsionalComplianceBlkCtrl < vsr.mdl.BlkMaskCtrl
    % TorsionalComplianceBlock handles callbacks for Torsional Compliance block
    
    %% Public methods
    methods
        %% Constructor
        function obj = TorsionalComplianceBlkCtrl(Block)
            obj@vsr.mdl.BlkMaskCtrl(Block);
            
        end
        
        %% calcStiffness
        function k = calcStiffness(obj)
            StiffnessCalcMethod = get_param(obj.BlkHdl, 'StiffnessCalcMethodPopup');
            switch StiffnessCalcMethod
                case 'None'
                    k = obj.getParamValue('k');
                case 'Shaft dimensions and material properties'
                    G = obj.getParamValue('G');
                    L = obj.getParamValue('L_shaft');
                    D_o = obj.getParamValue('D_o');
                    D_i = obj.getParamValue('D_i');
                    J = pi/32*(D_o^4 - D_i^4);
                    k = J*G/L;
                    set_param(obj.BlkHdl, 'k', num2str(k))
            end
            
        end
        
        %% calcDamping
        function b = calcDamping(obj)
            k = obj.calcStiffness;
            DampingCalcMethod = get_param(obj.BlkHdl, 'DampingCalcMethodPopup');
            switch DampingCalcMethod
                case 'None'
                    b = obj.getParamValue('b');
                case 'Damping ratio'
                    J = obj.getParamValue('Jcalc');
                    zeta = obj.getParamValue('zeta');
                    b = zeta*sqrt(2*k*J);
                    set_param(obj.BlkHdl, 'b', num2str(b))
                    wn = sqrt(k/J);
                    wd = sqrt(1-min(1, zeta)^2)*wn;
                    set_param(obj.BlkHdl, 'wn', num2str(wn))
                    set_param(obj.BlkHdl, 'wd', num2str(wd))
            end
        end
        
        
    end
    
    %% Static methods
    methods (Static)
        %% runInit
        function runInit(Block)
            obj = vsr.mdl.coupling.TorsionalComplianceBlkCtrl(Block);
            obj.updateMask(Block);
        end
        %% calcBlkValues
        function calcBlkValues(Block)
            obj = vsr.mdl.coupling.TorsionalComplianceBlkCtrl(Block);
            obj.calcStiffness;
            obj.calcDamping; 
        end
        %% updateMask
        function updateMask(Block)
            %% Setup
            obj = vsr.mdl.coupling.TorsionalComplianceBlkCtrl(Block);
            StiffnessCalcMethod = get_param(obj.BlkHdl, 'StiffnessCalcMethodPopup');
            DampingCalcMethod =  get_param(obj.BlkHdl, 'DampingCalcMethodPopup');
            
            %% Stiffness calculation
            switch StiffnessCalcMethod
                case 'None'
                    obj.setVisible({'D_o', 'D_i', 'L_shaft', 'G'}, 'off')
                    obj.setEnabled('k', 'on')
                case 'Shaft dimensions and material properties'
                    obj.setVisible({'D_o', 'D_i', 'L_shaft', 'G'}, 'on')
                    obj.setEnabled('k', 'on')
            end
            
            %% Damping calculation
            switch DampingCalcMethod
                case 'None'
                    obj.setVisible({'Jcalc', 'zeta', 'wn', 'wd'}, 'off')
                    obj.setEnabled('b', 'on')
                case 'Damping ratio'
                    obj.setVisible({'Jcalc', 'zeta', 'wn', 'wd'}, 'on')
                    obj.setEnabled('b', 'on')
            end
        end
    end
    
end
