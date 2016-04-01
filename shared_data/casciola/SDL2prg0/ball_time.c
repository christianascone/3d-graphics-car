#include <stdio.h>
#include <math.h>
#include "../GCGraLib2/GCGraLib2.h"

#define DIM 100
#define NUM_FRAMES 100
#define GC_VIDEOEXPOSE 999

Uint32 timelast=0;
Uint32 timelast_fps=0;
const Uint32 elapsed = 20;
int Frames=0;

int sgn(int a)
{
 if (a<=0) return(-1);
 else return(+1);
}

void draw_scene(SDL_Renderer *ren, int cx, int cy, float rx, int n, int x[], int y[])
{
SDL_SetRenderDrawColor(ren, 0, 125, 0, 255);
SDL_RenderFillRect(ren, NULL);
SDL_SetRenderDrawColor(ren, 255, 0, 0, 255);

#ifdef DEBUG
/* disegna il cerchio pieno */
   GC_FillCircle(ren, cx, cy, rx);
#else
/* disegna la circonferenza del cerchio */
   GC_DrawCircle(ren, cx, cy, rx);
/*
   SDL_RenderDrawLine(ren, cx+x[n-1], cy+y[n-1], cx+x[0], cy+y[0]);
   int i;
   for (i=0; i<n-1; i++)
     SDL_RenderDrawLine(ren, cx+x[i], cy+y[i], cx+x[i+1], cy+y[i+1]);
*/
#endif
SDL_RenderPresent(ren);
}

void myDrawScene(SDL_Renderer *ren, int *sx, int *sy, 
                 int *cx, int rx, int *cy, int ry, int vxmax, int vymax, 
                 int pxstep, int pystep, int n, int x[], int y[])
{
   if(*cx<=rx) *sx=1;
   if(*cx>=vxmax-rx) *sx=-1;
   if(*cy<=ry) *sy=1;
   if(*cy>=vymax-ry) *sy=-1;
   *cx+=*sx*pxstep;
   *cy+=*sy*pystep;

   draw_scene(ren, *cx, *cy, rx, n, x, y);

   Frames++;
}

void redraw()
{
SDL_Event e;

  e.type=GC_VIDEOEXPOSE;
  SDL_PushEvent(&e);
}

void myTimer()
{
Uint32 timeNow;
float seconds, fps;

  timeNow=SDL_GetTicks();
  if(timeNow >= timelast + elapsed)
  {
   redraw();
   if(Frames >NUM_FRAMES)
   {
     seconds = (timeNow - timelast_fps) / 1000.0;
     fps = Frames / seconds;
     printf("%d frames in %g seconds = %g FPS\n", Frames, seconds, fps);
     timelast_fps=timeNow;
     Frames=0;
   } 
   timelast=timeNow;
  }
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

  cx=rx+10;
  cy=ry+50;
  draw_scene(ren, cx, cy, rx, n, x, y);

  while(!done)
  {
    if(SDL_PollEvent(&event))
    {
      switch(event.type)
      {
        case GC_VIDEOEXPOSE:
              myDrawScene(ren,&sx,&sy,&cx,rx,&cy,ry,
                          vxmax,vymax,pxstep,pystep,n,x,y);
           break;
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
   else
     myTimer();
  }

  SDL_DestroyRenderer(ren);
  SDL_DestroyWindow(win);
  SDL_Quit();
  return(0);
}
