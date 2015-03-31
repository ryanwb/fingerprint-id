% Line thinning algorithm demonstration

% set(0, 'DefaultAxesFontSize', 18);
% set(0, 'DefaultAxesFontName', 'Bistream Vera Sans Mono')

% Clear workspace
clear all;

% Load image as black-and-white
img = load_bw_jpg('symbol.jpg');
% imshow(img);

% Convert image to binary
bin = to_binary(img);
bin = invert_binary(bin);

% Do Zhang-Suen line thinning
thin = zhang_suen(bin);