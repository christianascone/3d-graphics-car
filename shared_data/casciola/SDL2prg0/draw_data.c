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

void wind_view (float px, float py, int *ix, int *iy, VIEWPORT view, WINDOW win)
{
float scx, scy;
	get_scale (view, win, &scx, &scy);
	*ix = 0.5 + scx * (px - win.xmin) + view.xmin;
	*iy = 0.5 + scy * (win.ymin - py) + view.ymax;
}

void assi (SDL_Renderer* ren, VIEWPORT view, WINDOW win)
{
int ipx, ipy;
	wind_view (0, 0, &ipx, &ipy, view, win);
        SDL_RenderDrawLine(ren, ipx, view.ymin, ipx, view.ymax);
	SDL_RenderDrawLine(ren, view.xmin, ipy, view.xmax, ipy);
}

void draw_data (SDL_Renderer* ren, int n, VIEWPORT view, WINDOW win, float x[], float y[])
{
int i;
int x1, x2, y1, y2;
float px, py;

	px = x[1];
	py = y[1];
	wind_view (px, py, &x1, &y1, view, win);

	for (i = 2; i <= n; i++)
	{
		px = x[i];
		py = y[i];

		wind_view (px, py, &x2, &y2, view, win);
		SDL_RenderDrawLine(ren, x1, y1, x2, y2);

		x1 = x2;
		y1 = y2;
	}

	//assi (ren, view, win);
}

int leggi_data (int *n, WINDOW* rect, float x[], float y[])
{
FILE *f;
char myfile[50], str[256];
int i, ok;

	printf ("\nNome file : ");
	scanf ("%s", myfile);
	f = fopen (myfile, "r");
	if (f == NULL)
	{
		ok = 0;
		printf ("\n*** File non trovato ***");
	}
	else
	{
		ok = 1;
		rewind (f);
		fgets (str, 256, f);
		sscanf (str, " %d ", n);
		for (i = 1; i <= *n; i++)
		{
			fgets (str, 256, f);
			sscanf (str, "%e %e", &x[i],&y[i]);
		}
		fclose (f);
	}
	rect->xmin=rect->xmax=x[1];
	rect->ymin=rect->ymax=y[1];
	for (i = 2; i <= *n; i++)
	{
          if(x[i]<rect->xmin) rect->xmin=x[i];
          else if(x[i]>rect->xmax) rect->xmax=x[i];

          if(y[i]<rect->ymin) rect->ymin=y[i];
          else if(y[i]>rect->ymax) rect->ymax=y[i];
	}
	return (ok);
}

int main (void)
{
SDL_Window *win;
SDL_Renderer *ren;
//SDL_Texture *tex;
SDL_Rect sub_v, v;
TTF_Font *font;
int n, ok;
float x[DIM], y[DIM];
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

	font = TTF_OpenFont ("/usr/share/fonts/truetype/freefont/FreeSans.ttf", DEFAULT_PTSIZE);
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
		do
			ok = leggi_data (&n, &fun_win, x, y);
		while (ok == 0);

                SDL_SetRenderDrawColor(ren, 240, 240, 240, 255);
		SDL_RenderFillRect (ren, &sub_v);

                SDL_SetRenderDrawColor(ren, 0, 0, 255, 255);
		draw_data (ren, n, fun_view, fun_win, x, y);

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
