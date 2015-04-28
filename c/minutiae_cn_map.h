#ifndef MINUTIAE_CN_MAP_H
#define MINUTIAE_CN_MAP_H

#include "m_mem.h"
#include <math.h>

/* minutiae_cn_map
 * Creates a crossing number minutiae map from the given image
 * Input must be a 2D image with lines 0 and whitespace 1
 * Crossing numbers:
 * 		0: Isolated point
 *      1: Ridge ending
 *      2: Continuing ridge
 *      3: Bifurcation
 *      4: Crossing
 */
int* minutiae_cn_map(unsigned char* img, int width, int height)
{
	int* cn_map = m_malloc(width * height * sizeof(int));
	int i, j;

	for (i = 1; i < height - 1; i++) {
		for (j = 1; j < width - 1; j++) {
			int z = 0;
			if (img[i*width + j] == 0) {
				z += abs(img[(i)*width + j+1] - img[(i-1)*width + j+1]);
	            z += abs(img[(i-1)*width + j+1] - img[(i-1)*width + j]);
	            z += abs(img[(i-1)*width + j] - img[(i-1)*width + j-1]);
	            z += abs(img[(i-1)*width + j-1] - img[(i)*width + j-1]);
	            z += abs(img[(i)*width + j-1] - img[(i+1)*width + j-1]);
	            z += abs(img[(i+1)*width + j-1] - img[(i+1)*width + j]);
	            z += abs(img[(i+1)*width + j] - img[(i+1)*width + j+1]);
	            z += abs(img[(i+1)*width + j+1] - img[(i)*width + j+1]);
	            z /= 2;
	        }
	        cn_map[i*width + j] = z;
		}
	}

	return cn_map;
}

#endif
