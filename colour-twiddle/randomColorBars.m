function img = randomColorBars(x,y,n)

%x_resolution = 848;
%y_resolution = 480;
x_resolution = x;
y_resolution = y;

img = zeros(y_resolution, x_resolution, 3);

step = round(x_resolution/(n));
brick = ones(y_resolution, step);
for i = 1:n+round(0.5*n)
  if(mod(i,2) == 0)
      r = 50 + randi(205,1);
      g = 50 + randi(205,1);
      b = 50 + randi(205,1);
      img(:,1+(i-1)*step:i*step,1) = r.*brick;
      img(:,1+(i-1)*step:i*step,2) = g.*brick;
      img(:,1+(i-1)*step:i*step,3) = b.*brick;
  end
  end
  img = img(1:y_resolution,1:x_resolution,:);
  img = uint8(img);
  imshow(img);
  filename = sprintf('colourbars-%d.bmp',n);
  imwrite(img,filename);
end