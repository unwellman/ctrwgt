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

/* Write the main screen texture to dst and return the FRect bounding the
 * requested layer. dst is ignored if NULL.
 *
 * Currently hard-coded to 16 layers.
 * */
SDL_FRect render_get_layer (SDL_Renderer **ren_dst, SDL_Texture **tex_dst,
		Uint32 layer);

int render_present (void);

#endif

