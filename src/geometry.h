#ifndef CTRW_GEOMETRY_H
#define CTRW_GEOMETRY_H

/* Common tools for Bézier curve and polygon operations
 * Only cubic Bézier curves will be considered, so "cubic Bézier"
 * will be shortened simply to "Bézier"
 * */
#include <SDL3/SDL.h>

struct circle {
	SDL_FPoint center;
	float radius;
};

struct bezier {
	SDL_FPoint pts[4];
};

struct polygon {
	SDL_FPoint *pts;
	int npts;
	int closed;
};

struct geometry {
	SDL_Vertex *vbuf;
	int *ibuf;
	int nvert, nidx;
};

#endif

