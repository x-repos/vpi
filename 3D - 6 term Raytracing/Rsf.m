function Rsf(name,x,y,z,dx,dy,dz,horizon)

f=fopen(name,'w');
fprintf(f,'echo in=velocity.txt n1=%d n2=%d n3=%d data_format=ascii_float >velocity.rsf\n',...
  length(z),length(x),length(y));
fprintf(f,'sfdd <velocity.rsf form=native >velocity1.rsf\n');
fprintf(f,'<velocity1.rsf sfpad d1=%f d2=%f d3=%f o1=0 o2=0 o3=0 >velocity2.rsf\n',dz,dx,dy);
fprintf(f,'<velocity2.rsf sfpad label1="Depth-z" label2="Distance-x" label3="Distance-y">velocity3.rsf\n');
fprintf(f,'<velocity3.rsf sfpad unit1="m" unit2="m" unit3="m">velocity4.rsf\n');
fprintf(f,'rm velocity1.rsf velocity.rsf velocity2.rsf velocity3.rsf\n');
fprintf(f,"sfmath <velocity4.rsf output='input' > velocity.rsf\n");
fprintf(f,'rm velocity4.rsf\n');
fprintf(f,'echo in=horizon.txt n1=%d n2=%d data_format=ascii_float >horizon.rsf\n',...
  size(horizon,2),size(horizon,1));
fprintf(f,'sfdd <horizon.rsf form=native >horizon1.rsf\n');
fprintf(f,"sfmath <horizon1.rsf output='input' > horizon.rsf\n");
fprintf(f,'rm horizon1.rsf');
fclose all;

end