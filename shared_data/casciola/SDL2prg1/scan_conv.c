#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../GCGraLib2/GCGraLib2.h"

#define DIM 100

float           S1, S2;         /* fattori di scala */
unsigned int    sidex,sidey;    /* dimensioni rettangolo pixel */
int             polx[DIM],poly[DIM];
typedef float   rvector[10];
rvector         yymax , yymin , xa , dx;
int             edges , startedge , endedge , scandec;
float           scan;
int             nv;

/*                                                                    */
/*                          POLYINSERT                                */
/*                                                                    */
/*  Questa procedura ordina i lati del poligono e memorizza le infor- */
/*  mazioni del nuovo lato;e" un ordinamento ad inserzione.           */
/*  DATI IN INGRESSO                                                  */
/*    x21 , y21 , x22 , y22 : coordinate degli estremi del lato.      */
/*                                                                    */
void polyinsert(int edges,float x21,float y21,float x22,float y22)
{
int j1;
float   ym;

   j1=edges;
   if (y21 > y22)
	   ym=y21;
   else
	   ym=y22;
   while ((j1 != 1) && (yymax[j1-1] <ym))
   {
         yymax[j1]=yymax[j1-1];
         yymin[j1]=yymin[j1-1];
         dx[j1]=dx[j1-1];
         xa[j1]=xa[j1-1];
         j1=j1-1;
   }
   yymax[j1]=ym;
   dx[j1]=(x22-x21)/(y22-y21);
   if (y21 > y22)
   {
         yymin[j1]=y22;
         xa[j1]=x21;
   }
   else
   {
         yymin[j1]=y21;
         xa[j1]=x22;
   }
}/* polyinsert */

/*                                                                    */
/*                             LOADPOL                                */
/*                                                                    */
/*  Questa procedura ordina i lati del poligono secondo il valore del-*/
/*  le ordinate piu" grande.Inoltre memorizza e recupera delle infor- */
/*  mazioni relative ai lati eccetto quelli orrizontali.              */
/*                                                                    */
void loadpol(int n)
{
float x1 , x2 , y1 , y2;
int   k;

   x1=polx[n];
   y1=poly[n];
   edges=0;
   for (k=1; k<=n; k++)
    {
         x2=polx[k];
         y2=poly[k];
         if (y1==y2)
	    x1=x2;
         else
         {
               edges=edges+1;
               polyinsert(edges,x1,y1,x2,y2);
               x1=x2;
               y1=y2;
         }
    }
}/* loadpol */

/*                                                                    */
/*                            INCLUDE                                 */
/*                                                                    */
/*  Questa procedura aggiunge nuovi lati al gruppo che si sta conside-*/
/*  rando.                                                            */
/*                                                                    */
void includ()
{
   while ((endedge <= edges) && (yymax[endedge] >= scan))
   {
         dx[endedge]=dx[endedge]*(-scandec);
         endedge=endedge+1;
   }
}/* includ */
void xchange(float *a, float *b)
{
float t;

   t=*a;
   *a=*b;
   *b=t;
} /* xchange */

/*                                                                    */
/*                           XSORT                                    */
/*                                                                    */
/*  Questa procedura mette un elemento nella posizione corretta usando*/
/*  un algoritmo a bolla.                                             */
/*                                                                    */
void xsort(int kk)
{
int l;

   l=kk;
   while ((l > startedge) && (xa[l] < xa[l-1]))
    {
         xchange(&yymin[l],&yymin[l-1]);
         xchange(&xa[l],&xa[l-1]);
         xchange(&dx[l],&dx[l-1]);
         l=l-1;
    }
}/* xsort */

/*                                                                    */
/*                        UPDATEXVALUES                               */
/*                                                                    */
/*  Questa procedura rimuove i lati che non devono essere piu" consi- */
/*  derati;inoltre determina i punti di intersezione dei restanti lati*/
/*  con la scanline.                                                  */
/*                                                                    */
void updatexvalues()
{
int stopedge , beginedge , k , i;

   stopedge=endedge-1;
   beginedge=startedge;
   for (k=beginedge; k<=stopedge; k++)
    {
      if (yymin[k] < scan)
       {
            xa[k]=xa[k]+dx[k];
            xsort(k);
       }
      else
       {
            startedge=startedge+1;
            if (startedge <= k)
               for (i=k; i>=startedge; i--)
                {
                     yymin[i]=yymin[i-1];
                     xa[i]=xa[i-1];
                     dx[i]=dx[i-1];
                }
       }
   }
}/* updatexvalue */

/*                                                                    */
/*                               FILLSCAN                             */
/*                                                                    */
/*  Questa procedura disegna i segmenti individuati dalle intersezioni*/
/*  trovate con la procedura updatexvalues.                           */
/*                                                                    */
void fillscan(SDL_Renderer *ren)
{
int nx , j , k , y , x1 , x2;

   nx=(int)((endedge-startedge) / 2.0);
   j=startedge;
   y=(int)scan;
   for (k=1; k<=nx; k++)
    {
       if (xa[j+1]-xa[j] >= 0.5)
        {
         x1=(int)(xa[j]+0.5);
         x2=(int)(xa[j+1]+0.5);
         SDL_RenderDrawLine(ren, x1,y,x2,y);
        }
       j=j+2;
    }
}/* fillscan */

