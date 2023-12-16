#pragma warning(disable:4996)

#include <SDL.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <errno.h>

#include "consts.h"
#include "model.h"
#include "window.h"

int main(int argc, char* argv[]) {

	Model* model = NULL;
	Vertex3 shiftVertex = { 0, 0, 0 };

	Window* mainWindow = NULL;
	Window* settingsWindow = NULL;
	EventList eventsRoot = NULL;

	uint32_t running = 1, timerMain = 0;
	float angleX = 0.0f, angleY = 0.0f, angleZ = 0.0f, zoomStep = 0.1f;

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		SDL_Log("ERROR: Unable to initialize SDL: %s", SDL_GetError());
		return EXIT_FAILURE;
	}

	mainWindow = window_init(WINDOW_WIDTH_MAIN, WINDOW_HEIGHT_MAIN, 1);
	if (mainWindow == NULL) return EXIT_FAILURE;

	model = model_load("bunny.stl");
	if (model == NULL) return EXIT_FAILURE;

	while (!window_quit(mainWindow, eventsRoot)) {
		eventsRoot = event_poll(eventsRoot);

		timerMain = SDL_GetTicks();
		angleX = angleY = angleZ = 0.0f;

		for (EventList eventsNode = eventsRoot; eventsNode != NULL; eventsNode = eventsNode->next) {
			switch (eventsNode->event.type) {
			case SDL_MOUSEMOTION:
				if (SDL_GetMouseState(NULL, NULL) == SDL_BUTTON_LMASK) {
					angleY = eventsNode->event.motion.xrel / MOUSE_SENSITIVITY;
					angleX = eventsNode->event.motion.yrel / MOUSE_SENSITIVITY;
					model_rotate(model, angleX, angleY, angleZ);
				}
				if (SDL_GetMouseState(NULL, NULL) == SDL_BUTTON_RMASK) {
					shiftVertex.x = -(eventsNode->event.motion.xrel >> 2);
					shiftVertex.y = eventsNode->event.motion.yrel >> 2;
					model_shift(model, shiftVertex);
				}
				break;
			case SDL_MOUSEWHEEL:
				model->sideLength += (eventsNode->event.wheel.preciseY > 0.0f) ? model->sideLength * zoomStep : -(model->sideLength * zoomStep);
				if (model->sideLength < 0.0f) model->sideLength = 0.0f;
				break;
			}
		}


		if (model_draw(mainWindow->window, mainWindow->renderer, model) < 0) {
			SDL_Log("ERROR: SDL_Draw: %s", SDL_GetError());
		}
		SDL_RenderPresent(mainWindow->renderer);

		//printf("FPS: %d\n", 1000 / (SDL_GetTicks() - timerMain));
	}

	SDL_DestroyRenderer(mainWindow->renderer);
	SDL_DestroyWindow(mainWindow->window);
	SDL_Quit();

	free(model->polygons);
	free(model);
	for (EventList current = eventsRoot, next = NULL; current != NULL; current = next) {
		next = current->next;
		free(current);
	}
	free(mainWindow);

	return EXIT_SUCCESS;
}