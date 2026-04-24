#ifndef CTRW_RENDER_H
#define CTRW_RENDER_H

#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>

struct renderer_create_info {
	Uint32 window_w, window_h, logical_w, logical_h;
	Uint32 layers;
	const char *title;
};

extern struct renderer_create_info RENDERER_DEFAULTS;

/* The initialization behavior is hardcoded, so info will be ignored.
 * I may not ever change this fact.
 * */
int render_init (SDL_Renderer **dst, struct renderer_create_info *info);

void render_dest (void);

/* Change the renderer's viewport to draw on the given layer, returning
 * a pointer to the renderer.
 *
 * Currently hard-coded to 16 layers.
 * */
SDL_Renderer * render_set_layer (Uint32 layer);

int render_present (void);

#endif

