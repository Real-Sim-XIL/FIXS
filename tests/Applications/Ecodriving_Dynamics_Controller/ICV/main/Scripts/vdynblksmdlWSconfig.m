function [varargout]= vdynblksmdlWSconfig(varargin)
%   Copyright 2018 The MathWorks, Inc.
block = varargin{1};
if nargin >1
    userDef = varargin{2};
else
    userDef = true;
end
varargout{1} = {};
simStopped = autoblkschecksimstopped(block,true);
if simStopped
    rootModel = bdroot(block);
    ws = get_param(rootModel, 'modelworkspace');
    VEH = getVariable(ws,'VEH');
    if userDef
        ParamList={'X_o',[1,1],{};'Y_o',[1,1],{};'Z_o',[1,1],{};'phi_o',[1,1],{'gte',-pi/2;'lte',pi/2};'theta_o',[1,1],{'gte',-pi/2;'lte',pi/2};'psi_o',[1,1],{'gte',-2*pi;'lte',2*pi}};
        paramStruct = autoblkscheckparams(block, '3D Engine',ParamList);
    else
        paramStruct = vdynblks3Dsceneconfig(block);
    end
    VEH.InitialLongPosition = paramStruct.X_o;
    VEH.InitialLatPosition = paramStruct.Y_o;
    VEH.InitialVertPosition = paramStruct.Z_o;
    VEH.InitialRollAngle = paramStruct.phi_o;
    VEH.InitialPitchAngle = paramStruct.theta_o;
    VEH.InitialYawAngle = paramStruct.psi_o;    
    assignin(ws,'VEH',VEH);
    modelList = {'PassVeh14DOF','PassVeh7DOF'};
    for idx =1:length(modelList)
        load_system(modelList{idx});
        ws = get_param(modelList{idx}, 'modelworkspace');
        assignin(ws,'VEH',VEH);
        save_system(modelList{idx});
        close_system(modelList{idx});
    end
    disp('Model workspaces updated.')
end