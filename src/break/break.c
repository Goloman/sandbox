#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define SCREEN_WIDTH  640
#define SCREEN_HEIGHT 480
#define TICK_INTERNAL 17
#define MOVE 1
#define ARENA_WIDTH 100
#define PADDLE_WIDTH 10

#define SDL_ERR() (fprintf(stderr, "%s:%d %s\n", __FILE__, __LINE__, SDL_GetError()), exit(EXIT_FAILURE))

// We use globals since there's not much happening and I'm a lazy fuck
static SDL_Window *win;
static SDL_Renderer *ren;
static SDL_bool quit;

static SDL_Rect playArea;

static SDL_Rect paddle;
static SDL_bool right;
static SDL_bool left;

static short position;

void init(void);
void loop(void);
void getInput(void);
void handleWindowEvent(SDL_Event *ev);
void updateState(void);
void render(void);
void clean(void);

int main() {
	init();
	loop();
	clean();
	return EXIT_SUCCESS;
}

void init() {
	if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO) != 0)
		SDL_ERR();
	win = SDL_CreateWindow("break",
	                       SDL_WINDOWPOS_CENTERED,
	                       SDL_WINDOWPOS_CENTERED,
	                       SCREEN_WIDTH,
	                       SCREEN_HEIGHT,
	                       SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	if(win == NULL) SDL_ERR();
	ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if(ren == NULL) SDL_ERR();
	quit = SDL_FALSE;
	right = SDL_FALSE;
	left = SDL_FALSE;
	position = 50;
	playArea.x = 0;
	playArea.y = 0;
	playArea.w = SCREEN_WIDTH;
	playArea.h = SCREEN_HEIGHT;
	paddle.h = 10;
	paddle.w = ((PADDLE_WIDTH * playArea.w) / (PADDLE_WIDTH + ARENA_WIDTH));
	paddle.y = playArea.h - (paddle.h * 3);
}

void loop() {
	Uint32 next = SDL_GetTicks() + TICK_INTERNAL;
	while (!quit) {
		getInput();
		updateState();
		render();
		Uint32 current = SDL_GetTicks();
		if(next > current)
			SDL_Delay(next - current);
		next += TICK_INTERNAL;
	}
}

void getInput() {
	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		switch (e.type) {
		case SDL_QUIT:
			quit = SDL_TRUE;
			break;
		case SDL_KEYDOWN:
			switch(e.key.keysym.sym) {
			case SDLK_LEFT:
				left = SDL_TRUE;
				break;
			case SDLK_RIGHT:
				right = SDL_TRUE;
				break;
			default:
				break;
			}
			break;
		case SDL_KEYUP:
			switch(e.key.keysym.sym) {
			case SDLK_LEFT:
				left = SDL_FALSE;
				break;
			case SDLK_RIGHT:
				right = SDL_FALSE;
				break;
			}
			break;
		case SDL_WINDOWEVENT:
			handleWindowEvent(&e);
			break;
		default:
			break;
		}
	}

}

void handleWindowEvent(SDL_Event *ev) {
	SDL_WindowEvent *e = (SDL_WindowEvent *)ev;
	switch (e->event) {
	case SDL_WINDOWEVENT_RESIZED:
		if (e->data1 * 3 > e->data2 * 4) {
			playArea.w = (e->data2 * 4) / 3;
			playArea.h = e->data2;
			playArea.x = (e->data1 - playArea.w) / 2;
			playArea.y = 0;
		} else {
			playArea.w = e->data1;
			playArea.h = (e->data1 * 3) / 4;
			playArea.x = 0;
			playArea.y = (e->data2 - playArea.h) / 2;
		}
		paddle.h = 10;
		paddle.w = ((PADDLE_WIDTH * playArea.w) / (PADDLE_WIDTH + ARENA_WIDTH));
		paddle.y = playArea.h + playArea.y - (paddle.h * 3);
		break;
	default:
		break;
	}
}

void updateState() {
	if (left) position -= MOVE;
	if (right) position += MOVE;
	if (position < 0) position = 0;
	if (position > ARENA_WIDTH - PADDLE_WIDTH) position = ARENA_WIDTH - PADDLE_WIDTH;
}

void render() {
	paddle.x = playArea.x + (position * playArea.w) / (ARENA_WIDTH - 1);
	SDL_SetRenderDrawColor(ren, 0x33, 0x33, 0x33, 0xff);
	SDL_RenderClear(ren);
	SDL_SetRenderDrawColor(ren, 0x00, 0x00, 0x00, 0xff);
	SDL_RenderFillRect(ren, &playArea);
	SDL_SetRenderDrawColor(ren, 0xff, 0xff, 0xff, 0xff);
	SDL_RenderFillRect(ren, &paddle);
	SDL_RenderPresent(ren);
}

void clean() {
	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	SDL_Quit();
}
