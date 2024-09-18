function Surf3DHorizon(horizon,nHorx,nHory)
figure;
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
surf(X,Y,Z);
shading interp;
colormap winter;
% lighting gouraud
% camlight
rotate3d on
title('3D Horizon');
set(gca,'PlotBoxAspectRatio',[ 1 1 1 ])
set(gca,'CameraViewAngleMode','manual')
xlabel('Distance-x');ylabel('Distance-y');zlabel('Depth-z');
set(gca,'ZDir', 'reverse');

end