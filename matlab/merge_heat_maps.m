% WORK IN PROGRESS

function [ heat ] = merge_heat_maps( heat_one, heat_two )
%MERGE_HEAT_MAPS Merges two minutiae heat maps with a weighted average

[m1,n1] = size(heat_one.map);
[m2,n2] = size(heat_two.map);
m = min(m1,m2);
n = min(n1,n2);

heat.count = heat_one.count + heat_two.count;

x.ridgeending = 0;
x.bifurcation = 0;
x.crossing = 0;

heat.map = repmat(x,m,n);

for i = 1:m
    for j = 1:n
        heat.map(i,j).ridgeending = heat_one.count/heat.count * heat_one.map(i,j).ridgeending + ...
                                    heat_two.count/heat.count * heat_two.map(i,j).ridgeending;
        heat.map(i,j).bifurcation = heat_one.count/heat.count * heat_one.map(i,j).bifurcation + ...
                                    heat_two.count/heat.count * heat_two.map(i,j).bifurcation;
        heat.map(i,j).crossing = heat_one.count/heat.count * heat_one.map(i,j).crossing + ...
                                 heat_two.count/heat.count * heat_two.map(i,j).crossing;
    end
end

end

