#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../GCGraLib2/GCGraLib2.h"

#define DIM 200
#define DEFAULT_PTSIZE  18

void my_draw_rect(SDL_Renderer *ren,int ax,int ay,int width,int height)
{
	SDL_Rect rects = {ax-width,ay-height,2*width,2*height};
	SDL_RenderDrawRect(ren, &rects);
}

/* funzione di determinazione opzione del menu' grafico di scelta */
int opt_menu(int menu[][2],int n,int ix,int iy, int wh2)
{
	int i,choice;

	choice=-1;
	i=0;
	while (choice==-1 && i<n)
	{
		if(abs(menu[i][0]-ix)<=wh2 && abs(menu[i][1]-iy)<=wh2)
			choice=i;
		i++;
	}
	return(choice);
}

int main(void) 
{
SDL_Window *win, *wind;
SDL_Renderer *ren, *rend;
SDL_Event event;
TTF_Font *font;

int vxmax,vymax;
int n,i,j,cx,cy,ix,iy;
int x[DIM],y[DIM]; 
float rx,ry,step,alfa,alfa0=0.0;
int menu[12][2],wh=50,wh2,choice,b,vx,vy;
char menu_text[12][8] = { "1","2","3","4","5","6","7","8","9","OK","0","END" };

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
	// TTF_SetFontStyle(font, TTF_STYLE_NORMAL);

	vxmax=400;
	vymax=400;
	vx=150;
	vy=100;
	wh2=wh/2 - 1;

	/* set menu coordinates: center and radius*/
	for (i=0; i<12; i++)
	{
		menu[i][0] = vx + (i%3)*wh;
		menu[i][1] = vy + (i/3)*wh;
	}

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

	for( i=0 ; i<12 ; i++ )
	{
          GC_DrawText( ren, font, 255, 255, 255, 0, 0, 0, 0, 0, menu_text[i], 
                       menu[i][0] - strlen( menu_text[i] )*6, menu[i][1]-5, blended);
	}

	SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
	for( i=0 ; i<12 ; i++ )
	{
		my_draw_rect( ren, menu[i][0], menu[i][1], wh2, wh2);
	}

	//Update the screen
	SDL_RenderPresent(ren);

	do
	{
		n=0;
		b=1;

		/* main loop for events */
		choice=0;
		do
		{
			if (SDL_PollEvent(&event) && event.type == SDL_MOUSEBUTTONDOWN && event.button.button==1 )
			{
				ix=event.button.x;
				iy=event.button.y;
				choice=opt_menu(menu,12,ix,iy,wh2);
				if( choice == 9 || choice == 11 )
					break;

				choice += 1;
				if( choice == 11 )
					choice = 0;

				n=n*b+choice;
				b = 10;
				//printf( "n = %d\n", n );
			}
		}while(1);

		if( choice == 11 )
			break;

		if( n!=0 && n<DIM )
		{
                // new window for drawing
                   wind= SDL_CreateWindow("Polygon", 100+vxmax, 100, vxmax, vymax, SDL_WINDOW_SHOWN);
                   if(wind==NULL){
        	      fprintf(stderr,"SDL_CreateWindow Error: %s\n",SDL_GetError());
	              SDL_Quit();
                      return 1;
                   }
                   rend = SDL_CreateRenderer(wind, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
                   if (rend == NULL){
                      SDL_DestroyWindow(wind);
                      fprintf(stderr,"SDL_CreateRenderer Error: %s\n",SDL_GetError());
                      SDL_Quit();
                      return 1;
                   }
                   SDL_SetRenderDrawColor(rend, 255, 50, 50, 255);
                   SDL_RenderClear(rend);
                   SDL_RenderPresent(rend);
	  	   cx=(int)vxmax/2;
		   cy=(int)vymax/2;
		   rx=(float)vxmax/2-2;
		   ry=(float)vymax/2-2;
		   step=6.28/n;
		   for (j=0; j<n; j++)
		   {
			alfa=j*step+alfa0;
			x[j]=(int)rx*cos(alfa)+cx;
			y[j]=(int)ry*sin(alfa)+cy;
		   }
		   alfa0 += step;
		   /* disegna un poligono */
		   SDL_SetRenderDrawColor(rend,255,255,255,255);
                   SDL_RenderDrawLine(rend,x[n-1], y[n-1], x[0], y[0]);
		   for (i=0; i<n-1; i++)
                   	SDL_RenderDrawLine(rend,x[i], y[i], x[i+1], y[i+1]);
		   SDL_RenderPresent(rend);
#ifdef DEBUG
		   /* disegno artistico: definire DEBUG nel Makefile */
		   for (i=0; i<n; i++)
			for (j=i+1; j<n; j++)
				SDL_RenderDrawLine(rend,x[i], y[i], x[j], y[j]);
                   SDL_RenderPresent(rend);
#endif
                   // mostra solo per 5 secondi
                   SDL_Delay(5000);
                   SDL_DestroyRenderer(rend);
	           SDL_DestroyWindow(wind);
		}
		else
			printf( "Non posso disegnare: n=%d , DIM=%d\n", n, DIM );

	}while(1);

	TTF_CloseFont(font);
	TTF_Quit();
	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	SDL_Quit();
	return(0);
}
