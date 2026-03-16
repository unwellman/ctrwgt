#include <stdio.h>
#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

SDL_AppResult SDL_AppInit (void **app_state, int argc, char *argv[]) {
	printf("Initialized!\n");
	return SDL_APP_SUCCESS;
}

SDL_AppResult SDL_AppIterate (void *app_state) {
	printf("Hello world\n");
	// Quit immediately
	return SDL_APP_SUCCESS;
}

SDL_AppResult SDL_AppEvent (void *app_state, SDL_Event *event) {
	return SDL_APP_CONTINUE;
}

void SDL_AppQuit (void *app_state, SDL_AppResult result) {
	if (result == SDL_APP_SUCCESS)
		printf("Successfully executed\n");
	else if (result == SDL_APP_FAILURE)
		printf("Failed to execute\n");
	else
		printf("Something went wrong: %d\n", result);
}

