function [ heat ] = minutiae_heat_map( cn )
%MINUTIAE_HEAT_MAP Creates a minutiae heat map from a crossing number map

% heat.count is the number of merged heatmaps that this map represents
% heat.map is the heatmap itself, with peak values ranging from 0 to 255
% e.g. heat.map(i,j).ridgeending is the height of the ridgeending peak
% at pixel i,j

[m,n] = size(cn);

x.ridgeending = 0;
x.bifurcation = 0;
x.crossing = 0;

heat.map = repmat(x,m,n);

for i = 1:m
    for j = 1:n
        if cn(i,j) == 1
            heat.map(i,j).ridgeending = 255;
        elseif cn(i,j) == 3
            heat.map(i,j).bifurcation = 255;
        elseif cn(i,j) == 4
            heat.map(i,j).crossing = 255;
        end
    end
end

heat.count = 1;

end

