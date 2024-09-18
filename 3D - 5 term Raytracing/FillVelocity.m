function vel=FillVelocity(x,z,horizon,setofhor,vhor)
%% README
% Linear Regression - Apply boundary condition
%% INITIAL VARIABLES VARIABLES
velocity=zeros(length(z),length(x)); %velocity grid
numberofhorizon=length(setofhor);
horNew=cell(1,numberofhorizon);
vhorNew=zeros(1,numberofhorizon);
%% SET NEW INDEX OF VARIABLES
k=0;
for i=setofhor
  k=k+1;
  horNew{k}=horizon{i};
  vhorNew(k)=vhor(i);
end
numberofhorizonNew=k;
lengthx=size(velocity,2);
lengthz=size(velocity,1);
for i=1:lengthx
  velocity(1,i)=vhorNew(1);
end
for i=1:lengthx
  velocity(end,i)=vhorNew(end);
end

%% SET VELOCITY ON HORIZON
for i=1:numberofhorizonNew
  velocity=VelocityOnHorizon(velocity,horNew{i}.data,x,z,vhorNew(i));
end
%% SET VELOCITY ON ALL FIELD 
for i=1:lengthx
  k=0;
  ihor=zeros(1);
  for j=1:lengthz
    if velocity(j,i)~=0
      k=k+1;
      ihor(k)=j;
    end
  end
  for k1=1:k-1
    velocity(ihor(k1):ihor(k1+1),i)=linspace(velocity(ihor(k1),i),...
      velocity(ihor(k1+1),i),ihor(k1+1)-ihor(k1)+1);
  end
end
%% UPDATE VELOCITY
vel=velocity;
end