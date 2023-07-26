clear all

fid = fopen("Trips.2021-07-26.xml");

fileID = fopen("Trips.2021-07-26_mod.xml",'w');

rng(42);
tline = fgetl(fid);
i = 1;
while ischar(tline)
    %disp(tline)
    if ~contains(tline, '<trip')
        if contains(tline, '<vType')
            tline = strrep(tline,'vClass="passenger"',...
                'vClass="passenger" lcCooperative="0" lcSpeedGain="0" lcKeepRight="0"');
        end
        fprintf(fileID, '%s\n', tline);
    elseif ~( contains(tline, 'from="-1527') || contains(tline, 'from="1650') ...
            || contains(tline, 'from="-17275') || contains(tline, 'from="17235') ...
            || contains(tline, 'from="-17234') || contains(tline, 'from="17315') )
        temp = tline;
        if contains(tline, 'from="-16480')
            temp = strrep(tline,'from="-16480','from="-1527');
            if rand(1) < 0.1; temp = []; end % roll the dice
        elseif contains(tline, 'from="17276')
            temp = strrep(tline,'from="17276','from="17331');
            if rand(1) < 0.3; temp = []; end % roll the dice
        elseif contains(tline, 'from="31312')
            if rand(1) < 0.5; temp = []; end % roll the dice
        elseif contains(tline, 'to="-16228')
            if rand(1) < 0.7; temp = []; end % roll the dice
        elseif contains(tline, 'from="-929')
            if rand(1) < 0.1; temp = []; end % roll the dice
        end
        if ~isempty(temp)
            temp = strrep(temp,'color="blue"','color="yellow"');
            fprintf(fileID, '%s\n', temp);
        end 
    end
    tline = fgetl(fid);
    i = i+1;
end
fclose(fid);

fclose(fileID);
