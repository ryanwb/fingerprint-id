#ifndef MEDIAN_FILTER_H
#define MEDIAN_FILTER_H

#include "img_params.h"
#include "m_mem.h"

#define padding 2
#define windowXdim 3
#define windowYdim 3
#define MEDIANELEMENT (0.5)*windowXdim*windowYdim
#define WINDOWSIZE windowXdim*windowYdim

//BUBBLE SORT (REPLACEMENT FOR QSORT)
void bubble_sort(unsigned char a[], int len)
{
    // "Rising" bubble sort
    while (len > 0) {
        int swapped = 0;
        int i = 0;
        for (i = 0; i < len - 1; i++) {
            if (a[i] > a[i+1]) {
                // Swap
            	unsigned char x = a[i+1];
                a[i+1] = a[i];
                a[i] = x;
                swapped = 1;
            }
        }
        if (!swapped)
            break;
        --len;
    }
}

unsigned char * med_filter(unsigned char *image, int xDim, int yDim)
{
    //add padding to dimensions
    int paddedXDim=xDim+padding;
    int paddedYDim=yDim+padding;
    
    //padded matrix
    unsigned char *paddedMatrix=(unsigned char*)m_malloc(paddedXDim*paddedYDim*sizeof(unsigned char));
    int i,j;
    for(i=0;i<paddedYDim;i++)
    {
        for (j=0; j<paddedXDim; j++)
        {
            //initialize padded matrix to 0
            *(paddedMatrix +i*paddedXDim+j)=0;
        }
    }
    
    //copy original image onto the padded matrix
    int k,l;
    for(k=0;k<yDim;k++)
    {
        for (l=0; l<xDim; l++)
        {
            //initialize padded matrix to 0
            *(paddedMatrix +(k+1)*xDim+(l+1))=*(image +k*xDim+l);
        }
    }
    //TODO CHECK IF OUTPUT LOOKS SOMETHING LIKE
    /*
     000000000000
     011111111110
     011111111110
     011111111110
     011111111110
     011111111110
     011111111110
     000000000000
     */
    
    //create a 3x3 window
    unsigned char *window=(unsigned char*)m_malloc(windowXdim*windowYdim*sizeof(unsigned char));
    unsigned char *output=(unsigned char*)m_malloc(xDim*yDim*sizeof(unsigned char));
    
    int a,b;
    for(a=0;a<yDim; a++)
    {
        for(b=0;b<xDim; b++)
        {
            int c,d;
            for(c=0;c<windowYdim; c++)
            {
                for(d=0; d<windowXdim; d++)
                {
                    // copy the paddedmatrix into the window
                    *(window+c*windowXdim+d)=*(paddedMatrix+(a+c-1)*xDim+(b+d-1));
                }
            }
            //qsort(window,WINDOWSIZE,sizeof(int),cmpfunc);
            
            //for each window, find the median element of the window & copy to output
            bubble_sort(window, WINDOWSIZE);
            *(output+a*xDim+b)=window[(int)MEDIANELEMENT];
        }
    }
    //FREE MEMORY FOR TEMP MATRICIES, TODO: DO I NEED TO FREE *image (input)???
    m_free(image);
    m_free(window);
    m_free(paddedMatrix);
    return output;
}

#endif
