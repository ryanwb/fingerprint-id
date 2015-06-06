#ifndef IMG_UTILS_H
#define IMG_UTILS_H

#include "img_params.h"
#include "m_mem.h"
#include "mean_median_utils.h"

/* adaptive_binarize
 * Performs Bradley Adaptive Thresholding
 * Credit: Robert Theis (rmtheis, https://github.com/rmtheis)
 * bin is the output; it must be preallocated before the function call
 */
void adaptive_binarize(unsigned char* input, unsigned char* bin, int width, int height)
{
    // Parameters of the thresholding
    int S = width/20;
    float T = 0.04f;

    unsigned long* integral_img = 0;
    int i, j;
    int index;
    int x1, y1, x2, y2;
    int s2 = S/2;

    int sum;
    int count = 0;

    integral_img = (unsigned long*)m_malloc(width * height * sizeof(unsigned long));

    // Make the integral image
    for (i = 0; i < width; i++)
    {
        sum = 0;
        for (j = 0; j < height; j++)
        {
            index = j * width + i;
            sum += input[index];
            if (i == 0)
                integral_img[index] = sum;
            else
                integral_img[index] = integral_img[index - 1] + sum;
        }
    }

    // Do the actual thresholding
    for (i = 0; i < width; i++)
    {
        for (j = 0; j < height; j++)
        {
            index = j * width + i;

            // Set the SxS region
            x1 = i - s2;
            x2 = i + s2;
            y1 = j - s2;
            y2 = j + s2;

            // Adjust for out-of-bounds
            if (x1 < 0)
                x1 = 0;
            if (x2 >= width)
                x2 = width - 1;
            if (y1 < 0)
                y1 = 0;
            if (y2 >= height)
                y2 = height - 1;
            
            count = (x2 - x1) * (y2 - y1);

            sum = integral_img[y2 * width + x2] -
                  integral_img[y1 * width + x2] -
                  integral_img[y2 * width + x1] +
                  integral_img[y1 * width + x1];

            if ((long)(input[index] * count) < (long)(sum * (1.0 - T)))
                bin[index] = BBLACK;
            else
                bin[index] = BWHITE;
        }
    }
    m_free(integral_img);
}

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
