clear all;
InitialLongPosition = -28;
InitialLatPosition = 0;
InitialV = 20;
open_system('EvBoltCalib_Lat_v1_comb_openControl');
set_param(gcs, 'SimulationCommand','start','SimulationCommand','pause');
%t = tcpclient('192.168.1.68',6666);
t = tcpclient('10.1.114.35',6666);
all_data = [];

while 1
    % TCP receiving
    while(1) % loop, until getting some data
        nBytes = get(t,'BytesAvailable');
        disp('try');
        if nBytes > 0
            break;
        end
    end
    
    command_rev = read(t,nBytes); % read() will read binary as str
    data = str2num(char(command_rev)); % transform str into numerical matrix
    all_data = [all_data;data]; % store history data
    if isempty(data)
        data = [0,0,0];
    end
    data1 = data(1);
    data2 = data(2);
    data3 = data(3);
    
    set_param('EvBoltCalib_Lat_v1_comb_openControl/Accel','Value',num2str(data1));
    set_param('EvBoltCalib_Lat_v1_comb_openControl/Decel','Value',num2str(data2));
    set_param('EvBoltCalib_Lat_v1_comb_openControl/Steer','Value',num2str(data3));

    % TCP sending
    u1 = out.EvBoltCalib.signals.values(end,1);
    u2 = out.EvBoltCalib.signals.values(end,end-1);
    u3 = out.EvBoltCalib.signals.values(end,end);
    v = [u1,u2,u3];
    write(t,v);

    % run the simulink model for one step
    set_param(gcs, 'SimulationCommand','step');
end
set_param(gcs,'SimulationCommand','stop');