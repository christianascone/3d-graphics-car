#include <stdio.h>
#include <math.h>
#include "../GCGraLib2/GCGraLib2.h"

#define DIM 100
#define DEFAULT_PTSIZE  18

typedef struct RECT RECT;
struct RECT
{
	float xmin, xmax, ymin, ymax;
};

typedef RECT VIEWPORT;
typedef RECT WINDOW;

void get_scale (RECT r1, RECT r2, float *scx, float *scy)
{
	*scx = (r1.xmax - r1.xmin) / (r2.xmax - r2.xmin);
	*scy = (r1.ymax - r1.ymin) / (r2.ymax - r2.ymin);
}

void wind_view (float px, float py, float *ix, float *iy, VIEWPORT view, WINDOW win)
{
float scx, scy;
	get_scale (view, win, &scx, &scy);
	*ix = 0.5 + scx * (px - win.xmin) + view.xmin;
	*iy = 0.5 + scy * (win.ymin - py) + view.ymax;
}

void assi (SDL_Renderer* ren, VIEWPORT view, WINDOW win)
{
float ipx, ipy;
	wind_view (0, 0, &ipx, &ipy, view, win);
        SDL_RenderDrawLine(ren, ipx, view.ymin, ipx, view.ymax);
	SDL_RenderDrawLine(ren, view.xmin, ipy, view.xmax, ipy);
}

void draw_data (SDL_Renderer* ren, VIEWPORT view, WINDOW win)
{
	// curva parametrica definita da una funzione con parametro t che va da 0 a 1
float x1, x2, y1, y2;
float t, px, py;

	t = 0;
	// Equazione è y = 2t e x = 3t
	px = t*3;
	py = t*9;
	wind_view (px, py, &x1, &y1, view, win);

	for (t = 0; t <= 1; t+=0.1)
	{
		px = t*3;
		py = t*2;

		printf("px %f e py %f\n", px, py);

		wind_view (px, py, &x2, &y2, view, win);
		SDL_RenderDrawLine(ren, x1, y1, x2, y2);

		printf("x1 %f e x2 %f \n y1 %f e y2 %f \n", x1, x2, y1, y2);

		x1 = x2;
		y1 = y2;
	}

	//assi (ren, view, win);
}


int main (void)
{
SDL_Window *win;
SDL_Renderer *ren;
//SDL_Texture *tex;
SDL_Rect sub_v, v;
TTF_Font *font;
WINDOW fun_win;
VIEWPORT fun_view;
char ch;

	if (SDL_Init (SDL_INIT_VIDEO) < 0)
	{
		fprintf (stderr, "Couldn't init video: %s\n", SDL_GetError ());
		return (1);
	}

	/* Initialize the TTF library */
	if (TTF_Init () < 0)
	{
		fprintf (stderr, "Couldn't initialize TTF: %s\n", SDL_GetError ());
		SDL_Quit ();
		return (2);
	}

	font = TTF_OpenFont ("FreeSans.ttf", DEFAULT_PTSIZE);
	if (font == NULL)
	{
		fprintf (stderr, "Couldn't load font\n");
	}

	v.x = 0;
	v.y = 0;
	v.w = 780;
	v.h = 440;

	win= SDL_CreateWindow("Draw_data", v.x, v.y, v.w, v.h, SDL_WINDOW_SHOWN);
	if(win==NULL){
		fprintf(stderr,"SDL_CreateWindow Error: %s\n",SDL_GetError());
		SDL_Quit();
		return 1;
	}

	ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (ren == NULL){
		SDL_DestroyWindow(win);
		fprintf(stderr,"SDL_CreateRenderer Error: %s\n",SDL_GetError());
		SDL_Quit();
		return 1;
	}

	SDL_SetRenderDrawColor(ren, 50, 50, 50, 255);
	SDL_RenderClear(ren);
	SDL_RenderPresent(ren);


//SDL_SetRenderDrawColor(ren, 240, 240, 240, 255);
        SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
	SDL_RenderFillRect (ren, &v);
        SDL_SetRenderDrawColor(ren, 0, 0, 255, 255);
	GC_DrawText (ren, font, 255, 0, 0, 0, 255, 255, 255, 0, "Grafico Polilinea", 600, 50, shaded);
        SDL_SetRenderDrawColor(ren, 255, 0, 0, 255);

	sub_v.x = v.x + 10;
	sub_v.y = v.y + 10;
	sub_v.w = v.w - 210;
	sub_v.h = v.h - 60;

	fun_view.xmin = sub_v.x;
	fun_view.xmax = sub_v.x + sub_v.w - 1;
	fun_view.ymin = sub_v.y;
	fun_view.ymax = sub_v.y + sub_v.h - 1;

	//tex = SDL_CreateTextureFromSurface(ren, screen);
//	SDL_RenderClear(ren);
//	SDL_RenderCopy(ren, tex, NULL, NULL);
	SDL_RenderPresent(ren);

	do
	{

     SDL_SetRenderDrawColor(ren, 240, 240, 240, 255);
		SDL_RenderFillRect (ren, &sub_v);

                SDL_SetRenderDrawColor(ren, 0, 0, 255, 255);
		draw_data (ren, fun_view, fun_win);

		//tex = SDL_CreateTextureFromSurface(ren, screen);
		//SDL_RenderClear(ren);
		//SDL_RenderCopy(ren, tex, NULL, NULL);
		SDL_RenderPresent(ren);

		printf ("\n <e> EXIT   <f> NEW DATA  ");
		while ((ch = getchar ()) != '\n' && ch != EOF);
		scanf ("%c", &ch);
	}
	while ((ch != 'E') && (ch != 'e'));

	TTF_CloseFont (font);
	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	TTF_Quit ();
	SDL_Quit ();
	return (0);
}
