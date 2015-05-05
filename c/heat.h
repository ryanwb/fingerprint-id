#ifndef HEAT_H
#define HEAT_H

void initialize_heatmap_body(heat_t* heat, int height, int width)
{
	heat->height = height;
	heat->width = width;
	heat->map.ridgeending = m_malloc(sizeof(float) * height * width);
	heat->map.bifurcation = m_malloc(sizeof(float) * height * width);
	heat->map.crossing = m_malloc(sizeof(float) * height * width);
}

void free_heatmap_body(heat_t* heat)
{
	free(heat->map.ridgeending);
	free(heat->map.bifurcation);
	free(heat->map.crossing);
}

typedef struct {
	float* ridgeending;
	float* bifurcation;
	float* crossing;
} map_t;

typedef struct {
	int height;
	int width;
	map_t map;
	int count;
} heat_t;

#endif