/*                             SCANCONV                               */
/*                                                                    */
/*  Questa procedura fa lo scan conversion di un poligono.            */
/*  DATI IN INGRESSO                                                  */
/*    n     : numero dei vertici del poligono.                        */
/*    polx  : vettori contenenti le ascisse e le ordinate dei vertici */
/*    poly    del poligono.                                           */
/*                                                                    */
void scanconv(SDL_Renderer *ren, int n)
{
int   i;

   scandec=1;
   loadpol(n);
   if (edges > 1)
   {
         scan=yymax[1]-0.5;
         startedge=1;
         endedge=1;
         includ();
         updatexvalues();
         while (endedge != startedge)
         {
               fillscan(ren);
               scan=scan-scandec;
               includ();
               updatexvalues();
         }
   }
   for (i=1; i<n; i++) 
   {
       SDL_RenderDrawLine(ren, polx[i],poly[i],polx[i+1],poly[i+1]);
   }
   SDL_RenderDrawLine(ren, polx[n],poly[n],polx[1],poly[1]);
}/* scan_conv */

/*funzione di redraw*/
void redraw(int n, int x[], int y[],SDL_Renderer *ren)
{
int i;

 GC_FillCircle(ren,x[0],y[0],3);
 for (i=1; i<=n; i++)
 {
   SDL_RenderDrawLine(ren, x[i-1], y[i-1], x[i], y[i]);
   GC_FillCircle(ren,x[i],y[i],3);
 }
}

/*semplice programma di prova di input da mouse e tastiera*/
int main(void) 
{
SDL_Window *win;
SDL_Renderer *ren;
SDL_Event event;
int vxmax=500,vymax=500;
int esc=1,i,n=0;
int x[DIM],y[DIM]; 
int flag=1;

  if(SDL_Init(SDL_INIT_VIDEO)<0)
  {
    fprintf(stderr,"Couldn't init video: %s\n",SDL_GetError());
    return(1);
  }

  win= SDL_CreateWindow("ScanConv", 0, 0, vxmax, vymax, SDL_WINDOW_RESIZABLE);
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

  printf("\nDisegno di una poligonale interattivamente: \n");
  printf("Dare i punti sulla finestra grafica con il mouse (button sinistro) \n");
  printf("Finire l'inserimento con il button destro \n");
  printf("Uscire dal programma con il tasto <ESC>  \n");
//color
//SDL_SetRenderDrawColor(ren,255,255,255,255);
//  bcolor
//SDL_SetRenderDrawColor(ren,0,0,0,255);

//Set color
  SDL_SetRenderDrawColor(ren,0,0,0,255);
//Clear back buffer
  SDL_RenderClear(ren);
//Swap back and front buffer
  SDL_RenderPresent(ren);

  SDL_SetRenderDrawColor(ren,255,255,255,255);
  while(esc)
  {
    if (SDL_PollEvent(&event))
      switch(event.type)
      {
       case SDL_MOUSEBUTTONDOWN:
        if(event.button.button==1)
        {
          if (flag==1)
           {
             SDL_SetRenderDrawColor(ren,0,0,0,255);
             SDL_RenderFillRect(ren,NULL);
             SDL_SetRenderDrawColor(ren,255,255,255,255);
             SDL_RenderPresent(ren);
             flag=0;
             n=0;
           }
          x[n]=event.button.x;
          y[n]=event.button.y;
	  GC_FillCircle(ren,x[n],y[n],3);
          if(n>0) 
            SDL_RenderDrawLine(ren, x[n-1], y[n-1], x[n], y[n]);
          n++;
        }
        if(event.button.button==3)
        {
          flag=1;
	  SDL_RenderDrawLine(ren, x[n-1], y[n-1], x[0], y[0]);
          nv=n;
          for (i=0; i<nv; i++)
          {
            polx[i]=x[i];
            poly[i]=y[i];
          }
          polx[nv]=x[0];
          poly[nv]=y[0];
          scanconv(ren,nv);
	}
        SDL_RenderPresent(ren);
        break;
       case SDL_KEYDOWN:
	 if(event.key.keysym.sym == SDLK_ESCAPE)
	   esc=0;
        break;

       case SDL_WINDOWEVENT:
          switch (event.window.event) {
            case SDL_WINDOWEVENT_RESIZED:
              vxmax = event.window.data1;
              vymax = event.window.data2;
              redraw(n-1,x,y,ren);
              SDL_RenderPresent(ren);
          }
       break;
      }
  }

  SDL_DestroyRenderer(ren);
  SDL_DestroyWindow(win);
  SDL_Quit();
  return(0);
}
