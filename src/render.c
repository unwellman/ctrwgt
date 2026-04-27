#include "render.h"
#include "log.h"


static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static SDL_Texture *screen = NULL;
static struct renderer_create_info render_state = {0};

char WINDOW_TITLE[] = "Counterweight";
struct renderer_create_info RENDERER_DEFAULTS = {
	.window_w = 1920,
	.window_h = 1080,
	.logical_w = 1920,
	.logical_h = 1080,
	.layers = 16,
	.title = WINDOW_TITLE
};

/* Explicit triangle geometry for 16 screen layers tiled across
 * a 1920x1080 texture
 * {.position = {x, y}, .color = {r, g, b, a}, .tex_coord = {x, y}}
*/
#define NUM_VERTICES 64
#define NUM_INDICES 96
#define COLOR_OPAQUE {1.0f, 1.0f, 1.0f, 1.0f}
static SDL_Vertex geometry[NUM_VERTICES] = {
	{{0, 1080},	COLOR_OPAQUE, {0.0, 0.25}}, // Layer 0
	{{1920, 1080},	COLOR_OPAQUE, {0.25, 0.25}},
	{{1920, 0},	COLOR_OPAQUE, {0.25, 0.0}},
	{{0, 0},	COLOR_OPAQUE, {0.0, 0.0}},
	{{0, 1080},	COLOR_OPAQUE, {0.25, 0.25}}, // Layer 1
	{{1920, 1080},	COLOR_OPAQUE, {0.50, 0.25}},
	{{1920, 0},	COLOR_OPAQUE, {0.50, 0.0}},
	{{0, 0},	COLOR_OPAQUE, {0.25, 0.0}},
	{{0, 1080},	COLOR_OPAQUE, {0.50, 0.25}}, // Layer 2
	{{1920, 1080},	COLOR_OPAQUE, {0.75, 0.25}},
	{{1920, 0},	COLOR_OPAQUE, {0.75, 0.0}},
	{{0, 0},	COLOR_OPAQUE, {0.50, 0.0}},
	{{0, 1080},	COLOR_OPAQUE, {0.75, 0.25}}, // Layer 3
	{{1920, 1080},	COLOR_OPAQUE, {1.0, 0.25}},
	{{1920, 0},	COLOR_OPAQUE, {1.0, 0.0}},
	{{0, 0},	COLOR_OPAQUE, {0.75, 0.0}},
	{{0, 1080},	COLOR_OPAQUE, {0.0, 0.50}}, // Layer 4
	{{1920, 1080},	COLOR_OPAQUE, {0.25, 0.50}},
	{{1920, 0},	COLOR_OPAQUE, {0.25, 0.25}},
	{{0, 0},	COLOR_OPAQUE, {0.0, 0.25}},
	{{0, 1080},	COLOR_OPAQUE, {0.25, 0.50}}, // Layer 5
	{{1920, 1080},	COLOR_OPAQUE, {0.50, 0.50}},
	{{1920, 0},	COLOR_OPAQUE, {0.50, 0.25}},
	{{0, 0},	COLOR_OPAQUE, {0.25, 0.25}},
	{{0, 1080},	COLOR_OPAQUE, {0.50, 0.50}}, // Layer 6
	{{1920, 1080},	COLOR_OPAQUE, {0.75, 0.50}},
	{{1920, 0},	COLOR_OPAQUE, {0.75, 0.25}},
	{{0, 0},	COLOR_OPAQUE, {0.50, 0.25}},
	{{0, 1080},	COLOR_OPAQUE, {0.75, 0.50}}, // Layer 7
	{{1920, 1080},	COLOR_OPAQUE, {1.0, 0.50}},
	{{1920, 0},	COLOR_OPAQUE, {1.0, 0.25}},
	{{0, 0},	COLOR_OPAQUE, {0.75, 0.25}},
	{{0, 1080},	COLOR_OPAQUE, {0.0, 0.75}}, // Layer 8
	{{1920, 1080},	COLOR_OPAQUE, {0.25, 0.75}},
	{{1920, 0},	COLOR_OPAQUE, {0.25, 0.50}},
	{{0, 0},	COLOR_OPAQUE, {0.0, 0.50}},
	{{0, 1080},	COLOR_OPAQUE, {0.25, 0.75}}, // Layer 9
	{{1920, 1080},	COLOR_OPAQUE, {0.50, 0.75}},
	{{1920, 0},	COLOR_OPAQUE, {0.50, 0.50}},
	{{0, 0},	COLOR_OPAQUE, {0.25, 0.50}},
	{{0, 1080},	COLOR_OPAQUE, {0.50, 0.75}}, // Layer 10
	{{1920, 1080},	COLOR_OPAQUE, {0.75, 0.75}},
	{{1920, 0},	COLOR_OPAQUE, {0.75, 0.50}},
	{{0, 0},	COLOR_OPAQUE, {0.50, 0.50}},
	{{0, 1080},	COLOR_OPAQUE, {0.75, 0.75}}, // Layer 11
	{{1920, 1080},	COLOR_OPAQUE, {1.0, 0.75}},
	{{1920, 0},	COLOR_OPAQUE, {1.0, 0.50}},
	{{0, 0},	COLOR_OPAQUE, {0.75, 0.50}},
	{{0, 1080},	COLOR_OPAQUE, {0.0, 1.0}}, // Layer 12
	{{1920, 1080},	COLOR_OPAQUE, {0.25, 1.0}},
	{{1920, 0},	COLOR_OPAQUE, {0.25, 0.75}},
	{{0, 0},	COLOR_OPAQUE, {0.0, 0.75}},
	{{0, 1080},	COLOR_OPAQUE, {0.25, 1.0}}, // Layer 13
	{{1920, 1080},	COLOR_OPAQUE, {0.50, 1.0}},
	{{1920, 0},	COLOR_OPAQUE, {0.50, 0.75}},
	{{0, 0},	COLOR_OPAQUE, {0.25, 0.75}},
	{{0, 1080},	COLOR_OPAQUE, {0.50, 1.0}}, // Layer 14
	{{1920, 1080},	COLOR_OPAQUE, {0.75, 1.0}},
	{{1920, 0},	COLOR_OPAQUE, {0.75, 0.75}},
	{{0, 0},	COLOR_OPAQUE, {0.50, 0.75}},
	{{0, 1080},	COLOR_OPAQUE, {0.75, 1.0}}, // Layer 15
	{{1920, 1080},	COLOR_OPAQUE, {1.0, 1.0}},
	{{1920, 0},	COLOR_OPAQUE, {1.0, 0.75}},
	{{0, 0},	COLOR_OPAQUE, {0.75, 0.75}}
};
static int indices[NUM_INDICES] = {
	0, 1, 2, 0, 2, 3, // Layer 0
	4, 5, 6, 4, 6, 7, // Layer 1
	8, 9, 10, 8, 10, 11, // Layer 2
	12, 13, 14, 12, 14, 15, // Layer 3
	16, 17, 18, 16, 18, 19, // Layer 4
	20, 21, 22, 20, 22, 23, // Layer 5
	24, 25, 26, 24, 26, 27, // Layer 6
	28, 29, 30, 28, 30, 31, // Layer 7
	32, 33, 34, 32, 34, 35, // Layer 8
	36, 37, 38, 36, 38, 39, // Layer 9
	40, 41, 42, 40, 42, 43, // Layer 10
	44, 45, 46, 44, 46, 47, // Layer 11
	48, 49, 50, 48, 50, 51, // Layer 12
	52, 53, 54, 52, 54, 55, // Layer 13
	56, 57, 58, 56, 58, 59, // Layer 14
	60, 61, 62, 60, 62, 63 // Layer 15
};

