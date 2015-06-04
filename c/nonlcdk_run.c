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
    const int N_FPRINTS = 10;
    const int N_EACH = 3;
	mem_init();

	printf("Reading bitmaps...\n");

    char* files[N_FPRINTS * N_EACH] = {
        "1b.bmp", "2b.bmp", "3b.bmp", "4b.bmp", "5b.bmp", "6b.bmp", "7b.bmp", "8b.bmp", "9b.bmp", "10b.bmp",
        "1a.bmp", "2a.bmp", "3a.bmp", "4a.bmp", "5a.bmp", "6a.bmp", "7a.bmp", "8a.bmp", "9a.bmp", "10a.bmp",
        "1c.bmp", "2c.bmp", "3c.bmp", "4c.bmp", "5c.bmp", "6c.bmp", "7c.bmp", "8c.bmp", "9c.bmp", "10c.bmp"
    };

    // Modification: use hardcoded thresholds
    unsigned char thresholds[N_FPRINTS * N_EACH] = {
        163, 175, 175, 173, 210, 225, 145, 130, 160, 115,
        175, 170, 155, 155, 210, 220, 120, 130, 170, 105,
        135, 180, 153, 170, 210, 225, 130, 130, 165, 120
    };
    int use_hardcoded_threshold = 0;

    unsigned char* bitmap[N_FPRINTS * N_EACH];
    int width[N_FPRINTS * N_EACH];
    int height[N_FPRINTS * N_EACH];
    unsigned char* binary;
    unsigned char* median;
    unsigned char* zs_skeleton;
    unsigned char* skeleton;
    int* cn_map;
    heat_t heat[N_FPRINTS];
    heat_t new_heat;

    int k;
    int fid;

    /* Read an image file */
    for (k = 0; k < N_FPRINTS * N_EACH; k++)
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

    for (k = 0; k < N_FPRINTS; k++)
    {
        printf("\nProcessing bitmap %d...\n", k);
        printf("Converting bitmap to greyscale...\n");
        binary = to_greyscale(bitmap[k], width[k], height[k]); // load the image in black & white

        unsigned char bw_threshold;
        if (use_hardcoded_threshold) {
            // Modification: use hardcoded threshold
            bw_threshold = thresholds[k];
        }
        else {
            // Calculate the threshold to use for black and white conversion
            printf("Calculating black and white threshold...\n");
            bw_threshold = find_threshold(binary, width[k], height[k]);
        }

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
        m_free(skeleton);

        // Make a heat map from the CN map
        heat[k] = create_heatmap(cn_map, height[k], width[k]);
        m_free(cn_map);

        printf("Complete!\n");
    }

    printf("\nAll %d heatmaps created!\n", N_FPRINTS);

    while(1)
    {
        printf("\nEnter a fingerprint image number to load: ");
        fid = -1;
        while (!(0 <= fid && fid < N_FPRINTS * N_EACH))
            scanf("%d", &fid);

        printf("Processing bitmap %d...\n", fid);
        printf("Converting bitmap to greyscale...\n");
        binary = to_greyscale(bitmap[fid], width[fid], height[fid]); // load the image in black & white

        unsigned char bw_threshold;
        if (use_hardcoded_threshold) {
            // Modification: use hardcoded threshold
            bw_threshold = thresholds[fid];
        }
        else {
            // Calculate the threshold to use for black and white conversion
            printf("Calculating black and white threshold...\n");
            bw_threshold = find_threshold(binary, width[fid], height[fid]);
        }

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
        m_free(skeleton);

        // Make a heat map from the CN map
        new_heat = create_heatmap(cn_map, height[fid], width[fid]);
        m_free(cn_map);

        printf("Complete!\n");

        int best_match = -1;
        float best_score = 999.0;

        int i;
        for (i = 0; i < N_FPRINTS; i++) {
            float this_score = compute_match_score(new_heat, heat[i]);
            printf("%d-%d score: %.4f\n", fid, i, this_score);
            if (this_score < best_score) {
                best_score = this_score;
                best_match = i;
            }
        }

        printf("\nFound match: %d. Enter actual match: ", best_match);
        fid = -1;
        while (!(0 <= fid && fid < N_FPRINTS))
            scanf("%d", &fid);

        merge_heatmaps(&heat[fid], &new_heat);

        printf("Updated fingerprint database\n");

        free_heatmap_body(&new_heat);
    }

    for (k = 0; k < N_FPRINTS; k++)
        free_heatmap_body(&heat[k]);

    printf("Exiting...\n");
    return 0;
}
