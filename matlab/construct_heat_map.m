function [ heat ] = construct_heat_map( fname )
%CONSTRUCT_HEAT_MAP Constructs a heat map from a fingerprint image

% THIS IS FOR JPEGS RIGHT NOW

 % Load image as black-and-white
img = load_bw_jpg(fname);

% Run median filter on image
img = median_filter(img);

% Convert image to binary
bin = to_binary(img);
bin = invert_binary(bin);

% Do Zhang-Suen line thinning
bin = zhang_suen(bin);
% Do the improvement skeletonization
bin = zs_8conn(bin);

% Invert the binary again
bin = invert_binary(bin);

% Get a minutiae map for the image
map = minutiae_cn_map(bin);

% Get a heat map for the image
heat = minutiae_heat_map(map);

end

