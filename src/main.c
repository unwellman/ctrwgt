#include <stdio.h>
#include <assert.h>
#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_log.h>

#include "state.h"
#include "log.h"
#define LOGGING_DEV

static Uint64 NS_ELAPSED;
SDL_AppResult SDL_AppInit (void **app_state, int argc, char *argv[]) {
	NS_ELAPSED = SDL_GetTicksNS();
	SDL_SetAppMetadata("Counterweight", "0.0", "net.ddns.haruhionly.ctrwgt");
	SDL_SetLogPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_TRACE);

	if (!SDL_Init(SDL_INIT_VIDEO)) {
		log_critical("SDL init failed: %s", SDL_GetError());
		return SDL_APP_FAILURE;
	}

	state_stack_push(GROUND_STATE);
	state_stack_push(WINDOW_STATE);
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

static enum state_response ground_state_iterate (struct state *self,
		enum state_response prev, double dt) {
	// Keep track of time
	return STATE_CONTINUE;
}

static void ground_state_del (struct state *self) {
	return;
}

static enum state_response ground_state_event (struct state *self,
		void *event) {
	Uint32 type = ((SDL_Event *) event)->type;
	if (type == SDL_EVENT_QUIT) {
		log_trace("Received quit signal");
		return STATE_RETURN;
	}
	log_error("Unhandled event 0x%x", type);
	return STATE_CONTINUE;
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

