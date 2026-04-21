#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>

#include "log.h"
#include "sprite.h"
#include "state.h"

/*
 * An exercise in actually using SDL
 * */

struct sprite_data {
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Texture *texture;
};
static struct sprite_data GLOBAL_DATA;

static enum state_response sprite_init (struct state *state) {
	struct sprite_data *data = state->data;
	SDL_Surface *surface = NULL;
	char *fp = NULL;
	
	SDL_SetAppMetadata("Counterweight", "0.0", "net.ddns.haruhionly.ctrwgt");

	if (!SDL_Init(SDL_INIT_VIDEO)) {
		log_critical("SDL initialization failed: %s", SDL_GetError());
		return STATE_FAILURE;
	}

	if (!SDL_CreateWindowAndRenderer("Counterweight",
				1920, 1080, SDL_WINDOW_RESIZABLE,
				&data->window, &data->renderer)) {
		log_critical("Window/renderer init failed: %s", SDL_GetError());
		return STATE_FAILURE;
	}
	SDL_SetRenderLogicalPresentation(data->renderer, 360, 270,
			SDL_LOGICAL_PRESENTATION_INTEGER_SCALE);


	SDL_asprintf(&fp, "%s../res/migu.png", SDL_GetBasePath());
	surface = SDL_LoadPNG(fp);
	SDL_free(fp);

	data->texture = SDL_CreateTextureFromSurface(data->renderer,
			surface);
	if (!data->texture) {
		log_critical("Texture creation: %s", SDL_GetError());
		return STATE_FAILURE;
	}
	SDL_SetTextureScaleMode(data->texture, SDL_SCALEMODE_NEAREST);
	if (surface->w != 256 || surface->h != 128) {
		log_critical("Texture size is wrong: (%d, %d)", surface->w,
				surface->h);
		return STATE_FAILURE;
	}

	SDL_DestroySurface(surface);

	return STATE_CONTINUE;
}

static double time_elapsed = 0.0;
static Uint64 tick_counter = 0;

static enum state_response sprite_loop (struct state *state, enum state_response prev, double dt) {
	struct sprite_data *data = state->data;
	time_elapsed += dt;
	if ((Uint64) (24*time_elapsed) > tick_counter) {
		tick_counter++;
		log_info("Rendered frame %zu", tick_counter);
	}
	SDL_SetRenderDrawColor(data->renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(data->renderer);
	
	SDL_FRect dst = {
		.x = 164,
		.y = 119,
		.w = 32,
		.h = 32
	};
	SDL_FRect src = {
		.x = 32*((tick_counter/3) % 8),
		.y = 32,
		.w = 32,
		.h = 32
	};
	SDL_RenderTexture(data->renderer, data->texture, &src, &dst);
	SDL_RenderPresent(data->renderer);
	return STATE_CONTINUE;
}

static enum state_response sprite_event (struct state *state, void *event) {
	SDL_EventType type = ((SDL_Event *) event)->type;
	if (type == SDL_EVENT_QUIT)
		return STATE_DEFER;
	log_info("Sprite received event %d", type);
	
	return STATE_CONTINUE;
}

static void sprite_exit (struct state *state) {
	SDL_DestroyTexture(((struct sprite_data *)state->data)->texture);
}

static const char * const name = "sprite_artist";
struct state sprite_artist = {
	.name = name,
	.data = &GLOBAL_DATA,
	.init = sprite_init,
	.iterate = sprite_loop,
	.event = sprite_event,
	.del = sprite_exit
};

