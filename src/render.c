#include "render.h"
#include "log.h"

#define SCREEN_LAYER_MAX_SCALE 32

struct render_state {
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Texture *screen;
	int w, h;
};
static struct render_state RENDER_STATE;

char WINDOW_TITLE[] = "Counterweight";
char DISPLAY_NAME[] = "Built-in Retina Display";
struct window_create_info WINDOW_DEFAULTS = {
	.title = WINDOW_TITLE,
	.display_name = DISPLAY_NAME,
	.fullscreen = false,
	.maximized = false,
	.resizable = true,
	.hidden = true,
	.target_width = 480,
	.target_height = 270,
	.max_width = 640,
	.max_height = 360
};

/* Explicit triangle geometry for 16 screen layers tiled across a texture.
 * {.position = {x, y}, .color = {r, g, b, a}, .tex_coord = {x, y}}
 * Positions will be loaded at runtime as necessary.
*/
#define COLOR_OPAQUE {1.0f, 1.0f, 1.0f, 1.0f}
#define SCREEN_LAYER_NUM_VERTICES 64
#define SCREEN_LAYER_NUM_INDICES 96
static SDL_Vertex SCREEN_LAYER_GEOMETRY[SCREEN_LAYER_NUM_VERTICES] = {
	// Coordinate template:
	// {0, y}, {x, y}, {x, 0}, {0, 0}
	{{0}, COLOR_OPAQUE, {0.0, 0.25}}, // Layer 0
	{{0}, COLOR_OPAQUE, {0.25, 0.25}},
	{{0}, COLOR_OPAQUE, {0.25, 0.0}},
	{{0}, COLOR_OPAQUE, {0.0, 0.0}},
	{{0}, COLOR_OPAQUE, {0.25, 0.25}}, // Layer 1
	{{0}, COLOR_OPAQUE, {0.50, 0.25}},
	{{0}, COLOR_OPAQUE, {0.50, 0.0}},
	{{0}, COLOR_OPAQUE, {0.25, 0.0}},
	{{0}, COLOR_OPAQUE, {0.50, 0.25}}, // Layer 2
	{{0}, COLOR_OPAQUE, {0.75, 0.25}},
	{{0}, COLOR_OPAQUE, {0.75, 0.0}},
	{{0}, COLOR_OPAQUE, {0.50, 0.0}},
	{{0}, COLOR_OPAQUE, {0.75, 0.25}}, // Layer 3
	{{0}, COLOR_OPAQUE, {1.0, 0.25}},
	{{0}, COLOR_OPAQUE, {1.0, 0.0}},
	{{0}, COLOR_OPAQUE, {0.75, 0.0}},
	{{0}, COLOR_OPAQUE, {0.0, 0.50}}, // Layer 4
	{{0}, COLOR_OPAQUE, {0.25, 0.50}},
	{{0}, COLOR_OPAQUE, {0.25, 0.25}},
	{{0}, COLOR_OPAQUE, {0.0, 0.25}},
	{{0}, COLOR_OPAQUE, {0.25, 0.50}}, // Layer 5
	{{0}, COLOR_OPAQUE, {0.50, 0.50}},
	{{0}, COLOR_OPAQUE, {0.50, 0.25}},
	{{0}, COLOR_OPAQUE, {0.25, 0.25}},
	{{0}, COLOR_OPAQUE, {0.50, 0.50}}, // Layer 6
	{{0}, COLOR_OPAQUE, {0.75, 0.50}},
	{{0}, COLOR_OPAQUE, {0.75, 0.25}},
	{{0}, COLOR_OPAQUE, {0.50, 0.25}},
	{{0}, COLOR_OPAQUE, {0.75, 0.50}}, // Layer 7
	{{0}, COLOR_OPAQUE, {1.0, 0.50}},
	{{0}, COLOR_OPAQUE, {1.0, 0.25}},
	{{0}, COLOR_OPAQUE, {0.75, 0.25}},
	{{0}, COLOR_OPAQUE, {0.0, 0.75}}, // Layer 8
	{{0}, COLOR_OPAQUE, {0.25, 0.75}},
	{{0}, COLOR_OPAQUE, {0.25, 0.50}},
	{{0}, COLOR_OPAQUE, {0.0, 0.50}},
	{{0}, COLOR_OPAQUE, {0.25, 0.75}}, // Layer 9
	{{0}, COLOR_OPAQUE, {0.50, 0.75}},
	{{0}, COLOR_OPAQUE, {0.50, 0.50}},
	{{0}, COLOR_OPAQUE, {0.25, 0.50}},
	{{0}, COLOR_OPAQUE, {0.50, 0.75}}, // Layer 10
	{{0}, COLOR_OPAQUE, {0.75, 0.75}},
	{{0}, COLOR_OPAQUE, {0.75, 0.50}},
	{{0}, COLOR_OPAQUE, {0.50, 0.50}},
	{{0}, COLOR_OPAQUE, {0.75, 0.75}}, // Layer 11
	{{0}, COLOR_OPAQUE, {1.0, 0.75}},
	{{0}, COLOR_OPAQUE, {1.0, 0.50}},
	{{0}, COLOR_OPAQUE, {0.75, 0.50}},
	{{0}, COLOR_OPAQUE, {0.0, 1.0}}, // Layer 12
	{{0}, COLOR_OPAQUE, {0.25, 1.0}},
	{{0}, COLOR_OPAQUE, {0.25, 0.75}},
	{{0}, COLOR_OPAQUE, {0.0, 0.75}},
	{{0}, COLOR_OPAQUE, {0.25, 1.0}}, // Layer 13
	{{0}, COLOR_OPAQUE, {0.50, 1.0}},
	{{0}, COLOR_OPAQUE, {0.50, 0.75}},
	{{0}, COLOR_OPAQUE, {0.25, 0.75}},
	{{0}, COLOR_OPAQUE, {0.50, 1.0}}, // Layer 14
	{{0}, COLOR_OPAQUE, {0.75, 1.0}},
	{{0}, COLOR_OPAQUE, {0.75, 0.75}},
	{{0}, COLOR_OPAQUE, {0.50, 0.75}},
	{{0}, COLOR_OPAQUE, {0.75, 1.0}}, // Layer 15
	{{0}, COLOR_OPAQUE, {1.0, 1.0}},
	{{0}, COLOR_OPAQUE, {1.0, 0.75}},
	{{0}, COLOR_OPAQUE, {0.75, 0.75}}
};
static int SCREEN_LAYER_INDICES[SCREEN_LAYER_NUM_INDICES] = {
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

SDL_Window * make_window_from_properties (struct window_create_info *info) {
	// Return value must be destroyed with SDL_DestroyProperties
	SDL_PropertiesID props = SDL_CreateProperties();
	// Title
	SDL_SetStringProperty(props,
		SDL_PROP_WINDOW_CREATE_TITLE_STRING, info->title
	);
	// Size & Position
	if (info->bounds.w != 0) {
		SDL_SetNumberProperty(props,
			SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, info->bounds.w
		);
	}
	if (info->bounds.h != 0) {
		SDL_SetNumberProperty(props,
			SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, info->bounds.h
		);
	}
	SDL_SetNumberProperty(props,
		SDL_PROP_WINDOW_CREATE_X_NUMBER, info->bounds.x
	);
	SDL_SetNumberProperty(props,
		SDL_PROP_WINDOW_CREATE_Y_NUMBER, info->bounds.y
	);
	// Fullscreen
	SDL_SetBooleanProperty(props,
		SDL_PROP_WINDOW_CREATE_FULLSCREEN_BOOLEAN, info->fullscreen
	);
	// Maximized
	SDL_SetBooleanProperty(props,
		SDL_PROP_WINDOW_CREATE_MAXIMIZED_BOOLEAN, info->maximized
	);
	// Resizable
	SDL_SetBooleanProperty(props,
		SDL_PROP_WINDOW_CREATE_RESIZABLE_BOOLEAN, info->resizable
	);
	//Hidden
	SDL_SetBooleanProperty(props,
		SDL_PROP_WINDOW_CREATE_HIDDEN_BOOLEAN, info->hidden
	);

	SDL_Window *ret = SDL_CreateWindowWithProperties(props);
	SDL_DestroyProperties(props);
	return ret;
}

SDL_DisplayID get_display_from_name (const char *name) {
	if (!name) {
		return SDL_GetPrimaryDisplay();
	}
	int count;
	SDL_DisplayID ret = 0;
	SDL_DisplayID *displays = SDL_GetDisplays(&count);
	for (int i = 0; i < count; i++) {
		const char *curr_name = SDL_GetDisplayName(displays[i]);
		log_debug("Checking Display %s", curr_name);
		if (!SDL_strcmp(curr_name, name)) {
			ret = displays[i];
			break;
		}
	}
	SDL_free(displays);
	if (!ret) {
		log_warn("No display found matching %s", name);
		ret = SDL_GetPrimaryDisplay();
	}
	return ret;
}

SDL_Window * window_init (struct window_create_info *info) {
	SDL_SetHint(SDL_HINT_VIDEO_MAC_FULLSCREEN_SPACES, "1");
	log_debug("Fullscreen space hint: %s",
			SDL_GetHint(SDL_HINT_VIDEO_MAC_FULLSCREEN_SPACES));
	// Search for display
	SDL_DisplayID display = get_display_from_name(info->display_name);
	if (!display) {
		log_critical(
		"No display found: %s", SDL_GetError());
		return NULL;
	}
	info->display = display;
	// Fullscreen
	if (info->fullscreen)
		return make_window_from_properties(info);

	// Windowed
	SDL_Rect bounds;
	if (!SDL_GetDisplayUsableBounds(display, &bounds)) {
		log_error(
		"Could not get display bounds; defaulting to 1920 x 1080: %s",
		SDL_GetError());
		bounds = (SDL_Rect) {0, 0, 1920, 1080};
	}
	// Optional logic to match aspect ratio
	info->bounds.x = bounds.x;
	info->bounds.y = bounds.y;
	info->bounds.w = bounds.w < info->target_width ?
		info->target_width : bounds.w;
	info->bounds.h = bounds.h < info->target_height ?
		info->target_height : bounds.h;
	return make_window_from_properties(info);
}

SDL_Texture * make_screen_layers (
		int target_width, int target_height,
		int max_width, int max_height,
		int *actual_width, int *actual_height) {
	SDL_PixelFormat format = SDL_GetWindowPixelFormat(RENDER_STATE.window);
	if (format == SDL_PIXELFORMAT_UNKNOWN) {
		log_error(
		"Window pixel format unknown; defaulting to ABGR8888: %s",
		SDL_GetError());
		format = SDL_PIXELFORMAT_ABGR8888;
	}
	int w, h;
	if (!SDL_GetWindowSizeInPixels(RENDER_STATE.window, &w, &h)) {
		log_error(
		"Could not get current window size: %s", SDL_GetError());
		return NULL;
	}
	int best_scale = 0;
	int best_score = target_height*target_height + target_width*target_width;
	for (int div = 1; div <= SCREEN_LAYER_MAX_SCALE; div++) {
		if (w / div > max_width || h / div > max_height)
			continue;
		int score = (target_width - w / div) * (target_width - w / div)
			+ (target_height - h / div) * (target_height - h / div);
		if (score < best_score) {
			best_score = score;
			best_scale = div;
		}
	}
	if (best_scale == 0) {
		log_error(
		"No good pixel scale. Is the display resolution super high?");
		best_scale = SCREEN_LAYER_MAX_SCALE;
	}
	log_debug("Scale factor chosen: %d", best_scale);
	*actual_width = w / best_scale;
	*actual_height = h / best_scale;
	
	SDL_Texture *ret = SDL_CreateTexture(RENDER_STATE.renderer, format,
		SDL_TEXTUREACCESS_TARGET,
		4*(*actual_width), 4*(*actual_height));
	return ret;
}

void set_screen_layer_coords (float w, float h) {
	// {0, y}, {x, y}, {x, 0}, {0, 0}
	if (SCREEN_LAYER_NUM_VERTICES % 4 != 0) {
		log_error("SCREEN_LAYER_NUM_VERTICES is not a multiple of 4");
		return;
	}
	SDL_Vertex *geom = SCREEN_LAYER_GEOMETRY;
	for (int i = 0; i < SCREEN_LAYER_NUM_VERTICES / 4; i++) {
		(geom + 4*i + 0)->position = (SDL_FPoint) {0, h};
		(geom + 4*i + 1)->position = (SDL_FPoint) {w, h};
		(geom + 4*i + 2)->position = (SDL_FPoint) {w, 0};
		(geom + 4*i + 3)->position = (SDL_FPoint) {0, 0};
	}
}

int render_init (SDL_Renderer **dst, struct window_create_info *info) {
	RENDER_STATE.window = window_init(info);
	if (!RENDER_STATE.window) {
		log_critical("Window creation failed: %s", SDL_GetError());
		return -1;
	}

	RENDER_STATE.renderer = SDL_CreateGPURenderer(NULL, RENDER_STATE.window);
	if (!RENDER_STATE.renderer) {
		log_critical("Renderer creation failed: %s", SDL_GetError());
		return -1;
	}

	RENDER_STATE.screen = make_screen_layers(
			info->target_width, info->target_height,
			info->max_width, info->max_height,
			&(RENDER_STATE.w), &(RENDER_STATE.h));
	if (!RENDER_STATE.screen) {
		log_critical("Failed to create screen texture: %s",
				SDL_GetError());
		return -1;
	}
	set_screen_layer_coords(
			(float) 4 * RENDER_STATE.w,
			(float) 4 * RENDER_STATE.h);
	SDL_SetTextureScaleMode(RENDER_STATE.screen, SDL_SCALEMODE_NEAREST);
	// Assuming window was created in a hidden state
	SDL_ShowWindow(RENDER_STATE.window);
	// By default, draw to the screen and composite in render_present
	SDL_SetRenderTarget(RENDER_STATE.renderer, RENDER_STATE.screen);
	if (dst)
		*dst = RENDER_STATE.renderer;
	return 0;
}

void render_dest (void) {
	if (RENDER_STATE.renderer)
		SDL_SetRenderTarget(RENDER_STATE.renderer, NULL);

	if (RENDER_STATE.screen)
		SDL_DestroyTexture(RENDER_STATE.screen);
	if (RENDER_STATE.renderer)
		SDL_DestroyRenderer(RENDER_STATE.renderer);
	if (RENDER_STATE.window)
		SDL_DestroyWindow(RENDER_STATE.window);
}

int render_present (void) {
	SDL_SetRenderTarget(RENDER_STATE.renderer, NULL); // Draw to the window

	SDL_SetRenderDrawColor(RENDER_STATE.renderer,
			64, 64, 64, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(RENDER_STATE.renderer);

	SDL_RenderGeometry(RENDER_STATE.renderer, RENDER_STATE.screen,
			SCREEN_LAYER_GEOMETRY, SCREEN_LAYER_NUM_VERTICES,
			SCREEN_LAYER_INDICES, SCREEN_LAYER_NUM_INDICES);

	SDL_RenderPresent(RENDER_STATE.renderer);
	// Return target to screen and flush
	SDL_SetRenderTarget(RENDER_STATE.renderer, RENDER_STATE.screen);
	SDL_SetRenderDrawColor(RENDER_STATE.renderer,
			0, 0, 0, SDL_ALPHA_TRANSPARENT);
	SDL_RenderClear(RENDER_STATE.renderer);
	return 0;
}

static SDL_FRect layer_rect (Uint32 layer) {
	SDL_FPoint upper_left = SCREEN_LAYER_GEOMETRY[4*layer + 3].tex_coord;
	float w = (float) RENDER_STATE.w;
	float h = (float) RENDER_STATE.h;
	return (SDL_FRect) {4*w*upper_left.x, 4*h*upper_left.y, w, h};
}

SDL_FRect render_get_layer (SDL_Renderer **ren_dst, SDL_Texture **tex_dst,
		Uint32 layer) {
	if (!(0 <= layer && layer < 16))
		return (SDL_FRect) {0};
	if (ren_dst)
		*ren_dst = RENDER_STATE.renderer;
	if (tex_dst)
		*tex_dst = RENDER_STATE.screen;
	return layer_rect(layer);
}

SDL_Renderer * render_set_layer (Uint32 layer) {
	if (!(0 <= layer && layer < 16)) // Not permissible
		return NULL;
	SDL_FRect rect = layer_rect(layer);
	SDL_Rect viewport = {
		.x = (int) rect.x, .y = (int) rect.y,
		.w = (int) rect.w, .h = (int) rect.h
	};
	if(!SDL_SetRenderViewport(RENDER_STATE.renderer, &viewport))
		log_error("Viewport change failed: %s", SDL_GetError());
	return RENDER_STATE.renderer;
}

// State and event handling

enum state_response window_state_resize (struct state *self,
		SDL_WindowEvent *event) {
	struct window_state_data *data = self->data;
	struct window_create_info *info = data->info;
	SDL_SetRenderTarget(RENDER_STATE.renderer, NULL);

	SDL_DestroyTexture(RENDER_STATE.screen);

	RENDER_STATE.screen = make_screen_layers(
			info->target_width, info->target_height,
			info->max_width, info->max_height,
			&(RENDER_STATE.w), &(RENDER_STATE.h));
	if (!RENDER_STATE.screen) {
		log_critical("Failed to create screen texture: %s",
				SDL_GetError());
		return STATE_FAILURE;
	}
	set_screen_layer_coords(
			(float) 4 * RENDER_STATE.w,
			(float) 4 * RENDER_STATE.h);
	SDL_SetTextureScaleMode(RENDER_STATE.screen, SDL_SCALEMODE_NEAREST);

	SDL_SetRenderTarget(RENDER_STATE.renderer, RENDER_STATE.screen);
	log_info("Resized window to logical size %d, %d",
			RENDER_STATE.w, RENDER_STATE.h);
	return STATE_CONTINUE;
}

static enum state_response window_state_init (struct state *self) {
	struct window_state_data *data = self->data;
	struct window_create_info *info = data->info;
	int res = render_init(&(RENDER_STATE.renderer), info);
	if (res)
		return STATE_FAILURE;
	return STATE_CONTINUE;
}

static double time_elapsed = 0;
static Uint64 second_counter = 0;
static Uint64 frame_counter = 0;
static double measured_frame_rate = 0;
static enum state_response window_state_iterate (struct state *self,
		enum state_response prev, double dt) {
	frame_counter++;
	time_elapsed += dt;
	if ((Uint64) time_elapsed > second_counter) {
		second_counter++;
		measured_frame_rate = (double) frame_counter;
		frame_counter = 0;
	}

	// Draw some debug text
	SDL_Renderer *renderer = render_set_layer(15);
	if (!renderer) {
		log_critical("Render layer out of range");
		return STATE_FAILURE;
	}
	Uint8 r, g, b, a;
	SDL_GetRenderDrawColor(renderer, &r, &g, &b, &a);
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
	SDL_RenderDebugTextFormat(renderer, 0, 0, "%.0f FPS",
			measured_frame_rate);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	int res = render_present();
	if (res)
		return STATE_FAILURE;
	return STATE_CONTINUE;
}

static enum state_response window_state_event (struct state *self,
		void *event) {
	Uint32 type = ((SDL_Event *) event)->type;
	switch (type) {
	// Window events related to resizing
	case SDL_EVENT_WINDOW_RESIZED:
	case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
	case SDL_EVENT_WINDOW_METAL_VIEW_RESIZED:
	case SDL_EVENT_WINDOW_ICCPROF_CHANGED:
	case SDL_EVENT_WINDOW_DISPLAY_CHANGED:
	case SDL_EVENT_WINDOW_DISPLAY_SCALE_CHANGED:
	case SDL_EVENT_WINDOW_SAFE_AREA_CHANGED:
	case SDL_EVENT_WINDOW_ENTER_FULLSCREEN:
	case SDL_EVENT_WINDOW_LEAVE_FULLSCREEN:
	case SDL_EVENT_WINDOW_HDR_STATE_CHANGED:
	case SDL_EVENT_WINDOW_SETTINGS_CHANGED:
		return window_state_resize(self, event);
	// Window events which can be ignored
	case SDL_EVENT_WINDOW_SHOWN:
	case SDL_EVENT_WINDOW_HIDDEN:
	case SDL_EVENT_WINDOW_EXPOSED:
	case SDL_EVENT_WINDOW_MOVED:
	case SDL_EVENT_WINDOW_MINIMIZED:
	case SDL_EVENT_WINDOW_MAXIMIZED:
	case SDL_EVENT_WINDOW_RESTORED:
	case SDL_EVENT_WINDOW_MOUSE_ENTER:
	case SDL_EVENT_WINDOW_MOUSE_LEAVE:
	case SDL_EVENT_WINDOW_FOCUS_GAINED:
	case SDL_EVENT_WINDOW_FOCUS_LOST:
	case SDL_EVENT_WINDOW_HIT_TEST:
	case SDL_EVENT_WINDOW_OCCLUDED:
		return STATE_CONTINUE;
	// Window events which should be passed on
	case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
	case SDL_EVENT_WINDOW_DESTROYED:
		return STATE_DEFER;
	// Renderer events (handling not implemented)
	case SDL_EVENT_RENDER_TARGETS_RESET:
	case SDL_EVENT_RENDER_DEVICE_RESET:
	case SDL_EVENT_RENDER_DEVICE_LOST:
		log_critical("Unhandled renderer event: %s", SDL_GetError());
		return STATE_FAILURE;
	default:
		return STATE_DEFER;
	}
}

static void window_state_delete (struct state *self) {
	render_dest();
}

struct window_state_data DATA = {
	.info = &WINDOW_DEFAULTS,
};

struct state window_state = {
	.name = "WINDOW_STATE",
	.data = (void *) &DATA,
	.init = window_state_init,
	.iterate = window_state_iterate,
	.event = window_state_event,
	.del = window_state_delete
};

struct state *WINDOW_STATE = &window_state;

