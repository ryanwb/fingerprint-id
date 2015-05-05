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
	free(heat->map.ridgeending);
	free(heat->map.bifurcation);
	free(heat->map.crossing);
}

/* blur_point
 * Spreads a point i,j out around its neighboring points in a float matrix
 */
void blur_point(float* matrix, int height, int width, int i, int j)
{
	/* Right now, this is a pretty trivial/slow algorithm!
	 * We look at each point in the image add to that
	 * pixel by an amount inversely proportional to its distance.
	 */
	int y, x;
	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			if (!(i == y && j == x)) {
				float r = sqrt(pow(y-i, 2) + pow(x-j, 2));
				matrix[y*width + x] += matrix[i*width + j] / r;
			}
		}
	}
}

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
			if (cn_map[i*width + j] == RIDGE_ENDING) {
				// Add the peak
				heat.map.ridgeending[i*width + j] = HEAT_MAX;
				// Now spread it out to form a "mountain"
				blur_point(heat.map.ridgeending, height, width, i, j);
			}
			else if (cn_map[i*width + j] == BIFURCATION) {
				heat.map.bifurcation[i*width + j] = HEAT_MAX;
				blur_point(heat.map.bifurcation, height, width, i, j);
			}
			else if (cn_map[i*width + j] == CROSSING) {
				heat.map.crossing[i*width + j] = HEAT_MAX;
				blur_point(heat.map.crossing, height, width, i, j);
			}
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
