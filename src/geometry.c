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
	int n = poly->npts;
	struct dcel *ret = dcel_create(n, 4*n - 6, n - 1);
	struct vertex *last = ret->vbuf;
	last->pt = poly->buf[0];
	for (int i = 1; i < n; i++) {
		last = dcel_connect_vertex(ret, poly->buf[i], last, ret->fbuf);
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


static struct polygon polygon_create (int size) {
	/* Callers must handle ret.buf being NULL
	 * */
	struct polygon ret;
	SDL_FPoint *buf = SDL_malloc(sizeof(SDL_FPoint)*size);
	ret.buf = buf;
	ret.ptr = buf;
	ret.size = size;
	ret.npts = 0;
	ret.closed = 0;
	return ret;
}
static void polygon_dest (struct polygon *poly) {
	SDL_free(poly->buf);
	poly->ptr = NULL;
	poly->size = 0;
	poly->npts = 0;
}
static int polygon_add_point (struct polygon *poly, SDL_FPoint pt) {
	// Returns an error code if reallocation fails
	if ((size_t) (poly->ptr - poly->buf) >= poly->size) {
		// Is it smart to double the size each time? Who knows
		SDL_FPoint *tmp = poly->buf;
		poly->buf =
			SDL_realloc(poly->buf, 2*sizeof(SDL_FPoint)*poly->size);
		poly->size = 2*poly->size;
		if (!poly->buf) {
			SDL_free(tmp);
			log_error(
"Failed to reallocate a polygon buffer at <%p>", tmp);
			poly->ptr = NULL;
			poly->size = 0;
			poly->npts = 0;
			return -1;
		}
	}
	*(poly->ptr) = pt;
	poly->ptr++;
	poly->npts++;
	return 0;
}
static void polygon_erase (struct polygon *poly) {
	poly->ptr = poly->buf;
	poly->npts = 0;
}

static SDL_FPoint bezier_evaluate (struct bezier bez, float t) {
	SDL_FPoint B0 = bez.pts[0];
	SDL_FPoint B1 = bez.pts[1];
	SDL_FPoint B2 = bez.pts[2];
	SDL_FPoint B3 = bez.pts[3];
	float ret_x = B0.x * (1 - t)*(1 - t)*(1 - t)
		+ 3*B1.x * t * (1 - t)*(1 - t)
		+ 3*B2.x * t*t * (1 - t) + B3.x * t*t*t;
	float ret_y = B0.y * (1 - t)*(1 - t)*(1 - t)
		+ 3*B1.y * t * (1 - t)*(1 - t)
		+ 3*B2.y * t*t * (1 - t) + B3.y * t*t*t;
	return (SDL_FPoint) {ret_x, ret_y};
}
static SDL_FPoint bezier_derivative (struct bezier bez, float t) {
	SDL_FPoint B0 = bez.pts[0];
	SDL_FPoint B1 = bez.pts[1];
	SDL_FPoint B2 = bez.pts[2];
	SDL_FPoint B3 = bez.pts[3];
	float x_grad = 3*t*t*(-B0.x + 3*B1.x - 3*B2.x + B3.x)
		+ 6*t*(B0.x + 2*B1.x + B2.x) + 3*(-B0.x + B1.x);
	float y_grad = 3*t*t*(-B0.y + 3*B1.y - 3*B2.y + B3.y)
		+ 6*t*(B0.y + 2*B1.y + B2.y) + 3*(-B0.y + B1.y);
	return (SDL_FPoint) {x_grad, y_grad};
}
static float fpoint_magnitude (SDL_FPoint pt) {
	return SDL_sqrtf(pt.x*pt.x + pt.y*pt.y);
}
static float bezier_derivative_magnitude (struct bezier bez, float t) {
	return fpoint_magnitude(bezier_derivative(bez, t));
}
static float arc_length_approx (struct bezier bez, float t) {
	float q0 = bezier_derivative_magnitude(bez, 1.774597/2 * t);
	float q1 = bezier_derivative_magnitude(bez, 1/2 * t);
	float q2 = bezier_derivative_magnitude(bez, 0.225403/2 * t);
	return t/2 * (5/9 * q0 + 8/9 * q1 + 5/9 * q2);
}
static float arc_length_newton (struct bezier bez, float arc,
		float rtol, int max_iter) {
	// Use Newton's method on the Gaussian quadrature approximation
	// of cubic Bezier arc length
	float ret = 0.5;
	for (int i = 0; i < max_iter; i++) {
		float arc_diff = arc_length_approx(bez, ret) - arc;
		float arc_rel = arc_diff / arc;
		if (arc_rel < rtol && -arc_rel > -rtol)
			break;
		float arc_grad = bezier_derivative_magnitude(bez, ret);
		ret = ret - arc_diff / arc_grad;
	}
	// Clamp to valid Bezier parameter bounds
	if (ret < 0.0f)
		ret = 0.0f;
	if (ret > 1.0f)
		ret = 1.0f;
	return ret;
}
static int bezier_polygon (struct bezier bez, struct polygon *poly,
		int max_segments, float atol) {
	// Sample a Bezier curve into discrete polygonal segments
	// Error is the absolute deviation of the bezier from a linear path
	//
	// Return values:
	// 0: success
	// -1: memory-related failure
	// 0x2: over tolerance
	float total_arc = arc_length_approx(bez, 1.0f);
	for (int seg = 1; seg < max_segments; seg++) {
		// Is a linear search efficient? No.
		// I suspect that this won't be a significant performance issue.
		int over_tol = 0; // Set to 1 if tolerance is exceeded
		float freq = 1.0f / (float) seg;

		polygon_erase(poly);
		float t = 0.0f;
		for (int i = 0; i < seg; i++) {
			float t_prev = t;
			float arc_target = (float) i * freq * total_arc;
			float t = arc_length_newton(bez, arc_target,
					1.0e-6, 512);
			SDL_FPoint point = bezier_evaluate(bez, t);
			if (polygon_add_point(poly, point)) {
				// Realloc failure or other buffer overflow
				return -1;
			}

			SDL_FPoint real = bezier_evaluate(bez, (t + t_prev)/2);
			SDL_FPoint prev = poly->ptr[-1];
			prev.x = (point.x + prev.x) / 2 - real.x;
			prev.y = (point.y + prev.y) / 2 - real.y;
			float err = fpoint_magnitude(prev);
			if (err > atol)
				over_tol = 1;
		}
		if (!over_tol)
			return 0;
	}
	log_warn("Bezier flattening <%p> over tolerance", poly->buf);
	return 0x2;
}


/*
enum vertex_type {
	VERTEX_START, VERTEX_END, VERTEX_SPLIT, VERTEX_MERGE, VERTEX_REGULAR
};
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

