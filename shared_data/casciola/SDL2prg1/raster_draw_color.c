#include <stdio.h>
#include <math.h>
#include "../GCGraLib2/GCGraLib2.h"

#define DIM 100

int max(int a,int b,int c)
{
int m;
        m=a;
        if (b>m) m=b;
        if (c>m) m=c;
        return m;
}

int min(int a,int b,int c)
{
int m;
        m=a;
        if (b<m) m=b;
        if (c<m) m=c;
        return m;
}

float f01(float x0,float y0,float x1,float y1,float x,float y)
{
        return (y0-y1)*x+(x1-x0)*y+x0*y1-x1*y0;
}


/********************************************/
/*           Function draw_color                  */
/********************************************/
void draw_color(SDL_Renderer *ren,int *x,int *y, int *r, int *g, int *b)
{
int x0,y0,r0,g0,b0;
int x1,y1,r1,g1,b1;
int x2,y2,r2,g2,b2;
int xmin,xmax,ymin,ymax;
int ix,iy,ir,ig,ib;
//int ic;
float alpha, beta, gamma;

/*initialization */
x0=x[0];y0=y[0];
x1=x[1];y1=y[1];
x2=x[2];y2=y[2];
r0=r[0];g0=g[0];b0=b[0];
r1=r[1];g1=g[1];b1=b[1];
r2=r[2];g2=g[2];b2=b[2];
/* triangle extent */
   xmin=min(x0,x1,x2);
   xmax=max(x0,x1,x2);
   ymin=min(y0,y1,y2);
   ymax=max(y0,y1,y2);
/* triangle rasterization */
   for (ix=xmin; ix<=xmax; ix++)
     for (iy=ymin; iy<=ymax; iy++)
     {
/* barycentric coordinates */
             alpha=f01(x1,y1,x2,y2,ix,iy)/f01(x1,y1,x2,y2,x0,y0);
             beta=f01(x2,y2,x0,y0,ix,iy)/f01(x2,y2,x0,y0,x1,y1);
             gamma=f01(x0,y0,x1,y1,ix,iy)/f01(x0,y0,x1,y1,x2,y2);
             if (alpha>0 && beta>0 && gamma>0)
             {
                     ir=(int)(alpha*r0+beta*r1+gamma*r2);
                     ig=(int)(alpha*g0+beta*g1+gamma*g2);
                     ib=(int)(alpha*b0+beta*b1+gamma*b2);
//                     ic=ir*256*256+ig*256+ib;
                     SDL_SetRenderDrawColor(ren,ir,ig,ib,255);
                     SDL_RenderDrawPoint(ren, ix, iy);
             }
     }
}

int main(void) 
{
SDL_Window *win;
SDL_Renderer *ren;
SDL_Event event;
int vxmax=400,vymax=400;
int j,cx,cy;
int x[DIM],y[DIM]; 
int r[DIM],g[DIM],b[DIM]; 
float rx,ry,step,alfa;
int n=3;
int done=0;

  if(SDL_Init(SDL_INIT_VIDEO)<0)
  {
    fprintf(stderr,"Couldn't init video: %s\n",SDL_GetError());
    return(1);
  }

  win= SDL_CreateWindow("RasterDrawColor", 0, 0, vxmax, vymax, SDL_WINDOW_RESIZABLE);
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

/*colors setting*/
  r[0]=255; g[0]=0; b[0]=0;
  r[1]=0; g[1]=255; b[1]=0;
  r[2]=0; g[2]=0; b[2]=255;

/*center and radius*/
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
  }

//Set color
  SDL_SetRenderDrawColor(ren,0,0,0,255);
//Clear back buffer
  SDL_RenderClear(ren);
//Swap back and front buffer
  SDL_RenderPresent(ren);

/* rasterizza un triangolo */
  draw_color(ren,x,y,r,g,b);

#ifdef DEBUG
/* disegna bordo del triangolo */
int i;
  SDL_SetRenderDrawColor(ren,255,255,255,255);
  GC_DrawLine(ren, x[n-1], y[n-1], x[0], y[0]);
  for (i=0; i<n-1; i++)
    GC_DrawLine(ren, x[i], y[i], x[i+1], y[i+1]);
#endif

  SDL_RenderPresent(ren);

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
             default:
             break;
           }
        break;
        default:
        break;
      }
     }

  }

  SDL_DestroyRenderer(ren);
  SDL_DestroyWindow(win);
  SDL_Quit();
  return(0);
}
