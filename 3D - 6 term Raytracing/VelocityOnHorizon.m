function vel=VelocityOnHorizon(velocity,A,x,z,vhor)
vel=velocity;
for i=1:length(x)-1
  temp=false;
  for j=1:(length(z)-1)      
    for k=1:length(A)
      if A(k,3)>=z(j) && A(k,3)<=z(j+1)...
          && A(k,2)>=x(i) && A(k,2)<=x(i+1)
        vel(j,i)=vhor;
        temp=true;
      end
      if temp; break;
      end
    end
    if temp; break;
    end
  end
%   upperbound1(i)=j; %save the horizon
  %horizone (i*dx; j*dz)
  if i==length(x)-1 && temp
    vel(j,i+1)=vel(j,i);
  end
end
end