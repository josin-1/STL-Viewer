#ifndef WindowH
#define WindowH

#include <SDL.h>
#include <stdio.h>

struct Event {
	SDL_Event event;
	struct Event* next;
};
typedef struct Event* EventList;


EventList event_poll(EventList);

typedef struct {
	char title[50];
	int32_t width;
	int32_t height;
	int32_t posX;
	int32_t posY;
	uint8_t mainWindow;

	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Color backgroundColor;
	uint32_t windowID;


} Window;

Window* window_init(const uint32_t, const uint32_t, uint8_t);
uint8_t window_quit(Window*, EventList);



#endif // !WindowH