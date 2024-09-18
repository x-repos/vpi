clear;clc;close all;tic;
%% FORMAT INPUT

% ProMAX Data Export: List/Table - 212_hor-01_wb
% Area: 2021_training    Line: hapth-crooked_
%   LINE_NO       CDP         Z
% ---------+---------+---------+---------+---------+---------+
%  12292829         5    102.86
%  12292829        10    102.87
%  12292829        15    102.87
%  12292829        20    102.88
%% README

%  Helloworld \m/(>.<)\m/
%% INITIAL VARIABLES

% Interpolation horizon: horizon 2 and horizon 3
% Same value of 2500

horizonInputName      = '213_hor-0';
velocityInputName     = '213_V3_Interval.txt';
vhor              = [1500,2500,2500,5000,7700];

setofhor(1,:)     = [1,2,4,5];
setofhor(2,:)     = [1,3,4,5];
%% READ HORIZON FILES

nhorizon=length(vhor);
ManualVel=importdata(velocityInputName);
hor = cell(1,nhorizon);
for i = 1:nhorizon
  hor{i} = importdata(strcat(horizonInputName,int2str(i),'.txt'));
end
%% CORRECT HORIZON

hor=CorrectHorizonFiles(hor);
%% SET DIMENSIONS

zmin=ManualVel.data(1,2);
zmax=ManualVel.data(end,2);
xmin=ManualVel.data(1,1);
xmax=ManualVel.data(end,1);
dz=ManualVel.data(2,2)-ManualVel.data(1,2);
z=zmin:dz:zmax;
dx=ManualVel.data(length(z)+1)-ManualVel.data(length(z));
x=xmin:dx:xmax;
dy=dx; ymin=xmin; ymax=10001;
y=ymin:dy:ymax; ymax=max(y);
%% VELOCITY FIELD | ALL BOUNDARY CONDITION

velocityBoundary=cell(1,2);
for i=1:2
  velocityBoundary{i}=FillVelocity(x,z,hor,setofhor(i,:),vhor);
end
velocityCell=cell(1,length(y));
for i=1:length(y)
  velocityCell{i}=(velocityBoundary{end}-velocityBoundary{1})...
    ./(length(y)-1).*(i-1)+velocityBoundary{1};
end
% #[xz][xz][xz]...[xz]
velocity=velocityCell{1};
for i=1+1:length(y)
  velocity=[velocity,velocityCell{i}]; %#ok
end
%% EXPORT 3D VELOCITY FILE

%                       [zx]
%  Velocity rsf format  [zx]
%                       ...
%                       [zx]


velocityFile=velocity';
f=fopen('velocity.txt','w');
for i=1:size(velocityFile,1)
  for j=1:size(velocityFile,2)
    fprintf(f,'%10.3f',velocityFile(i,j));
  end
  fprintf(f,'\n');
end
%% SLICE 3D VELOCITY
Slice3DVelocity(x,y,z,velocityFile);

%% PlOT FRONT AND BEHIND VELOCITY

% for i=1:2
%   Figure(i,x,z,velocitycell,hor,setofhor);
% end
%% DISCRETIZED HORIZON AT BOUNDARY

cdpStep=hor{1}.data(100,2)-hor{1}.data(99,2);
stepSampling=10;
cdpstepDiscretized=cdpStep*stepSampling;
horDiscretized=cell(1,nhorizon);
for j=1:nhorizon
  k=0;
  for i=1:length(hor{j}.data)
    if mod(i-1,stepSampling)==0
      k=k+1;
      horDiscretized{j}.data(k,:)=hor{j}.data(i,:);
    end
  end
end
%% DISCRETIZED HORIZON

for i=1:length(horDiscretized{1}.data)
  if horDiscretized{1}.data(i,2)>max(y)
    nHory=i;
    break
  end
end
nHorx=length(horDiscretized{1}.data);
horizonCell=cell(1,nHory);
for i=1:nHory
  horizonCell{i}=(horDiscretized{4}.data-horDiscretized{3}.data)...
    ./(nHory-1).*(i-1)+horDiscretized{3}.data;
  horizonCell{i}(:,1)=[];
  horizonCell{i}=horizonCell{i}';
end
% #[][][]...[]
horizon=horizonCell{1};
horizonFile = 0*cdpstepDiscretized*ones(1,nHorx);
for i=1:nHory
  horizon=[horizon,horizonCell{i}]; %#ok
  horizonFile=[horizonFile,i*cdpstepDiscretized*ones(1,nHorx)]; %#ok %Add y
end
horizon=[horizonFile;horizon];
horizon=horizon';
%% SURF HORIZON

Surf3DHorizon(horizon,nHorx,nHory)
%% EXPORT DISCRETIZED HORIZON
clear horizonFile
horizonFile=zeros(nHory,nHorx);
for i=1:nHory
  for j=1:nHorx
    horizonFile(i,j)=horizon((i-1)*nHorx+j,3);
  end
end

f=fopen('horizon.txt','w');
for i=1:size(horizonFile,1)
  fprintf(f,'%10.3f',horizonFile(i,:));
  fprintf(f,'\n');
end
% set(gca,'DataAspectRatio',[ 1 1 1 ]);

%% SH RUN

Rsf('convertrsf',x,y,z,dx,dy,dz,horizonFile,cdpstepDiscretized);
