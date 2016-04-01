#include <stdio.h>
#include <math.h>
#include "../GCGraLib2/GCGraLib2.h"

#define DIM 100

void press(void)
{
int ich;
char ch;

   printf("\nPress a character to quit  ");
   while ((ich=getchar()) != '\n' && ich != EOF);
       scanf("%c",&ch);
}

int main(void) 
{
SDL_Window *win;
SDL_Renderer *ren;

int vxmax,vymax;
int n,i,j,cx,cy;
int x[DIM],y[DIM]; 
SDL_Point p[DIM];
float rx,ry,step,alfa;

  if(SDL_Init(SDL_INIT_VIDEO)<0)
  {
    fprintf(stderr,"Couldn't init video: %s\n",SDL_GetError());
    return(1);
  }

  vxmax=400;
  vymax=400;

  win= SDL_CreateWindow("Polygon", 100, 100, vxmax, vymax, SDL_WINDOW_SHOWN);
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

  printf("\nDisegno di un poligono regolare: \n");
  printf("Dai il numero di vertici: \n");
  scanf("%d",&n);
  cx=(int)vxmax/2;
  cy=(int)vymax/2;
  rx=(float)vxmax/2-2;
  ry=(float)vymax/2-2;
  step=6.28/n;
  for (j=0; j<n; j++)
  {
	  alfa=j*step;
	  x[j]=(int)rx*cos(alfa)+cx;
	  y[j]=(int)ry*sin(alfa)+cy;
          p[j].x=x[j];
          p[j].y=y[j];
  }
  p[n].x=x[0];
  p[n].y=y[0];


#ifdef DEBUG
/* disegna la crf. circoscritta al poligono */
 GC_FillCircle(ren, cx, cy, rx);
 GC_DrawCircle(ren, cx, cy, rx);
#endif

/* disegna un poligono di colore bianco */  
  SDL_SetRenderDrawColor(ren,255,255,255,255);
  SDL_RenderDrawLine(ren, x[n-1], y[n-1], x[0], y[0]);
  for (i=0; i<n-1; i++)
    SDL_RenderDrawLine(ren, x[i], y[i], x[i+1], y[i+1]);
  SDL_SetRenderDrawColor(ren,255,0,0,255);
  SDL_RenderDrawLines(ren, p, n+1);
  SDL_RenderPresent(ren);

#ifdef DEBUG
/* disegno artistico: definire DEBUG nel Makefile */
  SDL_SetRenderDrawColor(ren,255,0,0,255);
  for (i=0; i<n; i++)
    for (j=i+1; j<n; j++)
      SDL_RenderDrawLine(ren, x[i], y[i], x[j], y[j]);
  SDL_RenderPresent(ren);
#endif

press();
SDL_DestroyRenderer(ren);
SDL_DestroyWindow(win);

  
  SDL_Quit();
  return(0);
} 
