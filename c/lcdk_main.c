/*
 * This is the main program for the LCDK implementation of fingerprint-id!
 */

// This line tells the compiler to use m_malloc, m_free instead of malloc, free
#define LCDK_BUILD

#include "L138_LCDK_aic3106_init.h"
//#include "evmomapl138_gpio.h"
#include "m_mem.h"
#include "bmp.h"
#include "img_utils.h"
#include "median_filter.h"
#include <stdio.h>
#include <math.h>

unsigned char* binary;
unsigned char* bitmap;
unsigned char* median;

interrupt void interrupt4(void)
{
	output_left_sample(0);
}

int main(void)
{
	mem_init();
	printf("Reading bitmap...\n");
	bitmap = imread("C:\\Users\\EE113D\\MyProjects\\fingerprint-id\\104_2.bmp");
	int width = InfoHeader.Width;
	int height = InfoHeader.Height;
	printf("Finished reading bitmap; height: %d, width: %d\n", height, width);

	binary = to_bw(bitmap, width, height); // load the image in black & white
	m_free(bitmap);
	binarize(binary, width, height); // binarize the image (in-place)
	upsidedown(binary, width, height); // flip it upside down (in-place)

	median = med_filter(binary, width, height);

	printf("Done!\n");

	// L138_initialise_intr(FS_8000_HZ,ADC_GAIN_0DB,DAC_ATTEN_0DB,LCDK_LINE_INPUT);

	while(1)
		continue;
}
