/*
 * This is a program to test matching of fingerprints
 * Build me: gcc -Wall -Wextra qdbmp.c nonlcdk_match_test.c -o nonlcdk_match_test.out
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

    heat_t heat[N];
    int k;

    char* files[N] = { "1a.bmp", "1b.bmp", "2a.bmp", "2b.bmp", "3a.bmp", "3b.bmp",
                       "4a.bmp", "4b.bmp", "5a.bmp", "5b.bmp", "6a.bmp", "6b.bmp" };

    /* Read an image file */
    for (k = 0; k < N; k++)
    {
        BMP *bmp;
        // BMP *out_bmp;
        // BMP *heat_out_bmp;
        int    width, height;
        USHORT  depth;
        unsigned char bw_threshold;

        bmp = BMP_ReadFile(files[k]);
        BMP_CHECK_ERROR( stderr, -1 ); /* If an error has occurred, notify and exit */

        /* Get image's dimensions and depth */
        width = (int)BMP_GetWidth( bmp );
        height = (int)BMP_GetHeight( bmp );
        depth = BMP_GetDepth( bmp );

        // Put the bitmap in memory
        unsigned char* bitmap = array_from_bmp(bmp, width, height);

        /* Free all memory allocated for the image */
        BMP_Free(bmp);

        printf("Rendering as greyscale...\n");
        unsigned char* binary = to_greyscale(bitmap, width, height); // load the image in black & white
        m_free(bitmap);

        // Calculate the threshold to use for black and white conversion
        printf("Calculating black and white threshold...\n");
        bw_threshold = find_threshold(binary, width, height);

        printf("Binarizing... threshold: %d\n", bw_threshold);
        binarize(binary, width, height, bw_threshold); // binarize the image (in-place)
        upsidedown(binary, width, height); // flip it upside down (in-place)

        printf("Running median filter...\n");
        unsigned char* median = med_filter(binary, width, height);
        m_free(binary);

        // Invert the pixels for skeletonization
        invert_binary(median, width, height);

        printf("Skeletonizing...\n");
        unsigned char* zs_skeleton = zhang_suen(height, width, median);
        m_free(median);

        unsigned char* skeleton = zs_8conn(height, width, zs_skeleton);
        m_free(zs_skeleton);

        // Invert back
        invert_binary(skeleton, width, height);

        printf("Creating heatmap...\n");

        // Make CN map
        int* cn_map = minutiae_cn_map(skeleton, width, height);

        // Make a heat map from the CN map
        heat[k] = create_heatmap(cn_map, height, width);

    	// Flip it back upside down before display
    	upsidedown(skeleton, width, height);

        // TODO: OUR HEAT MAP IS UPSIDE DOWN RIGHT NOW
        // heat_out_bmp = bmp_from_heatmap(heat[k], depth);

        printf("Saving result...\n");
        /* Save result */
        // out_bmp = bmp_from_binary_array(skeleton, width, height, depth);

        // Put one red pixel where bifurcations are
        /*
        int i, j;
        for (i = 0; i < height; i++) {
            for (j = 0; j < width; j++) {
                if (cn_map[i*width + j] == 3) {
                    BMP_SetPixelRGB(out_bmp, j, height-i-1, 255, 0, 0);
                }
            }
        }
        */

        m_free(cn_map);

        // BMP_WriteFile(out_bmp, "104_2_out.bmp");
        // BMP_CHECK_ERROR(stderr, -2);

        // BMP_WriteFile(heat_out_bmp, "heat_out.bmp");
        // BMP_CHECK_ERROR(stderr, -2);

        /* Free all memory allocated for the image */
        // BMP_Free(out_bmp);
        m_free(skeleton);

    	printf("Done!\n");
    }

    printf("All %d heatmaps created!\n", N);

    int a, b;
    for (a = 0; a < N; a++)
        for (b = 0; b < N; b++)
            printf("%d-%d score: %.4f\n", a, b, compute_match_score(heat[a], heat[b]));

    for (k = 0; k < N; k++)
        free_heatmap_body(&heat[k]);

    printf("Exiting...\n");
    return 0;
}
