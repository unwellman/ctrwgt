#include "state.h"
#include "log.h"

/* Call STACK->del() and pop the state at the top of the stack */
static void state_stack_pop ();

/* If ptr is in the stack, pop every state up to and including ptr.
 * If ptr is not in the stack, do nothing. */
static void pop_until (struct state *ptr);

/* Top of the stack */
static struct state *STACK = NULL;

enum state_response state_stack_iterate (double dt) {
	enum state_response ret = STATE_DEFER;
	// This works because next is the first member of struct state
	struct state *ptr = (struct state *) &STACK;
	while (ret == STATE_DEFER) {
		ptr = ptr->next; // On first loop, now ptr = STACK
		if (!ptr) {
			log_critical("Iteration fell through the state stack");
			state_stack_destroy();
			return STATE_FAILURE;
		}
		if (ptr->iterate)
			ret = ptr->iterate(ptr, ret, dt);
	}
	switch (ret) {
	case STATE_RETURN:
	case STATE_FAILURE:
		pop_until(ptr);
	default:
		return ret;
	}
}

enum state_response state_stack_event (void *event) {
	enum state_response ret = STATE_DEFER;
	struct state *ptr = (struct state*) &STACK;
	while (ret == STATE_DEFER) {
		ptr = ptr->next;
		if (!ptr) {
			log_critical("Event fell through the state stack");
			state_stack_destroy();
			return STATE_FAILURE;
		}
		if (ptr->event)
			ret = ptr->event(ptr, event);
	}
	switch (ret) {
	case STATE_RETURN:
	case STATE_FAILURE:
		pop_until(ptr);
	default:
		return ret;
	}
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
	default:
		state_stack_pop();
		return ret;
	}
}

static void state_stack_pop () {
	if (!STACK) // In principle, this should never happen
		return;
	if (STACK->del)
		STACK->del(STACK);
	STACK = STACK->next;
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

