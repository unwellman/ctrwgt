#include "test.h"
#include "world.h"

#define NUM_WORLD_TESTS 1
static struct test TEST_LIST[NUM_WORLD_TESTS];
// Export
struct test_batch world_tests = {
	.count = NUM_WORLD_TESTS,
	.ptr = TEST_LIST
};

static int test_blank (void) {
	return 0;
}

static struct test TEST_LIST[NUM_WORLD_TESTS] = {
	{.func = test_blank, .name = "test_blank"},
};

