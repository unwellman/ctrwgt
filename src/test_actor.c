#include <assert.h>
#include "test.h"
#include "actor.h"

#define NUM_ACTOR_TESTS 2
static struct test actor_tests[NUM_ACTOR_TESTS];

int test_actor (void) {
	for (int i = 0; i < NUM_ACTOR_TESTS; i++) {
		struct test curr = actor_tests[i];
		printf("Running %s\n", curr.name);
		int result = curr.func();
		if (result)
			return result;
	}
	return 0;
}

static int test_actor_update (void) {
	return 0;
}

static int test_actor_message (void) {
	return 0;
}

static struct test actor_tests[NUM_ACTOR_TESTS] = {
	{.func = test_actor_update, .name = "test_actor_update"},
	{test_actor_message, "test_actor_message"}
};

