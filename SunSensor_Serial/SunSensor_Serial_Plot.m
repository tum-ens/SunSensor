% 	Source code for SunSensor in serial application with MATLAB to plot the data
% 	Written by Patrick Buchenberg patrick.buchenberg@tum.de
% 	Version2 - 14.06.2021

clear; close all; clc;
%% Parameter
b = -0.8;
bereich = 100;
Rmess = 2e3;
%% Create serial object for Arduino
serialportlist
s = serialport(serialportlist,115200);
%% Vector declaration
SensorPos = [1  0  0;   % Sensor n: [x_n, y_n ,z_n]'
             0  -1 0;   % Sensor 2
             -1 0  0;
             0  1  0;
             0  0  1;
             % 2. Version
%              1  -1  1;  % Sensor 6
%              1   1  1;
%              -1  1  1;
%              -1 -1  1
             ]';     
%% Kalibrierung
summe=0;
cnt=0;
flush(s)
for i=1:50
    data = readline(s);
    C = textscan(data, '%f');
    output = cell2mat(C).';
    if(length(output)==size(SensorPos,2))
        summe = summe+output;
        cnt = cnt + 1;
    end
end
    output = summe/cnt;
    R = (2^10./output-1)*Rmess;
    lux = R.^(1/b);
    lux = lux*30e4;

    ScaledMatrix = lux.*SensorPos;
    Offset = sum(ScaledMatrix,2);
      
%% Messurements
figure(1)
flush(s)
while 1
% scale vector with data
    data = readline(s);
    C = textscan(data, '%f');
    output = cell2mat(C).'
    if(max(output)>1000)
        disp('Warnung ADC')
    end
%     output(5) = 0;
%     output = [1:9];
%     output = ones(1,9);
    
    if(length(output)==size(SensorPos,2))
        R = (2^10./output-1)*Rmess;
        lux = R.^(1/b);
        lux = lux*30e4;
        
        ScaledMatrix = lux.*SensorPos;
        Sun = sum(ScaledMatrix,2);
        SunKali = Sun-Offset;
        
        winkel_ebene = rad2deg(atan2(SunKali(2),SunKali(1)))
        winkel_hoehe = rad2deg(atan2(SunKali(3),sqrt(SunKali(1)^2+SunKali(2)^2)))
        betrag = norm(SunKali)

        %% Plot Data
        figure(1)
        zpos = zeros(5,1);
        quiver3(zpos,zpos,zpos,ScaledMatrix(1,:).', ScaledMatrix(2,:).', ScaledMatrix(3,:).','-b');
        xlim([-bereich bereich])
        ylim([-bereich bereich])
        zlim([-bereich bereich])
        xlabel('x \rightarrow')
        ylabel('y \rightarrow')
        zlabel('z \rightarrow')
        hold on
        quiver3(0,0,0, Sun(1), Sun(2), Sun(3), '--g')
        quiver3(0,0,0, SunKali(1), SunKali(2), SunKali(3) ,'-y')
        hold off
        %legend('S1', 'S2', 'S3');
        title('Lichtsensor v3 - 3D')      
        legend('Sensor', 'Sun', 'SunKali');
%         hold on
%         compass(SunVektor, 'r')
%         hold off
        drawnow limitrate
        %pause(1)
        flush(s)
    %     figure(2)
    %     compass(SunVektor)
    %     drawnow limitrate
    else
        disp('Fehler')
    end
end