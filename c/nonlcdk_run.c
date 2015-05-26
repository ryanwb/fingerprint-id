/*
 * This is a program to test matching of fingerprints
 * Build me: gcc -Wall -Wextra qdbmp.c nonlcdk_run.c -o nonlcdk_run.out
 */

#include "qdbmp.h"
#include "m_mem.h"
#include "img_utils.h"
#include "median_filter.h"
#include "zhang_suen.h"
#include "zs_8conn.h"
#include "minutiae_cn_map.h"
#include "heat.h"
#include <stdio.h>
#include <math.h>

unsigned char* binary;
unsigned char* bitmap;
unsigned char* median;

unsigned char* array_from_bmp(BMP* bmp, UINT width, UINT height)
{
	unsigned char* out = m_malloc(width * height * 3);

	UCHAR   r, g, b;
	UINT	x, y;
	for (x = 0; x < width; ++x) {
		for (y = 0; y < height; y++) {
			BMP_GetPixelRGB( bmp, x, y, &r, &g, &b );
			out[(y*width + x)*3 + 0] = r;
			out[(y*width + x)*3 + 1] = g;
			out[(y*width + x)*3 + 2] = b;
		}
	}

	return out;
}

BMP* bmp_from_binary_array(unsigned char* array, UINT width, UINT height, USHORT depth)
{
	BMP* bmp = BMP_Create(width, height, depth);

	UINT	x, y;

	for (x = 0; x < width; ++x) {
		for (y = 0; y < height; y++) {
			UCHAR val = array[y*width + x] == BBLACK ? BLACK : WHITE;
			BMP_SetPixelRGB( bmp, x, y, val, val, val );
		}
	}

	return bmp;
}

// For now, only make the heat map for bifurcations
BMP* bmp_from_heatmap(heat_t heat, USHORT depth)
{
    BMP* bmp = BMP_Create(heat.width, heat.height, depth);

    UCHAR   r, g, b;
    int    i, j;

    // First find the max value
    float max = HEAT_MIN;
    for (i = 0; i < heat.height; i++)
        for (j = 0; j < heat.width; j++)
            max = heat.map.bifurcation[i*heat.width + j] > max ? heat.map.bifurcation[i*heat.width + j] : max;

    for (i = 0; i < heat.height; i++) {
        for (j = 0; j < heat.width; j++) {
            g = 0;
            r = (UCHAR)(255.0 * heat.map.bifurcation[i*heat.width + j]/max);
            b = (UCHAR)(255.0 * (max - heat.map.bifurcation[i*heat.width + j])/max);
            BMP_SetPixelRGB( bmp, j, i, r, g, b );
        }
    }

    return bmp;
}

int main(void)
{
    const int N = 12;
	mem_init();

	printf("Reading bitmaps...\n");

    char* files[N] = { "1b.bmp", "2b.bmp", "3b.bmp", "4b.bmp", "5b.bmp", "6b.bmp",
                       "1a.bmp", "2a.bmp", "3a.bmp", "4a.bmp", "5a.bmp", "6a.bmp" };

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

    int k;
    int fid;

    /* Read an image file */
    for (k = 0; k < N; k++)
    {
        BMP *bmp;
        USHORT  depth;

        bmp = BMP_ReadFile(files[k]);
        BMP_CHECK_ERROR( stderr, -1 ); /* If an error has occurred, notify and exit */

        /* Get image's dimensions and depth */
        width[k] = (int)BMP_GetWidth( bmp );
        height[k] = (int)BMP_GetHeight( bmp );
        depth = BMP_GetDepth( bmp );

        // Put the bitmap in memory
        bitmap[k] = array_from_bmp(bmp, width[k], height[k]);
        BMP_Free(bmp);
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

        // Note: removed use of ZS 8-conn skeletonization due to the
        /*
        printf("8-conn skeletonizing...\n");
        skeleton = zs_8conn(height[k], width[k], zs_skeleton);
        m_free(zs_skeleton);
        */
        skeleton = zs_skeleton;

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
        fid = -1;
        while (!(0 <= fid && fid < N))
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

        /*
        printf("8-conn skeletonizing...\n");
        skeleton = zs_8conn(height[fid], width[fid], zs_skeleton);
        m_free(zs_skeleton);
        */
        skeleton = zs_skeleton;

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
        fid = -1;
        while (!(0 <= fid && fid < N/2))
            scanf("%d", &fid);

        merge_heatmaps(&heat[fid], &new_heat);

        printf("Updated fingerprint database\n");

        free_heatmap_body(&new_heat);
    }

    for (k = 0; k < N; k++)
        free_heatmap_body(&heat[k]);

    printf("Exiting...\n");
    return 0;
}