function [ img ] = load_bw_jpg( fname )
%LOAD_BW_JPG Loads jpg into an m x n matrix of black and white pixels
%   Load image and get it into a binary black and white array (one value
%   per pixel), as it will be on the LCDK for image processing.

% Load image and greyscale it (flatten m x n x 3 to just m x n)
rgb = imread(fname);
img = rgb2gray(rgb);

% Get image dimensions
[m,n] = size(img);

% Step through and threshold to turn the greyscale to a black-and-white
THRESHOLD = 128;
WHITE = 255;
BLACK = 0;
for y = 1:m
    for x = 1:n
        if img(y,x) >= THRESHOLD
            img(y,x) = WHITE;
        else
            img(y,x) = BLACK;
        end
    end
end

end

