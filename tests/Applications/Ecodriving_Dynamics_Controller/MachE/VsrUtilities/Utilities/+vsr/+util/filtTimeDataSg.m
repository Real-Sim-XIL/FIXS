function [DataFiltered, DataDeriv1] = filtTimeDataSg(Time, Data, FilterTimeWindow, FilterOrder, EqualTimeStepTol)
% filtTimeDataSg(Time,Data,FilterTimeWindow, FilterOrder) filters data and
% calculates derivative using Savitzky-Golay Filter
% FilterTimeWindow can include future and/or past values. Example:
% FilterTimeWindow = 2 uses a values 1 second ahead and future values to
% fit data. FilterTimeWindow = [-1, 2] uses values 1 second behind and 2
% seconds in the future to calculate.

%% Set up
if length(FilterTimeWindow) == 1
    FilterTimeWindow = abs(FilterTimeWindow);
    FilterTimeWindow = [-FilterTimeWindow, FilterTimeWindow]/2;
end
if nargin < 5
    EqualTimeStepTol = eps(10);
end
DataFiltered = zeros(size(Time));
DataDeriv1 = DataFiltered;
AllIndices = 1:length(Time);

Time = Time(:);
Data = Data(:);

%% Filter data

CurrIdx = 1;

while CurrIdx <= length(Time)
    TimeWindow = Time(Time >= Time(CurrIdx)+FilterTimeWindow(1) & Time <= Time(CurrIdx) + FilterTimeWindow(2));
    
    % Find next constant time step
    if Time(CurrIdx)+FilterTimeWindow(1) < Time(1) ||...
            Time(CurrIdx) + FilterTimeWindow(2) > Time(end)||...
            any(abs(diff(diff(TimeWindow))) > EqualTimeStepTol) % Start and end out of range
        [DataFiltered(CurrIdx), DataDeriv1(CurrIdx)] = filterUnequalSpacing(Time, Data, FilterTimeWindow, FilterOrder, CurrIdx);
        CurrIdx = CurrIdx + 1;
    else % Constant time step
        IndexWindow = AllIndices(Time >= Time(CurrIdx)+FilterTimeWindow(1) & Time <= Time(CurrIdx) + FilterTimeWindow(2));
        EndWindowIndex = IndexWindow(end) + 1;
        CurrTimeStep = Time(IndexWindow(end)) - Time(IndexWindow(end-1));
        NextTimeStep = CurrTimeStep;
        EndIdx = CurrIdx;
        while abs(NextTimeStep - CurrTimeStep) < EqualTimeStepTol && EndWindowIndex <= length(Time)
            NextTimeStep = Time(EndWindowIndex) - Time(EndWindowIndex-1);
            EndWindowIndex = EndWindowIndex + 1;
            EndIdx = EndIdx + 1;
        end
        EndIdx = EndIdx - 1;
        Indices = unique(CurrIdx:EndIdx);
        if length(Indices) > 1
            [DataFiltered(Indices), DataDeriv1(Indices)] = filterEqualSpaced(Time, Data, FilterTimeWindow, FilterOrder, Indices);
            CurrIdx = EndIdx+1;
        else
            [DataFiltered(CurrIdx), DataDeriv1(CurrIdx)] = filterUnequalSpacing(Time, Data, FilterTimeWindow, FilterOrder, CurrIdx);
            CurrIdx = CurrIdx + 1;
        end
        
    end
end
end
%% filterUnequalSpacing
function [DataFiltered, DataDeriv1] = filterUnequalSpacing(Time, Data, FilterTimeWindow, FilterOrder, Indices)
%% Set up
if nargin < 5
    Indices = [1:length(Time)]';
end
Time = Time(:);
Data = Data(:);
Indices = Indices(:)'; 
%% Filter data
DataFiltered = zeros(length(Indices), 1);
DataDeriv1 = DataFiltered;
OutputIdx = 1;
for i = Indices
    if Time(i) + FilterTimeWindow(1) < Time(1)
        WindowOffset = Time(1) - Time(i) - FilterTimeWindow(1);
    elseif Time(i) + FilterTimeWindow(2) > Time(end)
        WindowOffset = (Time(end) - Time(i) - FilterTimeWindow(2));
    else
        WindowOffset = 0;
    end
   
    CurrTimeWindow = FilterTimeWindow + WindowOffset;
    
    IdxWindow = Time > (Time(i)+CurrTimeWindow(1)) & Time < (Time(i)+CurrTimeWindow(2));
    TimeWindow = Time(IdxWindow);
    DataWindow = Data(IdxWindow);
    
    if abs(max(DataWindow) - min(DataWindow)) > eps(10)
        [PolyCoeffs, ~, TimeScaleVals] = polyfit(TimeWindow, DataWindow, FilterOrder);
        TimeScaled = (Time(i) - TimeScaleVals(1))/TimeScaleVals(2);
        DataFiltered(OutputIdx) = polyval(PolyCoeffs, TimeScaled);
        DerivCoeffs = getDerivPolyCoeff(PolyCoeffs, TimeScaleVals);
        DataDeriv1(OutputIdx) = polyval(DerivCoeffs, TimeScaled);
    else
        DataFiltered(OutputIdx) = mean(DataWindow);
        DataDeriv1(OutputIdx) = 0;
    end
    
    OutputIdx = OutputIdx + 1;
end

end

%% getDerivPolyCoeff
function DerivPolyCoeffs = getDerivPolyCoeff(PolyCoeffs,TimeScaleVals)
    Order = length(PolyCoeffs) - 1;
    DerivPolyCoeffs = zeros(length(PolyCoeffs) - 1, 1);
    for i = 1:Order
        DerivPolyCoeffs(i) = PolyCoeffs(i)*(Order-i+1);
    end
    DerivPolyCoeffs = DerivPolyCoeffs/TimeScaleVals(2);
end

%% filterEqualSpaced
function [DataFilt, DataDeriv] = filterEqualSpaced(Time, Data, FilterTimeWindow, FilterOrder, Indices)
% Estimate derivative using Savitzki-Golay filter

% Set up
DiffTime = mean(diff(Time(Indices)));
Data = Data(:);

% Calculate coeff
IndexWindow = round(FilterTimeWindow/DiffTime);
z = IndexWindow(1):IndexWindow(2);
FiltNumPts = length(z);
J = zeros(FiltNumPts, FilterOrder+1);

for i = 1:size(J,1)
    for j = 1:size(J,2)
        J(i,j) = z(i)^(j-1);
    end
end
Coeffs = (J'*J)\J';
CoeffsDeriv = Coeffs(2:end,:).*repmat([1:FilterOrder]', 1, size(Coeffs,2));

% Filter values
DataFilt = zeros(size(Data(Indices)));
DataDeriv = zeros(size(Data(Indices)));

% Filter values inside z range
OutputIdx = 1;
for i = Indices
    ValsWindow = Data(z+i);
    DataFilt(OutputIdx) = sum(Coeffs*ValsWindow);
    DataDeriv(OutputIdx) = sum(CoeffsDeriv*ValsWindow)/DiffTime;  
    OutputIdx = OutputIdx + 1;
end

end