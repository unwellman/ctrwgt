#include "test.h"
#include "actor.h"

static int test_actor_update (void) {
	return 0;
}

static int test_actor_message (void) {
	return 0;
}

static struct test TEST_LIST[] = {
	TEST(test_actor_update),
	TEST(test_actor_message)
};
EXPORT(TEST_LIST, actor)

