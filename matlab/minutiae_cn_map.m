function [ map ] = minutiae_cn_map( img )
%MINUTIAE_MAP Creates a crossing number minutiae map from the given image
%   Input must be a 2D image with lines 0 and whitespace 1
%   Crossing numbers:
%       0: Isolated point
%       1: Ridge ending
%       2: Continuing ridge
%       3: Bifurcation
%       4: Crossing

[m,n] = size(img);
map = zeros(m,n);

for i = 1+1:m-1
    for j = 1+1:n-1
        z = 0;
        if img(i,j) == 0
            z = z + abs(img(i,j+1) - img(i-1,j+1));
            z = z + abs(img(i-1,j+1) - img(i-1,j));
            z = z + abs(img(i-1,j) - img(i-1,j-1));
            z = z + abs(img(i-1,j-1) - img(i,j-1));
            z = z + abs(img(i,j-1) - img(i+1,j-1));
            z = z + abs(img(i+1,j-1) - img(i+1,j));
            z = z + abs(img(i+1,j) - img(i+1,j+1));
            z = z + abs(img(i+1,j+1) - img(i,j+1));
            z = z/2;
        end
        map(i,j) = z;
    end
end

end

