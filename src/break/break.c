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

static SDL_Rect paddle;
static SDL_bool right;
static SDL_bool left;

static short position;
static int win_w;
static int win_h;

void init(void);
void loop(void);
void getInput(void);
void handleWindowEvent(SDL_Event *e);
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
	SDL_GetWindowSize(win, &win_w, &win_h);
	paddle.h = 10;
	paddle.w = ((PADDLE_WIDTH * win_w) / (PADDLE_WIDTH + ARENA_WIDTH));
	paddle.y = win_h - (paddle.h * 3);
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
	while (SDL_PollEvent(&e))
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

void handleWindowEvent(SDL_Event * e) {
	SDL_GetWindowSize(win, &win_w, &win_h);
	paddle.h = 10;
	paddle.w = ((PADDLE_WIDTH * win_w) / (PADDLE_WIDTH + ARENA_WIDTH));
	paddle.y = win_h - (paddle.h * 3);
}

void updateState() {
	if (left) position -= MOVE;
	if (right) position += MOVE;
	if (position < 0) position = 0;
	if (position >= ARENA_WIDTH) position = ARENA_WIDTH;
}

void render() {

	paddle.x = (position * (win_w - paddle.w)) / ARENA_WIDTH;
	SDL_SetRenderDrawColor(ren, 0x00, 0x00, 0x00, 0xff);
	SDL_RenderClear(ren);
	SDL_SetRenderDrawColor(ren, 0xff, 0xff, 0xff, 0xff);
	SDL_RenderFillRect(ren, &paddle);
	SDL_RenderPresent(ren);
}

void clean() {
	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	SDL_Quit();
}
