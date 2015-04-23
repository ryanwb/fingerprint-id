#ifndef HOUGH_H
#define HOUGH_H

#include "img_params.h"
#include "m_mem.h"
#include <math.h>

#define PI 3.1415926
#define DEG_TO_RAD(d)	(d * PI / 180.0)

// Hough transform parameters
#define THETA_LOW 0.0
#define THETA_HIGH 180.0
#define THETA_STEP 15.0

struct detected_line {
	double r;
	double theta;
	int v_x;
	int v_y;
	int votes;
};

struct hough_info {
	int width;
	int height;
	int r_max;
	double theta_low;
	double theta_high;
	double theta_step;
};

double x_to_theta(int x, double theta_min, double theta_step) {
	return theta_min + (theta_step*(double)x);
}

int theta_to_x(double theta, double theta_min, double theta_step) {
	return (int)((theta - theta_min)/theta_step);
}

int y_to_r(int y, int r_max) {
	return r_max - y;
}

int r_to_y(int r, int r_max) {
	return r_max - r;
}

/* hough_transform
 * Returns a votes matrix
 */
int* hough_transform(unsigned char* binary, int b_width, int b_height, struct hough_info* out)
{
	int r_max = b_width + b_height;
	// our (x, y) plot for votes is (theta, r)
	// x ranges from THETA_LOW to THETA_HIGH - THETA_STEP (here, 0 to 165, in increments of THETA_STEP for 12 total values)
	// y ranges from -r_max to r_max

	int v_width = (THETA_HIGH - THETA_LOW)/THETA_STEP;
	int v_height = 2*r_max + 1;
	int* votes = m_malloc(v_width * v_height);

	int b_x, b_y;	// x, y in binary image
	int v_x, v_y;	// x, y in votes matrix
	// Set all votes to zero
	for (v_y = 0; v_y < v_height; v_y++)
		for (v_x = 0; v_x < v_width; v_x++)
			votes[v_y*v_width + v_x] = 0;

	for (b_y = 0; b_y < b_height; b_y++) {
		for (b_x = 0; b_x < b_width; b_x++) {
			// For each pixel (b_x, b_y)
			if (binary[b_y*b_width + b_x] == BLACK) {
				double theta;
				for (theta = THETA_LOW; theta < THETA_HIGH; theta += THETA_STEP) {
					double r = b_x*cos(DEG_TO_RAD(theta)) + b_y*sin(DEG_TO_RAD(theta));
					int vote_x = theta_to_x(theta, THETA_LOW, THETA_STEP);
					int vote_y = r_to_y((int)r, r_max);
					votes[vote_y*v_width + vote_x]++;
				}
			}
		}
	}
	out->width = v_width;
	out->height = v_height;
	out->r_max = r_max;
	out->theta_low = THETA_LOW;
	out->theta_high = THETA_HIGH;
	out->theta_step = THETA_STEP;
	return votes;
}

#endif
