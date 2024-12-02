function [varargout]= vdynblks3Dsceneconfig(varargin)
%   Copyright 2018 The MathWorks, Inc.
block = varargin{1};
defaultStatus = get_param(block,'defaultPos');

if strcmp(defaultStatus,'Recommended for scene')
    sceneType = get_param(block,'SceneDesc');
    switch sceneType
        case 'Straight road'
            paramStruct.X_o = -118;
            paramStruct.Y_o = 3.125;
            paramStruct.Z_o = 0;
            paramStruct.phi_o = 0;
            paramStruct.theta_o = 0;
            paramStruct.psi_o = 0;
        case 'Curved road'
            paramStruct.X_o = 0.2;
            paramStruct.Y_o = -1605.0;
            paramStruct.Z_o = 0;
            paramStruct.phi_o = 0;
            paramStruct.theta_o = 0;
            paramStruct.psi_o = -156*pi/180;
        case 'Parking lot'
            paramStruct.X_o = -104.0;
            paramStruct.Y_o = -9.7;
            paramStruct.Z_o = 0;
            paramStruct.phi_o = 0;
            paramStruct.theta_o = 0;
            paramStruct.psi_o = 0;
        case 'Double lane change'
            paramStruct.X_o = 0;
            paramStruct.Y_o = 3.125;
            paramStruct.Z_o = 0;
            paramStruct.phi_o = 0;
            paramStruct.theta_o = 0;
            paramStruct.psi_o = 0;
        case 'Open surface'
            paramStruct.X_o = 0;
            paramStruct.Y_o = 0;
            paramStruct.Z_o = 0;
            paramStruct.phi_o = 0;
            paramStruct.theta_o = 0;
            paramStruct.psi_o = 0;
        case 'US highway'
            paramStruct.X_o = 3592.00;
            paramStruct.Y_o = 2617.00;
            paramStruct.Z_o = -1.0;
            paramStruct.phi_o = 0;
            paramStruct.theta_o = 0;
            paramStruct.psi_o = 0;
        case 'US city block'
            paramStruct.X_o = -125.19;
            paramStruct.Y_o = 1.65;
            paramStruct.Z_o = -0.04;
            paramStruct.phi_o = 0;
            paramStruct.theta_o = 0;
            paramStruct.psi_o = 0;
        case 'Large parking lot'
            paramStruct.X_o = 45.0;
            paramStruct.Y_o = 54.7;
            paramStruct.Z_o = 0;
            paramStruct.phi_o = 0;
            paramStruct.theta_o = 0;
            paramStruct.psi_o =-pi/2;
        case 'Virtual Mcity'
            paramStruct.X_o = -26.0;
            paramStruct.Y_o = 76.0;
            paramStruct.Z_o = 0.0;
            paramStruct.phi_o = 0;
            paramStruct.theta_o = 0;
            paramStruct.psi_o =-40*pi/180;
        otherwise
            paramStruct.X_o = 0;
            paramStruct.Y_o = 0;
            paramStruct.Z_o = 0;
            paramStruct.phi_o = 0;
            paramStruct.theta_o = 0;
            paramStruct.psi_o = 0;
    end
else
    ParamList={'X_o',[1,1],{};'Y_o',[1,1],{};'Z_o',[1,1],{};'phi_o',[1,1],{'gte',-pi/2;'lte',pi/2};'theta_o',[1,1],{'gte',-pi/2;'lte',pi/2};'psi_o',[1,1],{'gte',-2*pi;'lte',2*pi}};
    paramStruct = autoblkscheckparams(block, '3D Engine',ParamList);
    
end
try % to avoid promoted error
set_param(block,'X_o', num2str(paramStruct.X_o))
set_param(block,'Y_o', num2str(paramStruct.Y_o))
set_param(block,'Z_o', num2str(paramStruct.Z_o))
set_param(block,'phi_o', num2str(paramStruct.phi_o))
set_param(block,'theta_o', num2str(paramStruct.theta_o))
set_param(block,'psi_o', num2str(paramStruct.psi_o))
catch
end
varargout(1) = {paramStruct};
end