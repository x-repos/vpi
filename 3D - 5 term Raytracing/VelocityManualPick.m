clear;clc;close all;tic;
%% FORMAT READ
% ProMAX Data Export: List/Table - 212_hor-01_wb
% Area: 2021_training    Line: hapth-crooked_
%   LINE_NO       CDP         Z
% ---------+---------+---------+---------+---------+---------+
%  12292829         5    102.86
%  12292829        10    102.87
%  12292829        15    102.87
%  12292829        20    102.88

%% INITIAL VARIABLES
nhorizon    = 5;
% HorizonIn      = '212_hor-0';
% VelocityIn     = '212_V3_Interval';
HorizonInput      = '213_hor-0';
VelocityInput     = '213_V3_Interval';

%% READ HORIZON FILES
ManualVel=importdata(VelocityInput);
hor = cell(1,nhorizon);
for i = 1:nhorizon
  hor{i} = importdata(strcat(HorizonInput,int2str(i),'.txt'));
end
%% CORRECT HORIZON
hor=CorrectHorizonFiles(hor);

%% SET xz
zmin=ManualVel.data(1,2);
zmax=ManualVel.data(end,2);
xmin=ManualVel.data(1,1);
xmax=ManualVel.data(end,1);
dz=ManualVel.data(2,2)-ManualVel.data(1,2);
z=zmin:dz:zmax;
dx=ManualVel.data(length(z)+1)-ManualVel.data(length(z));
x=xmin:dx:xmax;

%% Plot Manual Pick Velocity
k=0;
for i=1:length(x)
  for j=1:length(z)
    k=k+1;
    ManualVelResize(j,i)=ManualVel.data(k,3); %#ok
  end
end
figure;
imagesc(x,z,ManualVelResize);
% caxis([1500 7700]);
colorbar;
xlabel('Lateral Distance (m)','FontSize',13);
ylabel('Time (ms)','FontSize',13);
title('Velocity Model','FontSize',15);
hold on
for i=1:nhorizon
  p=plot(hor{i}.data(:,2),hor{i}.data(:,3),'');
  p.LineWidth=1;
  p.Color='red';
end