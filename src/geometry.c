#include <SDL3/SDL.h>
#include <SDL3/SDL_stdinc.h>

#include "geometry.h"
#include "log.h"

/* Doubly-Connected Edge List
 * Incomplete implementation with only the features needed to monotonize and
 * triangulate a closed, simple polygon
 * */
struct vertex;
struct edge;
struct face;

struct vertex {
	SDL_FPoint pt;
	struct edge *edge;
};
struct edge {
	struct vertex *origin;
	struct edge *prev, *next, *twin;
	struct face *face;
};
struct face {
	struct edge *edge;
};

struct dcel {
	struct vertex *vbuf, *vptr;
	struct edge *ebuf, *eptr;
	struct face *fbuf, *fptr;
	size_t vsize, esize, fsize;
};

long dcel_euler_char (struct dcel *list) {
	// V - E + F should always be 2
	long V = list->vptr - list->vbuf;
	long E = list->eptr - list->ebuf;
	long F = list->fptr - list->fbuf;
	if (E % 2 != 0) {
		return (size_t) -1;
	}
	E = E/2;
	return V - E + F;
}

struct vertex * dcel_add_vertex (struct dcel *list) {
	if (!(list->vptr - list->vbuf < (long) list->vsize)) {
		return NULL;
	}
	return list->vptr++;
}

struct edge * dcel_add_edge (struct dcel *list) {
	if (!(list->eptr - list->ebuf < (long) list->esize)) {
		return NULL;
	}
	return list->eptr++;
}

struct face * dcel_add_face (struct dcel *list) {
	if (!(list->fptr - list->fbuf < (long) list->fsize)) {
		return NULL;
	}
	return list->fptr++;
}

struct dcel * dcel_create (size_t vsize, size_t esize, size_t fsize) {
	struct dcel *ret;
	if (vsize == 0 || esize == 0 || fsize == 0) {
		return NULL;
	}
	if (!(ret = SDL_calloc(1, sizeof(struct dcel)))) {
		log_error("Could not allocate DCEL object");
		return NULL;
	}
	if (!(ret->vbuf = SDL_calloc(vsize, sizeof(struct vertex)))) {
		log_error("Could not allocate DCEL vertex buffer of size %zu",
				vsize);
		return NULL;
	}
	ret->vptr = ret->vbuf;
	ret->vsize = vsize;
	if (!(ret->ebuf = SDL_calloc(esize, sizeof(struct edge)))) {
		log_error("Could not allocate DCEL edge buffer of size %zu",
				esize);
		return NULL;
	}
	ret->eptr = ret->ebuf;
	ret->esize = esize;
	if (!(ret->fbuf = SDL_calloc(fsize, sizeof(struct face)))) {
		log_error("Could not allocate DCEL face buffer of size %zu",
				fsize);
		return NULL;
	}
	ret->fptr = ret->fbuf;
	ret->fsize = fsize;
	// Add a dummy vertex and face to make the Euler characteristic == 2
	dcel_add_vertex(ret);
	dcel_add_face(ret);
	return ret;
}

void dcel_dest (struct dcel *arr) {
	SDL_free(arr->vbuf);
	SDL_free(arr->ebuf);
	SDL_free(arr->fbuf);
	SDL_free(arr);
}

// Return a pointer to the half-edge incident on face whose origin is vert
struct edge * dcel_find_edge_on_face (struct vertex *vert, struct face *face) {
	struct edge *start, *ret;
	start = ret = vert->edge;
	if (!start) // Vertex has no edges yet
		return start;
	while (ret->face != face) {
		if (ret->twin->next == start) // vertex has no such edge
			return NULL;
		ret = ret->twin->next;
	}
	return ret;
}

struct vertex * dcel_connect_vertex (struct dcel *arr, SDL_FPoint pt,
		struct vertex *con, struct face *face) {
	struct vertex *new;
	struct edge *edge, *twin, *next, *prev;
	if (!(new = dcel_add_vertex(arr)))
		return NULL;
	if (!(edge = dcel_add_edge(arr)))
		return NULL;
	if (!(twin = dcel_add_edge(arr)))
		return NULL;
	next = dcel_find_edge_on_face(con, face);
	if (next) {
		prev = next->prev;
		prev->next = edge;
		next->prev = twin;
	} else // Existing vertex has no edges
		prev = twin;
	edge->prev = prev;
	edge->next = twin;
	twin->prev = edge;
	twin->next = next;

	edge->twin = twin;
	twin->twin = edge;
	edge->origin = con;
	twin->origin = new;
	edge->face = twin->face = face;

	new->edge = twin;
	con->edge = edge;
	return new;
}

