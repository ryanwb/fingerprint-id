//
//  main.c
//  113DTesting
//  medianFilter
//  MEDIAN FILTER C IMPLEMENTATION
//
#include <stdlib.h>
#include <stdio.h>


#define padding 2
#define windowXdim 3
#define windowYdim 3
#define MEDIANELEMENT (0.5)*windowXdim*windowYdim
#define WINDOWSIZE windowXdim*windowYdim

//matrix printing helper function
void printMatrix(int r, int c, int * window)
{
    int i,j;
    for(i=0; i< r; i++)
    {
        for(j=0; j< c; j++)
        {
            printf("%i",*(window+i*c+j));
        }
        printf("\n");
    }
}

//comparison helper function for qsort
/*int cmpfunc (const void * a, const void * b)
 {
 return ( *(int*)a - *(int*)b );
 }*/

//BUBBLE SORT (REPLACEMENT FOR QSORT)
void bubble_sort(int a[], int len)
{
    // "Rising" bubble sort
    while (len > 0) {
        int swapped = 0;
        for (int i = 0; i < len - 1; i++) {
            if (a[i] > a[i+1]) {
                // Swap
                int x = a[i+1];
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

int * med_filter(int xDim, int yDim, int *image)
{
    //add padding to dimensions
    int paddedXDim=xDim+padding;
    int paddedYDim=yDim+padding;
    
    //padded matrix
    int *paddedMatrix=(int*)malloc(paddedXDim*paddedYDim*sizeof(int));
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
    int *window=(int*)malloc(windowXdim*windowYdim*sizeof(int));
    int *output=(int*)malloc(xDim*yDim*sizeof(int));
    
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
    free(image);
    free(window);
    free(paddedMatrix);
    return output;
}


