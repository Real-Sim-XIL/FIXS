clear all

D=cmread('debug_elevation_192455.erg');

Mn = {};
M = [];

Dnames = fieldnames(D);
for i=1:numel(Dnames)
    Mn = [Mn Dnames{i}];
    M = [M D.(Dnames{i}).data'];
end

T = array2table(M);

T.Properties.VariableNames = Mn;

writetable(T, '..\..\..\..\tests\SignalIpg\debug_cm_data3.csv')