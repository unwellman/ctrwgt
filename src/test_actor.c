#include <assert.h>
#include "test.h"
#include "actor.h"

#define NUM_ACTOR_TESTS 2
static struct test TEST_LIST[NUM_ACTOR_TESTS];
// Export
struct test_batch actor_tests = {
	.count = NUM_ACTOR_TESTS,
	.ptr = TEST_LIST
};

static int test_actor_update (void) {
	return 0;
}

static int test_actor_message (void) {
	return 0;
}

static struct test TEST_LIST[NUM_ACTOR_TESTS] = {
	{.func = test_actor_update, .name = "test_actor_update"},
	{test_actor_message, "test_actor_message"}
};

