#ifndef CTRW_RENDER_H
#define CTRW_RENDER_H

#include <stdbool.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_properties.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_hints.h>

struct window_create_info {
	// External
	const char *title;
	char *display_name;
	bool fullscreen, maximized, resizable, hidden;
	int target_width, target_height;
	int max_width, max_height;
	
	// Internal
	SDL_Rect bounds;
	SDL_DisplayID display;
};

extern struct window_create_info WINDOW_DEFAULTS;

/* The initialization behavior is hardcoded, so info will be ignored.
 * I may not ever change this fact.
 * */
int render_init (SDL_Renderer **dst, struct window_create_info *info);

void render_dest (void);

/* Change the renderer's viewport to draw on the given layer, returning
 * a pointer to the renderer.
 *
 * Currently hard-coded to 16 layers.
 * */
SDL_Renderer * render_set_layer (Uint32 layer);

int render_present (void);

#endif

