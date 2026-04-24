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
#include "render.h"
#define LOGGING_DEV

static Uint64 NS_ELAPSED;
SDL_AppResult SDL_AppInit (void **app_state, int argc, char *argv[]) {
	NS_ELAPSED = SDL_GetTicksNS();
	SDL_SetAppMetadata("Counterweight", "0.0", "net.ddns.haruhionly.ctrwgt");
	SDL_SetLogPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_TRACE);

	if (!SDL_Init(SDL_INIT_VIDEO)) {
		log_critical("SDL init failed: %s", SDL_GetError());
		return -1;
	}

	state_stack_push(GROUND_STATE);
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
static double ground_state_time_elapsed = 0;
static Uint64 ground_state_second_counter = 0;
static Uint64 frame_counter = 0;
static double measured_frame_rate = 0;

static enum state_response ground_state_init (struct state *self) {
	int res = render_init(NULL, &RENDERER_DEFAULTS);
	if (res)
		return STATE_FAILURE;
	return STATE_CONTINUE;
}

static enum state_response ground_state_iterate (struct state *self,
		enum state_response prev, double dt) {
	// Keep track of time
	frame_counter++;
	ground_state_time_elapsed += dt;
	if ((Uint64) ground_state_time_elapsed > ground_state_second_counter) {
		ground_state_second_counter++;
		measured_frame_rate = (double) frame_counter;
		frame_counter = 0;
	}

	// Draw some debug text
	SDL_Renderer *renderer = render_set_layer(15);
	if (!renderer) {
		log_critical("Render layer out of range");
		return STATE_FAILURE;
	}
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
	SDL_RenderDebugTextFormat(renderer, 0.0, 0.0, "%.0f FPS",
			measured_frame_rate);

	render_present();
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
	log_error("Unhandled event %d", type);
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

