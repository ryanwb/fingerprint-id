#ifndef MEDIAN_FILTER_H
#define MEDIAN_FILTER_H

#include "img_params.h"
#include "m_mem.h"

#define padding 2
#define windowXdim 3
#define windowYdim 3
#define MEDIANELEMENT 4
#define WINDOWSIZE (windowXdim*windowYdim)

// Bubble sort (our replacement for quicksort on the LCDK)
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
    // Add padding to dimensions
    int paddedXDim=xDim+padding;
    int paddedYDim=yDim+padding;
    
    // Padded matrix
    unsigned char *paddedMatrix=(unsigned char*)m_malloc(paddedXDim*paddedYDim*sizeof(unsigned char));
    int i,j;
    for(i=0;i<paddedYDim;i++)
    {
        for (j=0; j<paddedXDim; j++)
        {
            // Initialize padded matrix to 0
            *(paddedMatrix +i*paddedXDim+j)=0;
        }
    }
    
    // Copy original image onto the padded matrix
    int k,l;
    for(k=0;k<yDim;k++)
    {
        for (l=0; l<xDim; l++)
        {
            // Initialize padded matrix to 0
            *(paddedMatrix +(k+1)*xDim+(l+1))=*(image +k*xDim+l);
        }
    }
    
    // Create a 3x3 window
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
                    // Copy the paddedmatrix into the window
                    *(window+c*windowXdim+d)=*(paddedMatrix+(a+c-1)*xDim+(b+d-1));
                }
            }
            // Previously: qsort(window,WINDOWSIZE,sizeof(int),cmpfunc);
            // For each window, find the median element of the window & copy to output
            bubble_sort(window, WINDOWSIZE);
            *(output+a*xDim+b)=window[(int)MEDIANELEMENT];
        }
    }

    // Free memory for temp matricies
    m_free(window);
    m_free(paddedMatrix);
    return output;
}

#endif
