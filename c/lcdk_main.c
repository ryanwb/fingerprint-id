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
#include "zhang_suen.h"
#include "zs_8conn.h"
#include "minutiae_cn_map.h"
#include "heat.h"
#include <stdio.h>
#include <math.h>

// Number of files
#define N 12

unsigned char* bitmap[N];
int width[N];
int height[N];
unsigned char* binary;
unsigned char* median;
unsigned char* zs_skeleton;
unsigned char* skeleton;
int* cn_map;
heat_t heat[N/2];
heat_t new_heat;

interrupt void interrupt4(void)
{
	output_left_sample(0);
}

int main(void)
{
	mem_init();

	char* files[N] = {
			"C:\\Users\\EE113D\\MyProjects\\fingerprint-id\\1a.bmp",
			"C:\\Users\\EE113D\\MyProjects\\fingerprint-id\\2a.bmp",
			"C:\\Users\\EE113D\\MyProjects\\fingerprint-id\\3a.bmp",
			"C:\\Users\\EE113D\\MyProjects\\fingerprint-id\\4a.bmp",
			"C:\\Users\\EE113D\\MyProjects\\fingerprint-id\\5a.bmp",
			"C:\\Users\\EE113D\\MyProjects\\fingerprint-id\\6a.bmp",
			"C:\\Users\\EE113D\\MyProjects\\fingerprint-id\\1b.bmp",
			"C:\\Users\\EE113D\\MyProjects\\fingerprint-id\\2b.bmp",
			"C:\\Users\\EE113D\\MyProjects\\fingerprint-id\\3b.bmp",
			"C:\\Users\\EE113D\\MyProjects\\fingerprint-id\\4b.bmp",
			"C:\\Users\\EE113D\\MyProjects\\fingerprint-id\\5b.bmp",
			"C:\\Users\\EE113D\\MyProjects\\fingerprint-id\\6b.bmp"
	};

	int k;

	int fid;

	for (k = 0; k < N; k++)
	{
		printf("Reading bitmap...\n");
		bitmap[k] = imread(files[k]);

		width[k] = (InfoHeader.Width);
		height[k] = (InfoHeader.Height);
		printf("Finished reading bitmap %d; height: %d, width: %d\n", k, height[k], width[k]);
	}

	printf("Enter any number to initialize heatmap database: ");
	scanf("%d", &fid);

	for (k = 0; k < N/2; k++)
	{
		printf("\nProcessing bitmap %d...\n", k);
		printf("Converting bitmap to greyscale...\n");
		binary = to_greyscale(bitmap[k], width[k], height[k]); // load the image in black & white

		// Calculate the threshold to use for black and white conversion
		printf("Calculating black and white threshold...\n");
		unsigned char bw_threshold = find_threshold(binary, width[k], height[k]);

		printf("Binarizing... threshold: %d\n", bw_threshold);
		binarize(binary, width[k], height[k], bw_threshold); // binarize the image (in-place)
		upsidedown(binary, width[k], height[k]); // flip it upside down (in-place)

		printf("Running median filter...\n");
		median = med_filter(binary, width[k], height[k]);
		m_free(binary);

		// Invert the pixels for skeletonization
		invert_binary(median, width[k], height[k]);

		printf("Skeletonizing...\n");
		zs_skeleton = zhang_suen(height[k], width[k], median);
		m_free(median);

		printf("8-Conn skeletonizing...\n");
		skeleton = zs_8conn(height[k], width[k], zs_skeleton);
		m_free(zs_skeleton);

		// Invert back
		invert_binary(skeleton, width[k], height[k]);

		printf("Creating heatmap...\n");

		// Make CN map
		cn_map = minutiae_cn_map(skeleton, width[k], height[k]);

		// Make a heat map from the CN map
		heat[k] = create_heatmap(cn_map, height[k], width[k]);

		printf("Complete!\n");
	}

	printf("\nAll %d heatmaps created!\n", N/2);

	while(1)
	{
		printf("\nEnter a fingerprint image number to load: ");
		scanf("%d", &fid);

		printf("Processing bitmap %d...\n", fid);
		printf("Converting bitmap to greyscale...\n");
		binary = to_greyscale(bitmap[fid], width[fid], height[fid]); // load the image in black & white

		// Calculate the threshold to use for black and white conversion
		printf("Calculating black and white threshold...\n");
		unsigned char bw_threshold = find_threshold(binary, width[fid], height[fid]);

		printf("Binarizing... threshold: %d\n", bw_threshold);
		binarize(binary, width[fid], height[fid], bw_threshold); // binarize the image (in-place)
		upsidedown(binary, width[fid], height[fid]); // flip it upside down (in-place)

		printf("Running median filter...\n");
		median = med_filter(binary, width[fid], height[fid]);
		m_free(binary);

		// Invert the pixels for skeletonization
		invert_binary(median, width[fid], height[fid]);

		printf("Skeletonizing...\n");
		zs_skeleton = zhang_suen(height[fid], width[fid], median);
		m_free(median);

		printf("8-conn skeletonizing...\n");
		skeleton = zs_8conn(height[fid], width[fid], zs_skeleton);
		m_free(zs_skeleton);

		// Invert back
		invert_binary(skeleton, width[fid], height[fid]);

		printf("Creating heatmap...\n");

		// Make CN map
		cn_map = minutiae_cn_map(skeleton, width[fid], height[fid]);

		// Make a heat map from the CN map
		new_heat = create_heatmap(cn_map, height[fid], width[fid]);

		printf("Complete!\n");

		int best_match = -1;
		float best_score = 999.0;

		int i;
		for (i = 0; i < N/2; i++) {
			float this_score = compute_match_score(new_heat, heat[i]);
			printf("%d-%d score: %.4f\n", fid, i, this_score);
			if (this_score < best_score) {
				best_score = this_score;
				best_match = i;
			}
		}

		printf("\nFound match: %d. Enter actual match: ", best_match);
		scanf("%d", &fid);

		merge_heatmaps(&heat[fid], &new_heat);

		printf("Updated fingerprint database\n");

		free_heatmap_body(&new_heat);
	}

/*
	for (k = 0; k < N; k++)
		free_heatmap_body(&heat[k]);

	printf("Done...\n");

	// L138_initialise_intr(FS_8000_HZ,ADC_GAIN_0DB,DAC_ATTEN_0DB,LCDK_LINE_INPUT);

	while(1)
		continue;
*/
}
