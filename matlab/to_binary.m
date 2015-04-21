function [ out_img ] = to_binary( in_img )
%TO_BINARY Converts a 0-255 matrix to a 0/1 matrix

% Get image dimensions
[m,n] = size(in_img);
out_img = zeros(m,n);

% Step through image
THRESHOLD = 245;
WHITE = 1;
BLACK = 0;
for y = 1:m
    for x = 1:n
        if in_img(y,x) >= THRESHOLD
            out_img(y,x) = WHITE;
        else
            out_img(y,x) = BLACK;
        end
    end
end

end

