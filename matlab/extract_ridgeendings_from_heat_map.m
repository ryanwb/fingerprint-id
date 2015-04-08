function [ re ] = extract_ridgeendings_from_heat_map( heat )
%EXTRACT_RIDGEENDINGS_FROM_HEAT_MAP

[m,n] = size(heat.map);
re = zeros(size(m,n));
for i = 1:m
    for j = 1:n
        re(i,j) = heat.map(i,j).ridgeending;
    end
end
end

