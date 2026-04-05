#include "test.h"
#include "state.h"

/* TEST DATA STRUCTURES */
static int COUNTERS[] = {0, 0, 0, 0}; // {init, iterate, event, del}
static int data = 42;
static char * const name = "test_state";
static enum state_response init (struct state *self) {
	COUNTERS[0]++;
	return STATE_CONTINUE;
}
static enum state_response iterate (struct state *self,
		enum state_response prev) {
	COUNTERS[1]++;
	return STATE_CONTINUE;
}
static enum state_response event (struct state *self, void *event) {
	COUNTERS[2]++;
	return STATE_FAILURE;
}
static void del (struct state *self) {
	COUNTERS[3]++;
	return;
}
static struct state ground = {
	.name = name,
	.data = &data,
	.init = init,
	.iterate = iterate,
	.event = event,
	.del = del
};
/* END TEST DATA STRUCTURES */

static int test_state_stack_empty (void) {
	assert(state_stack_empty());
	return 0;
}

static int test_state_stack_push (void) {
	state_stack_push(&ground);
	assert(COUNTERS[0] == 1);
	return 0;
}

static int test_state_stack_peek (void) {
	assert(state_stack_peek() == &ground);
	return 0;
}

static int test_state_stack_iterate (void) {
	state_stack_iterate();
	assert(COUNTERS[1] == 1);
	return 0;
}

static int test_state_stack_destroy (void) {
	state_stack_destroy();
	assert(COUNTERS[3] == 1);
	assert(state_stack_empty());
	return 0;
}

static struct test TEST_LIST[] = {
	TEST(test_state_stack_empty),
	TEST(test_state_stack_push),
	TEST(test_state_stack_peek),
	TEST(test_state_stack_iterate),
	TEST(test_state_stack_destroy)
};
EXPORT(TEST_LIST, state)

