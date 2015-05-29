#ifndef HEAT_H
#define HEAT_H

#include "minutiae_cn_map.h"
#include <math.h>

#define HEAT_MIN 0.0
#define HEAT_MAX 100.0

/* map_t is the type for the 3 * m * n float struct that is the heatmap itself */
typedef struct {
	float* ridgeending;
	float* bifurcation;
	float* crossing;
} map_t;

/* heat_t is the container for the heamap that also wraps the height and width
 * count is the number of cumulative (merged) heat maps that this heat map represents
 */
typedef struct {
	int height;
	int width;
	map_t map;
	int count;
} heat_t;

/* compute_match_score
 * Returns a "score" for how good of a match the two heatmaps are
 * Lower scores mean the match is better
 */
float compute_match_score(heat_t heat_a, heat_t heat_b)
{
	float score = 0.0;
	int h = heat_a.height < heat_b.height ? heat_a.height : heat_b.height;
	int w = heat_a.width < heat_b.width ? heat_a.width : heat_b.width;
	// We can scale our pixel match value by N if we want
	// float N = (float)(h * w);
	int i, j;

	// First find total heat in each map we're looking at, so we can normalize them against eachother
	float a_bif_sum = HEAT_MIN;
	float b_bif_sum = HEAT_MIN;
	float a_re_sum = HEAT_MIN;
	float b_re_sum = HEAT_MIN;
	float a_cros_sum = HEAT_MIN;
	float b_cros_sum = HEAT_MIN;
	for (i = 0; i < h; i++) {
		for (j = 0; j < w; j++) {
			a_bif_sum += heat_a.map.bifurcation[i*w + j];
			b_bif_sum += heat_b.map.bifurcation[i*w + j];
			a_re_sum += heat_a.map.ridgeending[i*w + j];
			b_re_sum += heat_b.map.ridgeending[i*w + j];
			a_cros_sum += heat_a.map.crossing[i*w + j];
			b_cros_sum += heat_b.map.crossing[i*w + j];
		}
	}

	// Some fixes to avoid divide by zero:
	if (a_bif_sum == 0) a_bif_sum = 1; if (b_bif_sum == 0) b_bif_sum = 1;
	if (a_re_sum == 0) a_re_sum = 1; if (b_re_sum == 0) b_re_sum = 1;
	if (a_cros_sum == 0) a_cros_sum = 1; if (b_cros_sum == 0) b_cros_sum = 1;

	// Now compute the scores using the normalization
	for (i = 0; i < h; i++) {
		for (j = 0; j < w; j++) {
			score += fabsf(heat_a.map.bifurcation[i*w + j]/a_bif_sum - heat_b.map.bifurcation[i*w + j]/b_bif_sum);
			score += fabsf(heat_a.map.ridgeending[i*w + j]/a_re_sum - heat_b.map.ridgeending[i*w + j]/b_re_sum);
			score += fabsf(heat_a.map.crossing[i*w + j]/a_cros_sum - heat_b.map.crossing[i*w + j]/b_cros_sum);
		}
	}
	// TODO: BETTER SCORING
	return score;
}

/* initialize_heatmap_body
 * Initialize a new heatmap by allocating memory for the contents
 */
void initialize_heatmap_body(heat_t* heat, int height, int width)
{
	int i, j;
	heat->height = height;
	heat->width = width;
	heat->count = 1;
	heat->map.ridgeending = m_malloc(sizeof(float) * height * width);
	heat->map.bifurcation = m_malloc(sizeof(float) * height * width);
	heat->map.crossing = m_malloc(sizeof(float) * height * width);
	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			heat->map.ridgeending[i*width + j] = HEAT_MIN;
			heat->map.bifurcation[i*width + j] = HEAT_MIN;
			heat->map.crossing[i*width + j] = HEAT_MIN;
		}
	}
}

/* free_heatmap_body
 * Destroy a heatmap by freeing the memory associated with its contents
 */
void free_heatmap_body(heat_t* heat)
{
	m_free(heat->map.ridgeending);
	m_free(heat->map.bifurcation);
	m_free(heat->map.crossing);
}

/* blur_point
 * Spreads a point i,j out around its neighboring points in a float matrix
 */

void blur_point(float* matrix, int height, int width, int i, int j)
{
	// We'll only modify points in a box_size*2+1 by box_size*2+1 square around the point to blur
	int box_size = 4;
	int y_start = i - box_size >= 0 ? i - box_size : 0;
	int y_end = i + box_size <= height ? i + box_size : height;
	int x_start = j - box_size >= 0 ? j - box_size : 0;
	int x_end = j + box_size <= width ? j + box_size : width;
	int y, x;
	for (y = y_start; y < y_end; y++) {
		for (x = x_start; x < x_end; x++) {
			float r = sqrt(pow((float)(y-i), 2) + pow((float)(x-j), 2));
			matrix[y*width + x] += matrix[i*width + j] / (1.0 + r);
		}
	}
}

/*
void blur_point(float* matrix, int height, int width, int i, int j)
{
	int y, x;
	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			// New method:
			float r = sqrt(pow((float)(y-i), 2) + pow((float)(x-j), 2));
			matrix[y*width + x] += matrix[i*width + j] / (1.0 + r);
		}
	}
}
*/

/* create_heatmap
 * Creates a new heatmap based on an input crossing number map
 */
heat_t create_heatmap(int* cn_map, int height, int width)
{
	heat_t heat;
	int i, j;
	initialize_heatmap_body(&heat, height, width);
	// Add the peaks of the new heatmap
	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			if (cn_map[i*width + j] == RIDGE_ENDING)
				// Add the peak
				heat.map.ridgeending[i*width + j] = HEAT_MAX;
			else if (cn_map[i*width + j] == BIFURCATION)
				heat.map.bifurcation[i*width + j] = HEAT_MAX;
			else if (cn_map[i*width + j] == CROSSING)
				heat.map.crossing[i*width + j] = HEAT_MAX;
		}
	}
	// Now blur the hot points
	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			if (cn_map[i*width + j] == RIDGE_ENDING)
				// Spread it out to form a "mountain"
				blur_point(heat.map.ridgeending, height, width, i, j);
			else if (cn_map[i*width + j] == BIFURCATION)
				blur_point(heat.map.bifurcation, height, width, i, j);
			else if (cn_map[i*width + j] == CROSSING)
				blur_point(heat.map.crossing, height, width, i, j);
		}
	}
	return heat;
}

/* merge_heatmaps
 * Merges two heat maps using a weighted average
 * o_heat is modified (n_heat is "added" to o_heat)
 * n_heat is NOT freed/deleted after the maps are merged
 */
void merge_heatmaps(heat_t * o_heat, heat_t * n_heat){
	int new_count=o_heat->count+ n_heat->count;
	int m= o_heat->height;
	int n= o_heat->width;
	int i,j;
	for(i=0; i<m; i++){
		for(j=0; j<n; j++){
			o_heat->map.ridgeending[i*n+j]=((float)o_heat->count/new_count)*o_heat->map.ridgeending[i*n+j]
			+((float)n_heat->count/new_count)*n_heat->map.ridgeending[i*n+j];
			o_heat->map.bifurcation[i*n+j]=((float)o_heat->count/new_count)*o_heat->map.bifurcation[i*n+j]
			+((float)n_heat->count/new_count)*n_heat->map.bifurcation[i*n+j];
			o_heat->map.crossing[i*n+j]=((float)o_heat->count/new_count)*o_heat->map.crossing[i*n+j]
			+((float)n_heat->count/new_count)*n_heat->map.crossing[i*n+j];
		}
	}
	o_heat->count=new_count;
}

#endif
