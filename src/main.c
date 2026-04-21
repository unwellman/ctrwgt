#include <stdio.h>
#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_log.h>

#include "state.h"
#include "log.h"

#include "sprite.h"
#define LOGGING_DEV

static Uint64 NS_ELAPSED;

SDL_AppResult SDL_AppInit (void **app_state, int argc, char *argv[]) {
	NS_ELAPSED = SDL_GetTicksNS();
#ifdef LOGGING_DEV
	SDL_SetLogPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_TRACE);
#endif
	state_stack_push(GROUND_STATE);
	state_stack_push(&sprite_artist);
	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate (void *app_state) {
	Uint64 tmp = SDL_GetTicksNS();
	double dt = SDL_NS_TO_SECONDS((double) tmp - NS_ELAPSED);
	NS_ELAPSED = tmp;
	enum state_response result = state_stack_iterate(dt);
	if (state_stack_peek())
		return SDL_APP_CONTINUE;
	switch (result) {
	case STATE_RETURN:
		return SDL_APP_SUCCESS;
	default:
		return SDL_APP_FAILURE;
	}
}

SDL_AppResult SDL_AppEvent (void *app_state, SDL_Event *event) {
	enum state_response result = state_stack_event(event);
	switch (result) {
	case STATE_CONTINUE:
		return SDL_APP_CONTINUE;
	case STATE_RETURN:
		return SDL_APP_SUCCESS;
	default:
		return SDL_APP_FAILURE;
	}
}

void SDL_AppQuit (void *app_state, SDL_AppResult result) {
	log_trace("Destroying state stack");
	state_stack_destroy();
}

/* GROUND_STATE structures */
static enum state_response ground_state_init (struct state *self) {
	return STATE_CONTINUE;
}
static double ground_state_time_elapsed = 0;
static Uint64 ground_state_second_counter = 0;
static enum state_response ground_state_iterate (struct state *self,
		enum state_response prev, double dt) {
	ground_state_time_elapsed += dt;
	if ((Uint64) ground_state_time_elapsed > ground_state_second_counter) {
		ground_state_second_counter++;
		log_debug("Ground state seconds elapsed: %zu",
				ground_state_second_counter);
	}
	return STATE_CONTINUE;
}
static enum state_response ground_state_event (struct state *self,
		void *event) {
	if (((SDL_Event *) event)->type == SDL_EVENT_QUIT) {
		log_trace("Received quit signal");
		return STATE_RETURN;
	}
	log_critical("Unhandled event");
	return STATE_FAILURE;
}
static void ground_state_del (struct state *self) {
	return;
}

static struct state ground_state = {
	.name = "GROUND_STATE",
	.data = NULL,
	.init = ground_state_init,
	.iterate = ground_state_iterate,
	.event = ground_state_event,
	.del = ground_state_del
};

struct state *GROUND_STATE = &ground_state;

