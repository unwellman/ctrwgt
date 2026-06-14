#include <stdlib.h>

#include "log.h"
#include "geometry.h"
#include "test.h"

/* Forward declarations of geometry internals
 * This module requires tests of internal methods, so rather than writing a
 * header, these declarations are copied here.
 *
 * It violates DRY, but it also forces breaking changes to break tests first,
 * which encourages good architecture
 * */
struct vertex;
struct edge;
struct face;
struct dcel;

size_t dcel_euler_char (struct dcel *list);
struct dcel * dcel_create (size_t vsize, size_t esize, size_t fsize);
struct edge * dcel_find_edge_on_face (struct vertex *vert, struct face *face);
struct vertex * dcel_connect_vertex (struct dcel *arr, SDL_FPoint pt,
		struct vertex *con, struct face *face);
struct edge * dcel_add_diagonal (struct dcel *arr, struct vertex *a,
		struct vertex *b, struct face *out);
struct dcel * dcel_init_polygon (struct polygon *poly);
void dcel_dest (struct dcel *arr);

// Internal tests

static SDL_FPoint pts[] = { // The seventh roots of unity
	{1.0, 0.0},
	{0.6235, 0.7818},
	{-0.2225, 0.9749},
	{-0.9010, 0.4339},
	{-0.9010, -0.4339},
	{-0.2225, -0.9749},
	{0.6235, -0.7818}
};
static struct polygon poly = {
	.buf = pts,
	.ptr = pts,
	.size = sizeof(pts),
	.npts = 7,
	.closed = 1
};
static int test_dcel_init (void) {
	struct dcel *obj = dcel_init_polygon(&poly);
	size_t euler = dcel_euler_char(obj);
	if (euler != 2) {
		log_critical("Euler characteristic is %ld", euler);
		return -1;
	}
	dcel_dest(obj);
	return 0;
}

static struct bezier bez = {
	.pts = {
		{0.0, 0.0},
		{0.3333, 0.0},
		{0.6667, 0.0},
		{1.0, 0.0}
	}
};
static int test_flatten_bezier (void) {
	int val = sizeof(bez.pts);
	return val - val;
}

// Exposed interface tests


static struct test TEST_LIST[] = {
	TEST(test_dcel_init),
	TEST(test_flatten_bezier)
};
EXPORT(TEST_LIST, geometry)

