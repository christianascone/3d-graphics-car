#include <stdio.h>
#include <math.h>
#include "../GCGraLib2/GCGraLib2.h"

#define DIM 100

int sgn(int a)
{
 if (a<=0) return(-1);
 else return(+1);
}

void draw_scene(SDL_Renderer *ren, int cx, int cy, float rx, int n, int x[], int y[])
{
#ifdef DEBUG
/* disegna il cerchio pieno */
   GC_FillCircle(ren, cx, cy, rx);
#else
/* disegna la circonferenza anziche' il poligono */
/*   GC_DrawCircle(ren, cx, cy, rx);*/
   SDL_RenderDrawLine(ren, cx+x[n-1], cy+y[n-1], cx+x[0], cy+y[0]);
   int i;
   for (i=0; i<n-1; i++)
     SDL_RenderDrawLine(ren, cx+x[i], cy+y[i], cx+x[i+1], cy+y[i+1]);
#endif
}

int main(void) 
{
SDL_Window *win;
SDL_Renderer *ren;
SDL_Event event;
int done=0;
int vxmax=400,vymax=400;
int n=10;
int j,cx,cy;
int x[DIM],y[DIM]; 
float rx,ry,step,alfa;
int sx=1,sy=1,pxstep=2,pystep=2;

  printf("***************************\n");
  printf("usa le frecce LEFT e RIGHT\n");
  printf("per cambiare velocita'\n");
  printf("lettera q per quit\n");
  printf("***************************\n");
  if(SDL_Init(SDL_INIT_VIDEO)<0)
  {
    fprintf(stderr,"Couldn't init video: %s\n",SDL_GetError());
    return(1);
  }

win= SDL_CreateWindow("Ball Game", 100, 100, 400, 400, SDL_WINDOW_SHOWN);
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

  cx=(int)vxmax/8;
  cy=(int)vymax/8;
  rx=(float)vxmax/8-2;
  ry=(float)vymax/8-2;
  step=6.28/n;
  for (j=0; j<n; j++)
  {
	  alfa=j*step;
	  x[j]=(int)rx*cos(alfa);
	  y[j]=(int)ry*sin(alfa);
  }

/* disegna un poligono di colore rosso*/
//  SDL_SetRenderDrawColor(ren, 0, 125, 0, 255);
//  SDL_RenderClear(ren);
  SDL_SetRenderDrawColor(ren, 255, 0, 0, 255);
  cx=rx+10;
  cy=ry+50;
  draw_scene(ren, cx, cy, rx, n, x, y);

  while(!done)
  {
    if(SDL_PollEvent(&event))
    {
      switch(event.type)
      {
        case SDL_KEYDOWN:
	   switch(event.key.keysym.sym)
	   {
             case SDLK_q:
	      done = 1;
	     break;
             case SDLK_LEFT:
	      pxstep=pxstep/2+1;
	      pystep=pystep/2+1;
	     break;
             case SDLK_RIGHT:
	      pxstep=2*pxstep;
	      pystep=2*pystep;
	     break;
             case SDLK_UP:
              pxstep=0;
              pystep=0;
	     break;
             case SDLK_DOWN:
              if(pxstep==0 && pystep==0){
                pxstep=1;
                pystep=1;
              }
	     break;
	     default:
	     break;
	   }
	   break;
        default:
	     break;
      }
   }
   if(cx<=rx) sx=1;
   if(cx>=vxmax-rx) sx=-1;
   if(cy<=ry) sy=1;
   if(cy>=vymax-ry) sy=-1;
   cx+=sx*pxstep;
   cy+=sy*pystep;


   SDL_SetRenderDrawColor(ren, 0, 125, 0, 255);
   SDL_RenderFillRect(ren, NULL);
   SDL_SetRenderDrawColor(ren, 255, 0, 0, 255);
   draw_scene(ren, cx, cy, rx, n, x, y);

  SDL_RenderPresent(ren);
  }

  SDL_DestroyRenderer(ren);
  SDL_DestroyWindow(win);

  SDL_Quit();
  return(0);
}
