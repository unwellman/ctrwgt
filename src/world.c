#include <stdlib.h>
#include <math.h>

#define PI 3.141592653589793115997963468544185161590576171875

#include "world.h"

#define CHUNK_WIDTH 64
#define CHUNK_HEIGHT 64

struct tile {
	int data;
};

struct chunk {
	size_t pos_i, pos_j;
	struct tile buf[CHUNK_WIDTH*CHUNK_HEIGHT];
};

static inline size_t ring_subdivisions (size_t i) {
	return pow(2, ceil(0.5 + log2(PI * i)));
}

static double principal_argument (double x, double y) {
	// Arctangent function that matches intuition for Arg(z)
	if (x == 0) {
		if (y >= 0)
			return PI / 2;
		else
			return - PI / 2;
	} else if (x < 0) {
		if (y >= 0) // Quadrant II
			return atan(y / x) + PI;
		else // Quadrant III
			return atan(y / x) - PI;
	} else // x > 0; quadrants I and IV
		return atan(y / x);
}

void coord_world_tile (double *dst_x, double *dst_y, size_t i, size_t j) {
	double angle = j*2*PI / (double) ring_subdivisions(i);
	*dst_x = (double) i * cos(angle);
	*dst_y = (double) i * sin(angle);
}

void nearest_tile (size_t *dst_i, size_t *dst_j, double x, double y) {
	size_t i = ceil(sqrt(x*x + y*y));
	double subdivision = 2*PI / (double) ring_subdivisions(i);
	double theta = principal_argument(x, y);
	*dst_j = floor(theta / subdivision);
	*dst_i = i;
}


