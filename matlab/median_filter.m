function [ B ] = median_filter( A )
%MEDIAN_FILTER Runs the median filter noise removal algorithm
%   Algorithm Source: http://angeljohnsy.blogspot.com/2011/03/2d-median-filtering-for-salt-and-pepper.html

%PAD THE MATRIX WITH ZEROS ON ALL SIDES
modifyA=zeros(size(A)+2);
B=zeros(size(A));

%COPY THE ORIGINAL IMAGE MATRIX TO THE PADDED MATRIX
for x=1:size(A,1)
    for y=1:size(A,2)
        modifyA(x+1,y+1)=A(x,y);
    end
end

      %LET THE WINDOW BE AN ARRAY
      %STORE THE 3-by-3 NEIGHBOUR VALUES IN THE ARRAY
      %SORT AND FIND THE MIDDLE ELEMENT
     
for i= 1:size(modifyA,1)-2
    for j=1:size(modifyA,2)-2
        window=zeros(9,1);
        inc=1;
        for x=1:3
            for y=1:3
                window(inc)=modifyA(i+x-1,j+y-1);
                inc=inc+1;
            end
        end        
        med=sort(window);
        %PLACE THE MEDIAN ELEMENT IN THE OUTPUT MATRIX
        B(i,j)=med(5);      
    end
end

%CONVERT THE OUTPUT MATRIX TO 0-255 RANGE IMAGE TYPE
B=uint8(B);

end

