#include "window.h"

EventList event_poll(EventList root) {
	// free the given list
	for (EventList current = root, next = NULL; current != NULL; current = next) {
		next = current->next;
		free(current);
	}

	EventList current = NULL, prev = NULL;
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		if (prev != NULL) prev->next = current;
		current = malloc(sizeof(struct Event));
		if (current == NULL) {
			SDL_Log("Error: malloc() Event: %s", SDL_GetError());
			return NULL;
		}
		current->event = event;
		prev = current;
	}
	if (current != NULL) current->next = NULL;

	return current;
}

Window* window_init(const uint32_t windowWidth, const uint32_t windowHeight, char* title, uint8_t isMain) {

	Window* newWindow = malloc(sizeof(Window));
	if (newWindow == NULL) {
		SDL_Log("Error: malloc() Window: %s", SDL_GetError());
		return NULL;
	}
	
	newWindow->window = SDL_CreateWindow("STL Viewer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	if (newWindow->window == NULL) {
		SDL_Log("ERROR: Unable to create mainWindow: %s", SDL_GetError());
		return NULL;
	}

	newWindow->renderer = SDL_CreateRenderer(newWindow->window, -1, SDL_RENDERER_ACCELERATED);
	if (newWindow->renderer == NULL) {
		SDL_Log("ERROR: Unable to create renderer: %s", SDL_GetError());
		return NULL;
	}

	if (SDL_SetRenderDrawBlendMode(newWindow->renderer, SDL_BLENDMODE_NONE) < 0) {
		SDL_Log("ERROR: Set Blend Mode: %s", SDL_GetError());
		return NULL;
	}

	if (SDL_RenderSetVSync(newWindow->renderer, 1) < 0) {
		SDL_Log("ERROR: Set VSync: %s", SDL_GetError());
		return NULL;
	}

	newWindow->mainWindow = (isMain) ? 1 : 0;
	newWindow->windowID = SDL_GetWindowID(newWindow->window);
	if (newWindow->windowID == 0) {
		SDL_Log("ERROR: Set windowID: %s", SDL_GetError());
		return NULL;
	}

	return newWindow;
}

uint8_t window_quit(Window* window, EventList node) {
	if (window->mainWindow) {
		for (; node != NULL; node = node->next) {
			if (node->event.type == SDL_WINDOWEVENT) {
				if (node->event.window.windowID == window->windowID && node->event.window.event == SDL_WINDOWEVENT_CLOSE) return 1;
			}
			// if only one window is open, there is no SDL_WINDOWEVENT_CLOSE, but a SDL_QUIT event
			if (node->event.type == SDL_QUIT) return 1;
		}
	}
	return 0;
}