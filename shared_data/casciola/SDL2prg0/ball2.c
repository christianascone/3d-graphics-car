#include <stdio.h>
#include <math.h>
#include "../GCGraLib2/GCGraLib2.h"

#define DIM 100
#define n 10
typedef int vettore[150];
typedef double vector[300];
typedef  
struct
{
	double xmin, xmax, ymin, ymax;
}RECT;

typedef RECT VIEWPORT;
typedef RECT WINDOW;

int sgn(int a)
{
 if (a<=0) return(-1);
 else return(+1);
}


//FUNZIONI DI DISEGNO DELLE SCHERMATE E DI LETTURA E DISEGNO DEI FILES
void get_scale (RECT r1, RECT r2, double *scx, double *scy)
{
	*scx = (r1.xmax - r1.xmin) / (r2.xmax - r2.xmin);
	*scy = (r1.ymax - r1.ymin) / (r2.ymax - r2.ymin);
}

void wind_view(double px, double py, int *ix, int *iy, VIEWPORT view, WINDOW win)
{
double scx, scy;
	get_scale (view, win, &scx, &scy);
	*ix = 0.5 + scx * (px - win.xmin) + view.xmin;
	*iy = 0.5 + scy * (win.ymin - py) + view.ymax;
}

void view_wind(double px, double py, double *ix, double *iy, VIEWPORT view, WINDOW win)
{
double scx, scy;
	get_scale (view, win, &scx, &scy);
	*ix = (px - view.xmin)/scx + win.xmin;
	*iy = (view.ymax - py)/scy + win.ymin;
}

int main(void) 
{
SDL_Window *win;
SDL_Renderer *ren;
SDL_Event event;
int done=0;
int vxmax,vymax;
float wxmax,wymax;
int cx,cy,cx0,cy0;
float rx,ry,rx0,ry0,m,m0,c;
float x,y,x0,y0;
float vx,vy,vx0,vy0,vxn,vyn,vy0n,vx0n;
VIEWPORT v;
WINDOW w;

  printf("***************************\n");
  printf("usa le frecce LEFT e RIGHT\n");
  printf("per cambiare velocita'\n");
  printf("ESC per quit\n");
  printf("***************************\n");
  if(SDL_Init(SDL_INIT_VIDEO)<0)
  {
    fprintf(stderr,"Couldn't init video: %s\n",SDL_GetError());
    return(1);
  }
v.xmin=0;
v.xmax=400;
v.ymin=0;
v.ymax=400;
w.xmin=0;
w.xmax=400;
w.ymin=0;
w.ymax=400;
vxmax=v.xmax-v.xmin;
vymax=v.ymax-v.ymin;
wxmax=w.xmax-w.xmin;
wymax=w.ymax-w.ymin;

  win= SDL_CreateWindow("Two Ball Game", 100, 100, vxmax, vymax, SDL_WINDOW_SHOWN);
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
  cx0=(int)vxmax/7;
  cy0=(int)vymax/7;
  rx0=(float)vxmax/10-2;
  ry0=(float)vymax/10-2;
  c=rx0/rx;
  m=1;
  m0=m*pow(rx0,2)/pow(rx,2); 
  SDL_SetRenderDrawColor(ren, 0, 0, 255, 255);
  SDL_RenderClear(ren);
  SDL_SetRenderDrawColor(ren, 255, 255, 0, 255);
/* disegna la crf. circoscritta al poligono */
  GC_FillCircle(ren, cx, cy, rx);
//  GC_DrawCircle(ren, cx, cy, rx);

  x=58;
  y=58;
  x0=48;
  y0=352;
  vx=5;
  vy=-5;
  vx0=8;
  vy0=8; 
  
  cx=rx+10;
  cy=ry+10;
  cx0=rx0+10;
  cy0=vymax-ry0-10;
  while(!done)
  {
    if(SDL_PollEvent(&event))
    {
      switch(event.type)
      {
        case SDL_KEYDOWN:
	   switch(event.key.keysym.sym)
	   {
             case SDLK_ESCAPE:
	      done = 1;
	     break;
             case SDLK_LEFT:
	      vx=vx/2+1;
	      vy=vy/2+1;
	      vx0=vx0/2+1;
	      vy0=vy0/2+1;
	     break;
             case SDLK_RIGHT:
	      vx=2*vx;
	      vy=2*vy;
	      vx0=2*vx0;
	      vy0=2*vy0;
	     break;
             case SDLK_UP:
              vx=0;
              vy=0;
	      vx0=0;
	      vy0=0;
	     break;
             case SDLK_DOWN:
              if(vx==0 && vy==0){
                vx=1;
                vy=1;
	        vx0=1;
	        vy0=1;
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
//servono per dare la direzione ad entrambe le palline
   if(x<=rx) 
	vx=-vx;
   if(x>=wxmax-rx)
	vx=-vx;
   if(y<=ry)
	vy=-vy;
   if(y>=wymax-ry)
	vy=-vy;
   if(x0<=rx0) 
	vx0=-vx0;
   if(x0>=wxmax-rx0)
	vx0=-vx0;
   if(y0<=ry0)
	vy0=-vy0;
   if(y0>=wymax-ry0)
	vy0=-vy0;
   if(vx==0)
	 vx=1;
   if(vy==0)
	 vy=1;
   if(vx0==0)
	 vx0=1;
   if(vy0==0)
	 vy0=1;
   if(sqrt(pow(x-x0,2)+pow(y-y0,2))<=(rx+rx0))
	{
	vxn=((m-m0)*vx+2*m0*vx0)/(m+m0);
	vyn=((m-m0)*vy+2*m0*vy0)/(m+m0);
	vx0n=((m0-m)*vx0+2*m*vx)/(m+m0);
	vy0n=((m0-m)*vy0+2*m*vy)/(m+m0);
	vx=vxn;
	vy=vyn;
	vx0=vx0n;
	vy0=vy0n;
	}
   x=x+vx;
   y=y+vy;    
   x0=x0+vx0;
   y0=y0+vy0;  
   wind_view(x,y,&cx,&cy,v,w);
   wind_view(x0,y0,&cx0,&cy0,v,w);

//ATTENZIONE
//ogni volta ridisegno TUTTA l'immagine, compreso lo sfondo, in modo tale che sembri
//che si muova così non devo cancellare il vecchio cerchio, ottenendo l'effetto 
//di accendi/spegni.

  SDL_SetRenderDrawColor(ren, 0, 0, 255, 255);
  SDL_RenderClear(ren);
  SDL_SetRenderDrawColor(ren, 255, 255, 0, 255);
  GC_FillCircle(ren, cx, cy, rx);
  SDL_SetRenderDrawColor(ren, 255, 0, 0, 255);
  GC_FillCircle(ren, cx0, cy0, rx);
  SDL_RenderPresent(ren);
  }

  SDL_DestroyRenderer(ren);
  SDL_DestroyWindow(win);

  SDL_Quit();
  return(0);
}
