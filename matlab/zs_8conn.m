function [ out ] = zs_8conn( in )
%ZS_8CONN Skeletonization improvement after Zhang-Suen algorithm

% Improvement/test: try getting the image to single-pixel width

[m,n] = size(in);
out = in;

for i = 1+1:m-1
    for j = 1+1:n-1
        if out(i,j) == 1
            if B(out,i,j) > 2
                % if removing the point breaks 8-connectivity,
                % don't actually remove the point
                out(i,j) = 0;
                if is_8conn(out,i,j) == 0
                    out(i,j) = 1;
                end
            end
        end
    end
end

end

function [ r ] = is_8conn( img, i, j )
n_black = 0;
for y = i-1:i+1
    for x = j-1:j+1
        if img(y,x) == 1
            n_black = n_black + 1;
        end
    end
end
for y = i-1:i+1
    for x = j-1:j+1
        if img(y,x) == 1
            n = flood_count(img,i,j,y,x);
            if n_black == n
                r = 1;
                return;
            else
                r = 0;
                return;
            end
        end
    end
end
end

function [ n ] = flood_count(img,i,j,y,x)
    % y and x are current positions
    % i and j are center-of-frame positions
    if ~ (i-1 <= y && y <= i+1)
        n = 0;
        return;
    end
    if ~ (j-1 <= x && x <= j+1)
        n = 0;
        return;
    end
    if img(y,x) == 0
        n = 0;
        return;
    end
    nimg = img;
    nimg(y,x) = 0;
    n = 1 + flood_count(nimg,i,j,y-1,x-1) + flood_count(nimg,i,j,y-1,x) + ...
            flood_count(nimg,i,j,y-1,x+1) + flood_count(nimg,i,j,y,x-1) + ...
            flood_count(nimg,i,j,y,x+1) + flood_count(nimg,i,j,y+1,x-1) + ...
            flood_count(nimg,i,j,y+1,x) + flood_count(nimg,i,j,y+1,x+1);
end


function [ b ] = B( img, i, j )
%B Gets the Zhang-Suen B function value for a pixel img(i,j)

b = 0;
b = b + img(i-1, j);
b = b + img(i-1, j+1);
b = b + img(i, j+1);
b = b + img(i+1, j+1);
b = b + img(i+1, j);
b = b + img(i+1, j-1);
b = b + img(i, j-1);
b = b + img(i-1, j-1);

end
