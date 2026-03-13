#include "test.h"
#include "world.h"

#define NUM_WORLD_TESTS 2
static struct test TEST_LIST[NUM_WORLD_TESTS];
// Export
struct test_batch world_tests = {
	.count = NUM_WORLD_TESTS,
	.ptr = TEST_LIST
};


static int test_coord_world_tile (void) {
	double x, y;
	coord_world_tile(&x, &y, 49, 0);
	assert(is_close(x, 49.0) && is_close(y, 0.0));
	coord_world_tile(&x, &y, 49, 128);
	assert(is_close(x, -49.0) && is_close(y, 0.0));
	return 0;
}

static int test_nearest_tile (void) {
	size_t i, j;
	nearest_tile(&i, &j, 49.0, 0.0);
	assert(i == 49 && j == 0);
	nearest_tile(&i, &j, -49.0, -0.0);
	assert(i == 49 && j == 128);
	return 0;
}

static struct test TEST_LIST[NUM_WORLD_TESTS] = {
	{.func = test_coord_world_tile, .name = "test_coord_world_tile"},
	{.func = test_nearest_tile, .name = "test_nearest_tile"},
};

