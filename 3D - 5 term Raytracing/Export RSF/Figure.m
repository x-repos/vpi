function noname = Figure(th,x,z,velocity,hor,setofhor)
switch nargin
  case 6
  figure;
  imagesc(x,z,velocity{th});
  colorbar;
  xlabel('Lateral Distance (m)','FontSize',13);
  ylabel('Time (ms)','FontSize',13);
  title('Velocity Model','FontSize',13)
  hold on
  for i=setofhor(th,:)
    p=plot(hor{i}.data(:,2),hor{i}.data(:,3),'');
    p.LineWidth=1;
    p.Color='r';
  end
  noname=true;
  case 4
  figure;
  imagesc(x,z,velocity{th});
  colorbar;
  xlabel('Lateral Distance (m)','FontSize',13);
  ylabel('Time (ms)','FontSize',13);
  title('Velocity Model','FontSize',13)
  hold on    
end
end