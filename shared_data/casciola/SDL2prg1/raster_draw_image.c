#include <stdio.h>
#include <math.h>
#include "../GCGraLib2/GCGraLib2.h"

#define DIM 100

void conv(unsigned long m, int b, int a[])
{
int p;

a[0]=a[1]=a[2]=0;
p=-1;
while (m != 0)
  {
    p++;
    a[p]= m % b;
    m= m / b;
  }
}

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
/*           Function draw_image                  */
/********************************************/
void draw_image(SDL_Renderer *ren,SDL_Surface *tux,int *x,int *y, int *vtx, int *vty)
{
int x0,y0;
int x1,y1;
int x2,y2;
int xmin,xmax,ymin,ymax;
int ix,iy;
//int ic;
float alpha, beta, gamma;
int xim0,yim0,xim1,yim1,xim2,yim2;
float fx,fy,aa,bb;
int i,j,pix[3],pix1[3],pix2[3],pix3[3],pix4[3];

/*triangolo immagine con cui texturare il triangolo schermo */
    xim0=vtx[0]; yim0=vty[0];
    xim1=vtx[1]; yim1=vty[1];
    xim2=vtx[2]; yim2=vty[2];
/*triangolo schermo */
    x0=x[0];y0=y[0];
    x1=x[1];y1=y[1];
    x2=x[2];y2=y[2];
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
                 fx=alpha*xim0+beta*xim1+gamma*xim2;
                 fy=alpha*yim0+beta*yim1+gamma*yim2;
                 j=(int)fx;
                 i=(int)fy;
                 aa=fx-j;
                 bb=fy-i;
/*bilinear interpolation */
                 conv(GC_GetPixelImage(tux,j,i),256,pix1);
                 conv(GC_GetPixelImage(tux,j+1,i),256,pix2);
                 conv(GC_GetPixelImage(tux,j,i+1),256,pix3);
                 conv(GC_GetPixelImage(tux,j+1,i+1),256,pix4);

                 pix[0]=(int)((1-aa)*(1-bb)*pix1[0]+aa*(1-bb)*pix2[0]+bb*(1-aa)*pix3[0]+aa*bb*pix4[0]);
                 pix[1]=(int)((1-aa)*(1-bb)*pix1[1]+aa*(1-bb)*pix2[1]+bb*(1-aa)*pix3[1]+aa*bb*pix4[1]);
                 pix[2]=(int)((1-aa)*(1-bb)*pix1[2]+aa*(1-bb)*pix2[2]+bb*(1-aa)*pix3[2]+aa*bb*pix4[2]);
//                 ic=pix[2]*256*256+pix[1]*256+pix[0];
/*nearest neighbour */
//                     conv(GC_GetPixelImage(tux,ix,iy),256,pix);
//                     ic=pix[2]*256*256+pix[1]*256+pix[0];
                 SDL_SetRenderDrawColor(ren,pix[2],pix[1],pix[0],255);
                 SDL_RenderDrawPoint(ren, ix, iy);
             }
     }
}

int main(void) 
{
SDL_Window *win;
SDL_Renderer *ren;
SDL_Surface *tux;
SDL_Event event;
int res = 0;
int vxmax=400,vymax=400;
int j,cx,cy;
int x[DIM],y[DIM]; 
int vtx[DIM],vty[DIM]; 
float rx,ry,step,alfa;
int n=3;
int done=0;

/* load an image by file */
  tux = GC_LoadImage("ganesha.ppm",&res);
  if (res == 0)
    return 1;

  if(SDL_Init(SDL_INIT_VIDEO)<0)
  {
    fprintf(stderr,"Couldn't init video: %s\n",SDL_GetError());
    return(1);
  }

  win= SDL_CreateWindow("RasterDrawImage", 0, 0, vxmax, vymax, SDL_WINDOW_RESIZABLE);
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

/*texture coordinates initialization*/
  vtx[0]=tux->w; vty[0]=0;
  vtx[1]=0; vty[1]=tux->h;
  vtx[2]=0; vty[2]=0;

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

/* rasterizza un poligono con una immagine */
  draw_image(ren,tux,x,y,vtx,vty);

#ifdef DEBUG
/* disegna bordo di colore bianco */
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
