#ifndef IMG_UTILS_H
#define IMG_UTILS_H

#include "img_params.h"
#include "m_mem.h"
#include "mean_median_utils.h"

/* find_threshold
 * Finds the "threshold" greyscale value to be used for later black and white conversion
 */
unsigned char find_threshold(unsigned char* image, int width, int height)
{
	return find_mean(image, width * height) - 15;
}

/* to_bw
 * Converts a BMP image with RGB values 0-255 to a 0/255 matrix
 */
unsigned char* to_bw(unsigned char* bmp, int width, int height, unsigned char threshold)
{
	unsigned int N = width * height;
	unsigned char* b = m_malloc(N * sizeof(unsigned char));
	unsigned int i;
	for (i = 0; i < N * 3; i += 3) {
		if (bmp[i] < threshold && bmp[i+1] < threshold && bmp[i+2] < threshold)
			b[i/3] = BLACK;
		else
			b[i/3] = WHITE;
	}
	return b;
}

/* to_greyscale
 * Converts a BMP image with RGB values 0-255 to a 0-255 greyscale matrix
 */
unsigned char* to_greyscale(unsigned char* bmp, int width, int height)
{
	unsigned int N = width * height;
	unsigned char* b = m_malloc(N * sizeof(unsigned char));
	unsigned int i;
	for (i = 0; i < N * 3; i += 3)
		b[i/3] = bmp[i]/3 + bmp[i+1]/3 + bmp[i+2]/3;
	return b;
}

/* binarize
 * Converts a greyscale matrix with values 0-255 to a 0/1 matrix (in-place)
 */
void binarize(unsigned char* img, int width, int height, unsigned char threshold)
{
	int N = width * height;
	int i;
	for (i = 0; i < N; i++)
		img[i] = img[i] < threshold ? BBLACK: BWHITE;
}


/* upsidedown
 * Flips an image across its horizontal axis (in-place)
 * We do this because bitmap indexing starts at the bottom left;
 * however, we want 0,0 to be the top left pixel instead
 */
void upsidedown(unsigned char* img, int width, int height)
{
	int i, j;
	for (i = 0; i < height/2; i++) {
		for (j = 0; j < width; j++) {
			// Swap the upper-half and lower-half pixels
			unsigned char temp = img[i*width + j];
			img[i*width + j] = img[(height-1-i)*width + j];
			img[(height-1-i)*width + j] = temp;
		}
	}
}

/* upsidedown
 * Switches 1s to 0s and 0s to 1s in an image (in-place)
 * We do this because some algorithms consider a filled pixel
 * to be a 0 while others consider a filled pixel to be a 1
 */
void invert_binary(unsigned char* img, int width, int height)
{
	int i, j;
	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			if (img[i*width + j] == 0)
				img[i*width + j] = 1;
			else
				img[i*width + j] = 0;
		}
	}
}

#endif
