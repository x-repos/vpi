function velocity=ExportVelocity(Input,name,n)
velocity=Input{1};
for i=1+1:n
  velocity=[velocity;Input{i}]; %#ok
end
f=fopen(name,'w');
for i=1:size(velocity,1)
  for j=1:size(velocity,2)
    fprintf(f,'%10.3f',velocity(i,j));
  end
  fprintf(f,'\n');
end
end
