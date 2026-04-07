#ifndef CTRW_STATE_H
#define CTRW_STATE_H

enum state_response {
	/* State was successful and should be destroyed */
	STATE_RETURN,
	/* State was successful and should be iterated again */
	STATE_CONTINUE,
	/* State failed and should be destroyed */
	STATE_FAILURE,
	/* The next highest state in the stack should receive the same signal */
	STATE_DEFER,
};

struct state {
	/* For use only within the implementation of the stack.
	 * Having a linked list baked in allows the stack implementation to
	 * avoid making allocations or having a size limitation. */
	struct state *next;

	/* Comment used for debugging */
	const char *name;

	/* Pointer to data used by the state.
	 * Generally, this should only be cast within the state's own methods. */
	void *data;

	/* Initialize any necessary data and set the data pointer.
	 * If no initializer is provided, iterate() will still be called at least
	 * once. */
	enum state_response (*init)(struct state *self);

	/* Run the main loop, i.e. the code to draw a frame. */
	enum state_response (*iterate)(struct state *self,
			enum state_response prev);

	/* Pass an event to this state */
	enum state_response (*event)(struct state *self, void *event);
	
	/* Prepare this state for destruction */
	void (*del)(struct state *self);
};

/* Basic state which fails if it receives an event signal.
 * To be defined in main.c */
extern struct state *GROUND_STATE;

/* Push a new state onto the stack and call ptr->init()
 * Note that the state will be popped without calling iterate() if it returns
 * STATE_RETURN. */
enum state_response state_stack_push (struct state *ptr);

/* Return a pointer to the state on top of the stack without changing the stack */
struct state * state_stack_peek ();

/* Check if the state stack is empty and return 1 if empty, 0 if not */
int state_stack_empty ();

/* Run the iterate method of the state at the top of the stack.
 * Continue down the stack running state.iterate() until a state returns something
 * other than STATE_DEFER.
 * If the bottom state run returns STATE_RETURN or STATE_FAILURE, all states above
 * and including it will be popped in order.
 * If the bottom state run returns STATE_CONTINUE, then this function returns
 * STATE_CONTINUE and will not modify the stack. */
enum state_response state_stack_iterate ();

/* Pass an event to be handled by the state stack. event is assumed to be a
 * pointer to an SDL_Event object, but the state stack does not need to know this.
 * */
enum state_response state_stack_event (void *event);

/* Pop every element of the stack. */
void state_stack_destroy ();

#endif

