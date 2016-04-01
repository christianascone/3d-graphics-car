#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../GCGraLib2/GCGraLib2.h"

#define DIM 200
#define DEFAULT_PTSIZE  22

int main(void) 
{
SDL_Window *win;
SDL_Renderer *ren;
//	SDL_Texture *tex;
TTF_Font *font;
SDL_Rect rect;
char a[3][5]={"prova"," prov","  pro"};

	int vxmax,vymax;
//	int n,i,j,cx,cy,color,ix,iy;
//	int x[DIM],y[DIM]; 
//	float rx,ry,step,alfa,alfa0=0.0;
	
	if(SDL_Init(SDL_INIT_VIDEO)<0)
	{
		fprintf(stderr,"Couldn't init video: %s\n",SDL_GetError());
		return(1);
	}

	/* Initialize the TTF library */
	if(TTF_Init() < 0)
	{ 
		fprintf(stderr, "Couldn't initialize TTF: %s\n",SDL_GetError());
		SDL_Quit();
		return(2);
	}

	font = TTF_OpenFont("FreeSans.ttf", DEFAULT_PTSIZE);
	if(font == NULL) 
		fprintf(stderr, "Couldn't load font\n");
	TTF_SetFontStyle(font, TTF_STYLE_NORMAL);

	vxmax=400;
	vymax=400;
	

	win= SDL_CreateWindow("Mouse_Polygon", 100, 100, vxmax, vymax, SDL_WINDOW_SHOWN);
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


        rect.x=100;
        rect.y=100;
        rect.w=vxmax/2;
        rect.h=vymax/2;
        SDL_RenderDrawRect(ren,&rect);
//	GC_DrawRect( ren, 100, 100, vxmax/2, vymax/2);
	GC_DrawText( ren, font, 255, 255, 255, 0, 0, 0, 0, 0, a[0], a[0][0] - strlen( a[0] )*6, a[0][1]-5, blended);
	//GC_DrawText( ren, font, 255, 255, 255, 0, 0, 0, 0, 0, a[0], 100, 92, shaded);

	SDL_RenderPresent(ren);
        SDL_Delay(1000);
	TTF_CloseFont(font);
	TTF_Quit();
	SDL_Quit();
	return(0);
} 
