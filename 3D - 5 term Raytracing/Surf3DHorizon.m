function Surf3DHorizon(horizon,nHorx,nHory)
X=zeros(nHory,nHorx);
Y=zeros(nHory,nHorx);
Z=zeros(nHory,nHorx);
for i=1:nHory
  for j=1:nHorx
    X(i,j)= horizon(j,2);
    Y(i,j)= horizon(487*(i-1)+j,1);
    Z(i,j)= horizon(487*(i-1)+j,3);
  end
end
% figure;
h=surf(X,Y,Z);
alpha(h,0.5);
shading interp;
% colormap ();
% lighting gouraud
% camlight
rotate3d on
title('3D Model');
% set(gca,'PlotBoxAspectRatio',[ 1 1 1 ])
set(gca,'DataAspectRatio',[ 1 1 1 ])
set(gca,'CameraViewAngleMode','manual')
xlabel('Distance-x');ylabel('Distance-y');zlabel('Depth-z');

end