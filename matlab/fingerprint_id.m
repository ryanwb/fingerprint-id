% set(0, 'DefaultAxesFontSize', 18);
% set(0, 'DefaultAxesFontName', 'Bistream Vera Sans Mono')

% Clear workspace
clear all;

%images = {'fingerprint-vector.jpg', ...
%          'russ1.JPG', 'russ2.JPG', 'russ3.JPG', ...
%          'ryan1.JPG', 'ryan2.JPG', 'ryan3.JPG', ...
%          '104_2.tif', '104_6.tif'};

images = {'104_2.tif'};

% Preallocate figures? Not sure how to do this...
% figures = gobjects(1, length(images));

for i = 1 : length(images)
    
    % Load image as black-and-white
    [token, remain] = strtok(images{i}, '.');
    if strcmpi(remain, '.tif') == 1
        img = load_bw_tif(images{i});
    elseif strcmpi(token, 'fingerprint-vector') == 1
        img = load_bw_tif(images{i});
    else
        img = load_bw_jpg(images{i});
    end
    
    figures(i) = figure('Position', [100, 100, 1024, 200]);
    set(0, 'currentfigure', figures(i));
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

