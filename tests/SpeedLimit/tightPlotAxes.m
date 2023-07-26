function AxisOuterPos = tightPlotAxes(nRow, nCol, offsetPos)
%
%   offsetPos = [leftOffset bottomOffset widthOffset heightOffset]
%       Note: All offsetPos values are non-negative
%       leftOffset--cut space to left edge of the figure
%       bottomOffset--cut space to bottom edge of the figure
%       widthOffset--cut space between two column plots 
%       heightOffset--cut space between two row plots

if ~exist('offsetPos', 'var') || isempty(offsetPos)
    offsetPos = [0 0 0 0];
end
 
% if any(offsetPos<-1e-5)
%     error('All OffsetPos must be non-negative')
% end

leftOffset = -offsetPos(1);
bottomOffset = -offsetPos(2);
widthOffset = offsetPos(3);
heightOffset = offsetPos(4);
 
nPlot = nRow * nCol;
plots = 0:(nPlot - 1);
row   = (nRow - 1) - fix(plots(:) / nCol);
col   = rem(plots(:), nCol);
width = (1-nCol*leftOffset)/nCol;
height = (1-nRow*bottomOffset)/nRow;
AxisOuterPos = [leftOffset+col/nCol, ...
    bottomOffset+row/nRow, ...
    width(ones(nPlot, 1), 1)+widthOffset, ...
    height(ones(nPlot, 1), 1)+heightOffset];

% nPlot = nRow * nCol;
% plots = 0:(nPlot - 1);
% row   = (nRow - 1) - fix(plots(:) / nCol);
% col   = rem(plots(:), nCol);
% width = (1-leftOffset)/nCol;
% height = (1-bottomOffset)/nRow;
% AxisOuterPos = [leftOffset+col*width, ...
%     bottomOffset+row*height, ...
%     width(ones(nPlot, 1), 1)+widthOffset, ...
%     height(ones(nPlot, 1), 1)+heightOffset];
