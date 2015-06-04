#ifndef SOBEL_H
#define SOBEL_H

#include "img_params.h"
#include "m_mem.h"
#include <math.h>

/* sobel_filter
 * Returns a Sobel-filtered image with the same dimensions as the input image
 */
unsigned char* sobel_filter(unsigned char* image, int width, int height)
{
	unsigned char* sobel_out = m_malloc(width * height * sizeof(unsigned char));

	int sobel_x[3][3] = {{-1,  0,  1},
				   	     {-2,  0,  2},
				   	     {-1,  0,  1}};

/*	int sobel_y[3][3] = {{-1, -2, -1},
	           	   	     {0,   0,  0},
	           	   	     {1,   2,  1}}; */

	int sobel_y[3][3] = {{1,   2,   1},
		           	   	 {0,   0,   0},
		           	   	 {-1, -2,  -1}};

	int x, y;
	for (y = 0; y < height; y++) {
		for (x = 0; x < width ; x++) {
		    int pixel_x = (sobel_x[0][0] * image[(y-1)*width + (x-1)]) + (sobel_x[0][1] * image[(y-1)*width + (x)]) + (sobel_x[0][2] * image[(y-1)*width + (x+1)]) +
		              	  (sobel_x[1][0] * image[(y)*width + (x-1)])   + (sobel_x[1][1] * image[(y)*width + (x)])   + (sobel_x[1][2] * image[(y)*width + (x+1)]) +
		              	  (sobel_x[2][0] * image[(y+1)*width + (x-1)]) + (sobel_x[2][1] * image[(y+1)*width + (x)]) + (sobel_x[2][2] * image[(y+1)*width + (x+1)]);

		    int pixel_y = (sobel_y[0][0] * image[(y-1)*width + (x-1)])  + (sobel_y[0][1] * image[(y-1)*width + (x)]) + (sobel_y[0][2] * image[(y-1)*width + (x+1)]) +
		              	  (sobel_y[1][0] * image[(y)*width + (x-1)])    + (sobel_y[1][1] * image[(y)*width + (x)])   + (sobel_y[1][2] * image[(y)*width + (x+1)]) +
		              	  (sobel_y[2][0] * image[(y+1)*width + (x-1)])  + (sobel_y[2][1] * image[(y+1)*width + (x)]) + (sobel_y[2][2] * image[(y+1)*width + (x+1)]);

		    double val = sqrt((double)((pixel_x * pixel_x) + (pixel_y * pixel_y)));

		    int threshold=100;
		    if (val>threshold)
		    	val=1;
		    else
		    	val=0;

		    sobel_out[y*width + x] =val;
		}
	}
	return sobel_out;
}

#endif
