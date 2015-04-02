function [ thin ] = zhang_suen( img )
%ZHANG_SUEN Performs Zhang-Suen line thinning on an input binary array

[m,n] = size(img);
thin = img;
did_change = 1;

% Note: pts_to_remove will be a linked list in C? or can just use an array

while did_change == 1
    p = 1;
    did_change = 0;
    pts_to_remove = zeros(m * n, 2);
    
    % Do the first subiteration
    for i = 1+1:m-1
        for j = 1+1:n-1
            if thin(i,j) == 1 && subcond_one(thin,i,j) == 1
                pts_to_remove(p,:) = [i j];
                did_change = 1;
                p = p + 1;
            end
        end
    end

    % Remove points found in first subiteration
    for x = 1:p-1
        thin(pts_to_remove(x,1), pts_to_remove(x,2)) = 0;
    end
    p = 1;
    
    % Do the second subiteration
    for i = 1+1:m-1
        for j = 1+1:n-1
            if thin(i,j) == 1 && subcond_two(thin,i,j) == 1
                pts_to_remove(p,:) = [i j];
                did_change = 1;
                p = p + 1;
            end
        end
    end
    
    % Remove points found in second subiteration
    for x = 1:p-1
        thin(pts_to_remove(x,1), pts_to_remove(x,2)) = 0;
    end
end

end

function [ s ] = subcond_one( img, i, j )
%SUBCOND_ONE Determines if the first Zhang-Suen subcondition is true
    s = 0;
    a = A(img,i,j);
    b = B(img,i,j);
    if 2 <= b && b <= 6 && a == 1
        if img(i-1,j) * img(i,j+1) * img(i+1,j) == 0
            if img(i,j+1) * img(i+1,j) * img(i,j-1) == 0
                s = 1;
            end
        end
    end
end

function [ s ] = subcond_two( img, i, j )
%SUBCOND_TWO Determines if the second Zhang-Suen subcondition is true
    s = 0;
    a = A(img,i,j);
    b = B(img,i,j);
    if 2 <= b && b <= 6 && a == 1
        if img(i-1,j) * img(i,j+1) * img(i,j-1) == 0
            if img(i-1,j) * img(i+1,j) * img(i,j-1) == 0
                s = 1;
            end
        end
    end
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

function [ a ] = A( img, i, j )
%A Gets the Zhang-Suen A function value for a pixel img(i,j)

a = 0;

% P2 -> P3
if img(i-1,j) == 0 && img(i-1,j+1) == 1
    a = a + 1;
end
% P3 -> P4
if img(i-1,j+1) == 0 && img(i,j+1) == 1
    a = a + 1;
end
% P4 -> P5
if img(i,j+1) == 0 && img(i+1,j+1) == 1
    a = a + 1;
end
% P5 -> P6
if img(i+1,j+1) == 0 && img(i+1,j) == 1
    a = a + 1;
end
% P6 -> P7
if img(i+1,j) == 0 && img(i+1,j-1) == 1
    a = a + 1;
end
% P7 -> P8
if img(i+1,j-1) == 0 && img(i,j-1) == 1
    a = a + 1;
end
% P8 -> P9
if img(i,j-1) == 0 && img(i-1,j-1) == 1
    a = a + 1;
end
% P9 -> P2
if img(i-1,j-1) == 0 && img(i-1,j) == 1
    a = a + 1;
end

end