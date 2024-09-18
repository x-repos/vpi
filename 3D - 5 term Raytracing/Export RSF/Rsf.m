function Rsf(name,x,y,z,dx,dy,dz,horizonFile,cdpstepDiscretized)

f=fopen(name,'w');
fprintf(f,'echo in=velocity.txt n1=%d n2=%d n3=%d data_format=ascii_float >velocity.rsf\n',...
  length(z),length(x),length(y));
fprintf(f,'sfdd <velocity.rsf form=native >velocity1.rsf\n');
fprintf(f,'<velocity1.rsf sfpad d1=%f d2=%f d3=%f o1=%f o2=%f o3=%f >velocity2.rsf\n',dz,dx,dy,min(z),min(x),min(y));
fprintf(f,'<velocity2.rsf sfpad label1="Depth-z" label2="Distance-x" label3="Distance-y">velocity3.rsf\n');
fprintf(f,'<velocity3.rsf sfpad unit1="m" unit2="m" unit3="m">velocity4.rsf\n');
fprintf(f,'rm velocity1.rsf velocity.rsf velocity2.rsf velocity3.rsf\n');
fprintf(f,"sfmath <velocity4.rsf output='input' > velocity.rsf\n");
fprintf(f,'rm velocity4.rsf\n');
fprintf(f,'echo in=horizon.txt n1=%d n2=%d data_format=ascii_float >horizon.rsf\n',...
  size(horizonFile,2),size(horizonFile,1));
fprintf(f,'sfdd <horizon.rsf form=native >horizon1.rsf\n');

fprintf(f,'<horizon1.rsf sfpad d1=%f d2=%f o1=0 o2=0 >horizon2.rsf\n',cdpstepDiscretized,cdpstepDiscretized);
fprintf(f,'<horizon2.rsf sfpad label1="Distance-x" label2="Distance-y">horizon3.rsf\n');
fprintf(f,'<horizon3.rsf sfpad unit1="m" unit2="m" >horizon4.rsf\n');

fprintf(f,'sfmath <horizon4.rsf output="input" > horizon.rsf\n');


fprintf(f,'rm horizon1.rsf horizon2.rsf horizon3.rsf horizon4.rsf');
fclose all;

end