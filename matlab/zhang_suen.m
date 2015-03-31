function [ thin ] = zhang_suen( img )
%ZHANG_SUEN Performs Zhang-Suen line thinning on an input binary array

[m,n] = size(img);

thin = 0;

end

function [ a ] = A( img, i, j )
%A Gets the Zhang-Suen A function value for a pixel img(i,j)

a = 0;

end

function [ b ] = B( img, i, j )
%B Gets the Zhang-Suen B function value for a pixel img(i,j)

b = 0;

end