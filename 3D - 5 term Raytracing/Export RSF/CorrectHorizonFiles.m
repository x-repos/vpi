function [horNew]=CorrectHorizonFiles(hor)
%% INITIAL VARIABLES
nhorizon=size(hor,2);
%% FIND SINGULARITY FILES
cdpstep=hor{1}.data(100,2)-hor{1}.data(99,2);
for i=1:nhorizon
  for j=1:10
    if hor{i}.data(j+1,2)-hor{i}.data(j,2) ~= cdpstep
      hor{i}.data(j,:)=[];
    end
  end
  for j=length(hor{i}.data):-1:length(hor{i}.data)-10
    if hor{i}.data(j,2)-hor{i}.data(j-1,2) ~= cdpstep
      hor{i}.data(j,:)=[];
    end
  end
end

%% MAKE FILES THE SAME SIZE
% Head of file
tmp = hor{1}.data(1,2);
for i = 2:nhorizon
  if tmp > hor{i}.data(1,2)
    hor{i}.data(1,2)
    tmp = hor{i}.data(1,2);
  end
end
for i=1:nhorizon
  if tmp < hor{i}.data(1,2)
    for j=((hor{i}.data(1,2)-tmp)/cdpstep):-1:1
      hor{i}.data=[0,tmp+(j-1)*cdpstep,hor{i}.data(1,3);hor{i}.data];
    end
  end
end

for i=1:nhorizon
  while hor{i}.data(1,2) ~= 0
    hor{i}.data=[0,hor{i}.data(1,2)-cdpstep,hor{i}.data(1,3);hor{i}.data];
  end
end

% End of file
maxlength=length(hor{1}.data);
for i=2:nhorizon
  if maxlength<length(hor{i}.data)
    maxlength=length(hor{i}.data);
  end
end
for i=1:nhorizon
  if length(hor{i}.data)<maxlength
    for j=length(hor{i}.data)+1:maxlength
      hor{i}.data=[hor{i}.data;0,j*cdpstep,hor{i}.data(end,3)];
    end
  end
end
horNew=hor;
end