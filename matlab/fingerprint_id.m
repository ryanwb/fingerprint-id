% set(0, 'DefaultAxesFontSize', 18);
% set(0, 'DefaultAxesFontName', 'Bistream Vera Sans Mono')

% Clear workspace
clear all;

images = {'russ1.JPG', 'russ2.JPG', 'russ3.JPG', ...
          'ryan1.JPG', 'ryan2.JPG', 'ryan3.JPG'};

for i = 1 : length(images)

    % Load image as black-and-white
    img = load_bw_jpg(images{i});
    figure(i);
    subplot(1,3,1);
    imshow(img);

    % Run median filter on image
    img = median_filter(img);

    % Display the median-filtered image
    subplot(1,3,2);
    imshow(img);

    % Convert image to binary
    bin = to_binary(img);
    bin = invert_binary(bin);

    % Do Zhang-Suen line thinning
    bin = zhang_suen(bin);
    % Do the improvement skeletonization
    bin = zs_8conn(bin);

    % Invert the binary again
    bin = invert_binary(bin);

    % Convert to black and white and display the line-thinned image
    img = to_bw(bin);
    subplot(1,3,3);
    imshow(img);

    % Get a minutiae map for the image
    map = minutiae_cn_map(bin);

    % Find minutiae
    hold on;
    [m,n] = size(map);
    for i = 1:m
        for j = 1:n
            if map(i,j) == 1      % ridge ending
                plot(j,i,'ro', 'MarkerSize', 10);
            elseif map(i,j) == 3  % bifurcation
                plot(j,i,'bo', 'MarkerSize', 10);
            elseif map(i,j) == 4  % crossing
                plot(j,i,'go', 'MarkerSize', 10);
            end
        end
    end
    
end

