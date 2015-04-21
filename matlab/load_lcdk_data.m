function [ out ] = load_lcdk_data( fn, height, width)
%LOAD_LCDK_DAT Loads a specified .dat file as exported from CCS
% The function also reshapes the image with the specified height and width
% Don't forget to remove the first line of metadata beforehand!

% (This code is the way it is because of how MATLAB reshapes data...)
out = load(fn);
out = out';
out = reshape(out, [width height]);
out = out';

end
