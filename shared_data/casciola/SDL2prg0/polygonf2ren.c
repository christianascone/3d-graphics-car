#include <stdio.h>
#include <math.h>
#include <time.h>
#include "../GCGraLib2/GCGraLib2.h"
#define DIM 100

typedef struct
{
	int vxmin;
	int vxmax;
	int vymin;
	int vymax;
}VIEW;

typedef struct
{
	float wxmin;
	float wxmax;
	float wymin;
	float wymax;
}WIN;

void press(void)
{
int ich;
char ch;

   printf("\nPress a character to quit  ");
   while ((ich=getchar()) != '\n' && ich != EOF);
       scanf("%c",&ch);
}

void wind_view(float px,float py,int *ix,int *iy,
        float scx,float scy, VIEW *view, WIN *win)
{
  *ix=0.5 + scx * (px - win->wxmin) + view->vxmin;
  *iy=0.5 + scy * (win->wymin - py) + view->vymax;
}

int main(void) 
{
SDL_Window *wind;
SDL_Renderer *ren;
VIEW view={0, 400, 0, 400};
WIN win={-1.0, 1.0, -1.0, 1.0};
int n,i,j;
int x[DIM],y[DIM]; 
float step,s,c,scx,scy;
float x0,y0,xi,yi;
clock_t start, endt;

  if(SDL_Init(SDL_INIT_VIDEO)<0)
  {
    fprintf(stderr,"Couldn't init video: %s\n",SDL_GetError());
    return(1);
  }

  wind= SDL_CreateWindow("Polygonf", 100, 100,view.vxmax,view.vymax, SDL_WINDOW_SHOWN);
  if(wind==NULL){
	fprintf(stderr,"SDL_CreateWindow Error: %s\n",SDL_GetError());
	SDL_Quit();
	return 1;
  }

  ren = SDL_CreateRenderer(wind, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (ren == NULL){
	SDL_DestroyWindow(wind);
	fprintf(stderr,"SDL_CreateRenderer Error: %s\n",SDL_GetError());
	SDL_Quit();
	return 1;
  }

  SDL_SetRenderDrawColor(ren, 50, 50, 50, 255);
  SDL_RenderClear(ren);
  SDL_RenderPresent(ren);

  scx=(view.vxmax-view.vxmin)/(win.wxmax-win.wxmin);
  scy=(view.vymax-view.vymin)/(win.wymax-win.wymin);
  printf("\nDisegno di un poligono regolare: \n");
  printf("Dai il numero di vertici: \n");
  scanf("%d",&n);
  step=6.28/n;
  s=sin(step);
  c=cos(step);
  x0=1.0;
  y0=0.0;
  wind_view(x0,y0,&x[0],&y[0],scx,scy,&view,&win);
  for (i=0; i<=n; i++)
  {
      xi=x0*c-y0*s;
      yi=x0*s+y0*c;
      wind_view(xi,yi,&x[i],&y[i],scx,scy,&view,&win);
      x0=xi;
      y0=yi;
  }

start=clock();
/* disegna poligono */
  SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
  SDL_RenderDrawLine(ren, x[n-1], y[n-1], x[0], y[0]);
  for (i=0; i<n-1; i++)
	SDL_RenderDrawLine(ren, x[i], y[i], x[i+1], y[i+1]);
  SDL_RenderPresent(ren);  
#ifdef DEBUG
/* disegno artistico: definire DEBUG nel Makefile*/
  SDL_SetRenderDrawColor(ren, 255, 255, 0, 255);
  for (i=0; i<n; i++)
    for (j=i+1; j<n; j++)
       SDL_RenderDrawLine(ren, x[i], y[i], x[j], y[j]);
  SDL_RenderPresent(ren);
#endif
endt=clock();
  printf("\n Tempo: %f \n", (double)(endt-start)/CLOCKS_PER_SEC);
  press();
  SDL_Quit();
  return(0);
}
