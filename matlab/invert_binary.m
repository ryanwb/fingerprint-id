function [ out_img ] = invert_binary( in_img )
%INVERT_BINARY Inverts a binary image

% Get image dimensions
[m,n] = size(in_img);
out_img = zeros(m,n);

% Step through image
for y = 1:m
    for x = 1:n
        if in_img(y,x) == 0
            out_img(y,x) = 1;
        else
            out_img(y,x) = 0;
        end
    end
end

end

