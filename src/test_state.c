#include "test.h"
#include "state.h"

/*
static void *data;
static char *name = "test_state";
static enum state_response init (struct state *self) {
	return STATE_CONTINUE;
}
*/

static int test_state_stack_push (void) {
	return 0;
}

static int test_state_stack_peek (void) {
	return 0;
}

static int test_state_stack_empty (void) {
	return 0;
}

static int test_state_stack_iterate (void) {
	return 0;
}

static int test_state_stack_destroy (void) {
	return 0;
}

static struct test TEST_LIST[] = {
	TEST(test_state_stack_destroy),
	TEST(test_state_stack_iterate),
	TEST(test_state_stack_empty),
	TEST(test_state_stack_peek),
	TEST(test_state_stack_push)
};
EXPORT(TEST_LIST, state)