int render_init (SDL_Renderer **dst, struct renderer_create_info *info) {
	render_state = *info;
	window = SDL_CreateWindow(info->title, info->window_w, info->window_h,
			SDL_WINDOW_RESIZABLE);
	if (!window) {
		log_critical("Window creation failed: %s", SDL_GetError());
		return -1;
	}

	renderer = SDL_CreateGPURenderer(NULL, window);
	if (!renderer) {
		log_critical("Renderer creation failed: %s", SDL_GetError());
		return -1;
	}
	SDL_SetRenderLogicalPresentation(renderer, info->logical_w,
			info->logical_h, SDL_LOGICAL_PRESENTATION_INTEGER_SCALE);

	// Pixel format inferred by loading a PNG
	screen = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888,
			SDL_TEXTUREACCESS_TARGET, info->logical_w,
			info->logical_h);
	if (!screen) {
		log_critical("Failed to create screen texture: %s",
				SDL_GetError());
		return -1;
	}
	log_debug("screen: %d, %d", screen->w, screen->h);
	SDL_SetTextureScaleMode(screen, SDL_SCALEMODE_NEAREST);
	// By default, draw to the screen and composite in render_present
	SDL_SetRenderTarget(renderer, screen);
	if (dst)
		*dst = renderer;
	return 0;
}

void render_dest (void) {
	if (window)
		SDL_DestroyWindow(window);
	if (renderer)
		SDL_DestroyRenderer(renderer);
	if (screen)
		SDL_DestroyTexture(screen);
}

int render_present (void) {
	SDL_Texture *tmp = SDL_GetRenderTarget(renderer);
	SDL_SetRenderTarget(renderer, NULL); // Draw to the window

	SDL_SetRenderDrawColor(renderer, 64, 64, 64, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(renderer);


	SDL_RenderGeometry(renderer, screen, geometry, NUM_VERTICES,
			indices, NUM_INDICES);

	SDL_RenderPresent(renderer);
	SDL_SetRenderTarget(renderer, tmp); // Restore previous target
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_TRANSPARENT);
	SDL_RenderClear(renderer);
	return 0;
}

static SDL_FRect layer_rect (Uint32 layer) {
	SDL_FPoint upper_left = geometry[4*layer + 3].tex_coord;
	float w = (float) render_state.logical_w;
	float h = (float) render_state.logical_h;
	return (SDL_FRect) {w*upper_left.x, h*upper_left.y, w/4, h/4};
}

SDL_FRect render_get_layer (SDL_Renderer **ren_dst, SDL_Texture **tex_dst,
		Uint32 layer) {
	if (!(0 <= layer && layer < 16))
		return (SDL_FRect) {0};
	if (ren_dst)
		*ren_dst = renderer;
	if (tex_dst)
		*tex_dst = screen;
	return layer_rect(layer);
}

SDL_Renderer * render_set_layer (Uint32 layer) {
	if (!(0 <= layer && layer < 16)) // Not permissible
		return NULL;
	SDL_FRect rect = layer_rect(layer);
	SDL_Rect viewport = {
		.x = (int) rect.x, .y = (int) rect.y,
		.w = (int) rect.y, .h = (int) rect.h
	};
	if(!SDL_SetRenderViewport(renderer, &viewport))
		log_error("Viewport change failed: %s", SDL_GetError());
	return renderer;
}

