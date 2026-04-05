#include "state.h"
#include "log.h"

/* If ptr is in the stack, pop every state up to and including ptr.
 * If ptr is not in the stack, do nothing. */
static void pop_until (struct state *ptr);

/* Pop the state at the top of the stack and call ptr->del() */
static enum state_response state_stack_pop ();

/* Top of the stack */
static struct state *STACK = NULL;

/* Stores the previous return value of state_stack_iterate */
static enum state_response PREVIOUS = STATE_CONTINUE;

enum state_response state_stack_iterate () {
	enum state_response ret;
	struct state *ptr = STACK;
	do {
		if (!ptr) {
			state_stack_destroy();
			return STATE_FAILURE;
		}
		ret = ptr->iterate(ptr, PREVIOUS);
		ptr = ptr->next;
	} while (ret == STATE_DEFER);
	return ret;
}

void state_stack_destroy () {
	while (STACK)
		state_stack_pop();
}

enum state_response state_stack_push (struct state *ptr) {
	struct state *tmp = STACK;
	STACK = ptr;
	STACK->next = tmp;
	enum state_response ret;
	if (STACK->init)
		ret = STACK->init(ptr);
	else {
		log_warn("State <%s> pushed with no initializer", ptr->name);
		return STATE_CONTINUE;
	}
	switch (ret) {
	case STATE_CONTINUE:
		return ret;
	case STATE_RETURN:
	case STATE_FAILURE:
	case STATE_DEFER: // Disallowed
		state_stack_pop();
		return ret;
	}
}

static enum state_response state_stack_pop () {
	if (!STACK) // In principle, this should never happen
		return STATE_FAILURE;
	if (STACK->del)
		STACK->del(STACK);
	STACK = STACK->next;
	if (!STACK)
		return STATE_RETURN;
	return STATE_CONTINUE;
}

struct state * state_stack_peek () {
	return STACK;
}

int state_stack_empty () {
	if (STACK)
		return 0;
	else
		return 1;
}

static void pop_until (struct state *ptr) {
	size_t ctr = 1;
	struct state *tmp = STACK;
	while (tmp != ptr) {
		if (tmp == NULL)
			return;
		tmp = tmp->next;
		ctr++;
	}
	for (size_t i = 0; i < ctr; i++)
		state_stack_pop();
}

