#ifndef CTRW_GEOMETRY_H
#define CTRW_GEOMETRY_H

/* Common tools for Bezier curve and polygon operations
 * Only cubic Bezier curves will be considered, so "cubic Bezier"
 * will be shortened simply to "Bezier"
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
	SDL_FPoint *buf, *ptr;
	size_t size;
	int npts;
	int closed;
};

struct geometry {
	SDL_Vertex *vbuf;
	int *ibuf;
	int nvert, nidx;
};

#endif