struct edge * dcel_add_diagonal (struct dcel *arr, struct vertex *a,
		struct vertex *b, struct face *out) {
	// Add a diagonal; assume the direction a --> b is counterclockwise
	// with respect to the new face
	// I'm pretty sure this implementation will be O(n) because we have
	// to traverse a bunch of edges changing the face. This will make the
	// monotonization algorithm O(n^2).
	// Frankly, that's OK. This code will not run per-frame anyway.
	// Keeping the faces adds the benefit that we can fetch the
	// triangles super easily when the triangulation is done.
	struct edge *edge, *twin, *aprev, *anext, *bprev, *bnext;
	struct face *in;
	if (!(edge = dcel_add_edge(arr)))
		return NULL;
	if (!(twin = dcel_add_edge(arr)))
		return NULL;
	if (!(in = dcel_add_face(arr)))
		return NULL;
	anext = dcel_find_edge_on_face(a, out);
	bnext = dcel_find_edge_on_face(b, out);
	if (anext) {
		aprev = anext->prev;
		aprev->next = edge;
		anext->prev = twin;
	} else
		aprev = NULL;
	if (bnext) {
		bprev = bnext->prev;
		bprev->next = twin;
		bnext->prev = edge;
	} else
		bprev = NULL;
	edge->prev = aprev;
	edge->next = bnext;
	twin->prev = bprev;
	twin->next = anext;

	edge->twin = twin;
	twin->twin = edge;
	edge->origin = a;
	twin->origin = b;
	twin->face = out;
	edge->face = in;
	out->edge = twin;
	in->edge = edge;
	for (struct edge *ptr = edge->next; ptr != edge; ptr = ptr->next) {
		if (!ptr) // There should be a loop
			return NULL;
		ptr->face = in;
	}
	return edge;
}

struct dcel * dcel_init_polygon (struct polygon *poly) {
	// Expect a dcel with 2 faces, n vertices, 2n half-edges,
	// and an Euler characteristic of 2
	size_t n = poly->npts;
	struct dcel *ret = dcel_create(n, 4*n - 6, n - 1);
	struct vertex *last = ret->vbuf;
	last->pt = poly->pts[0];
	for (size_t i = 1; i < n; i++) {
		last = dcel_connect_vertex(ret, poly->pts[i], last, ret->fbuf);
		if(!last) { // Something went wrong
			dcel_dest(ret);
			return NULL;
		}
	}
	struct edge *tmp =
		dcel_add_diagonal(ret, last, ret->vbuf, ret->fbuf);
	if (!tmp) {
		dcel_dest(ret);
		return NULL;
	}
	return ret;
}

/*
enum vertex_type {
	VERTEX_START, VERTEX_END, VERTEX_SPLIT, VERTEX_MERGE, VERTEX_REGULAR
};
static void polygon_add_point (struct polygon *polygon, SDL_FPoint pt);


void flatten_bezier (struct polygon *target, struct bezier bez, int depth) {
	// Algorithm courtesy of NanoSVG:
	// nsvg__addPathPoint in nanosvgrast.h
	SDL_FPoint p01, p12, p23, p012, p123, p0123;
	SDL_FPoint d;

	if (depth > 10) return;
	p01.x = (bez.pts[0].x + bez.pts[1].x) * 0.5f;
	p01.y = (bez.pts[0].y + bez.pts[1].y) * 0.5f;
	p12.x = (bez.pts[1].x + bez.pts[2].x) * 0.5f;
	p12.y = (bez.pts[1].y + bez.pts[2].y) * 0.5f;
	p23.x = (bez.pts[2].x + bez.pts[3].x) * 0.5f;
	p23.y = (bez.pts[2].y + bez.pts[3].y) * 0.5f;

	p012.x = (p01.x + p12.x) * 0.5f;
	p012.y = (p01.y + p12.y) * 0.5f;

	d.x = bez.pts[3].x - bez.pts[0].x;
	d.y = bez.pts[3].y - bez.pts[0].y;

	float d1 = abs((bez.pts[3].x - bez.pts[1].x) * d.y
			- (bez.pts[3].y - bez.pts[1].y) * d.x);
	float d2 = abs((bez.pts[3].x - bez.pts[2].x) * d.y
			- (bez.pts[3].y - bez.pts[2].y) * d.x);

	if ((d1 + d2)*(d1 + d2) < tol * (d.x * d.x + d.y * d.y)) {
		polygon_add_point(target, bez.pts[3]);
		return;
	}

	p123.x = (p12.x + p23.x) * 0.5f;
	p123.y = (p12.y + p23.y) * 0.5f;
	p0123.x = (p012.x + p123.x) * 0.5f;
	p0123.y = (p012.y + p123.y) * 0.5f;
	struct bezier first_half = {.pts = { bez.pts[0], p01, p012, p0123 }};
	struct bezier second_half = {.pts = { p0123, p123, p23, bez.pts[3] }};
	flatten_bezier(target, first_half, depth+1);
	flatten_bezier(target, second_half, depth+1);
}


static int make_monotone (struct edge_list *edges, struct polygon *src) {
	struct vertex_tree *vertex_heap; // keep sorted by height
	struct sweepline_tree *sweepline;
	int vertex = -1;

	while (-1 != (vertex = tree_pop(vertex_heap)->i)) {
		switch (vertex_type(src, vertex->i)) {
			case (VERTEX_START):
			handle_start(vertex_heap, edges, sweepline, vertex);
			break;
			case (VERTEX_END):
			handle_end(vertex_heap, edges, sweepline, vertex);
			break;
			case (VERTEX_SPLIT):
			handle_split(vertex_heap, edges, sweepline, vertex);
			break;
			case (VERTEX_MERGE):
			handle_merge(vertex_heap, edges, sweepline, vertex);
			break;
			case (VERTEX_REGULAR):
			handle_REGULAR(vertex_heap, edges, sweepline, vertex);
			default:
			return -1;
		}
	}

	return 0;
}
*/

