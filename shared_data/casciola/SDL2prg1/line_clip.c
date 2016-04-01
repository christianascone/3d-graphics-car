#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../GCGraLib2/GCGraLib2.h"

#define DIM 100

typedef int intvec[100];
intvec x,y;
float dt,c,s,xp,yp,t,fx1,fy1,fx2,fy2;
int i,j,cx,cy;
int xw[2],yw[2],count;
int esci;

/* Dichiarazioni per le funzioni di clipping */
int accept,done;
float wxmin,wymin,wxmax,wymax;
typedef int arroutcode[4];
arroutcode outcode1,outcode2;

void outcodes(float x,float y,arroutcode outcode)
{
    outcode[0] = y > wymax;
    outcode[1] = y < wymin;
    outcode[2] = x > wxmax;
    outcode[3] = x < wxmin;
}

int reject_check(arroutcode outcode1,arroutcode outcode2)
{
int i,reject;

  i = 0;
  do {
    reject = outcode1[i] && outcode2[i];
    i++;
  }while ((i < 4) && !reject);
return(reject);
}

int accept_check(arroutcode outcode1,arroutcode outcode2)
{
int i,accept;

    i= 0;
    do {
      accept = !(outcode1[i] || outcode2[i]);
      i++;
    }while ((i < 4) && accept);
return(accept);
}

void swap(void)
{
int   i,tem;
float temp;

    temp = fx1;
    fx1 = fx2;
    fx2 = temp;
    temp = fy1;
    fy1 = fy2;
    fy2 = temp;
    for (i=0; i<=3; i++)
    { 
        tem = outcode1[i];
        outcode1[i] = outcode2[i];
        outcode2[i] = tem;
    }
}

void clip_line(float *fx1,float *fy1,float *fx2,float *fy2,float wxmin,float wymin,
	float wxmax,float wymax,int *accept)
{
int done,reject;
   
   *accept=0;
   done = 1;
   outcodes(*fx1,*fy1,outcode1);
//   printf("code1 %d %d %d %d \n",outcode1[0],outcode1[1],outcode1[2],outcode1[3]);
   outcodes(*fx2,*fy2,outcode2);
//   printf("code2 %d %d %d %d \n",outcode2[0],outcode2[1],outcode2[2],outcode2[3]);
   do {
     reject = reject_check(outcode1,outcode2);
     if (reject)
         done = 0;
     else
     {
         *accept = accept_check(outcode1,outcode2);
         if (*accept)
	     done = 0;
         else
	 {
             if (!(outcode1[0] || outcode1[1] || outcode1[2] || outcode1[3]))
		 swap();
             if (outcode1[0])
	     {
                 *fx1 = *fx1 + (*fx2 - *fx1) * (wymax - *fy1)/(*fy2 - *fy1);
                 *fy1 = wymax;
	     }
             else
               if (outcode1[1])
               {
                   *fx1 = *fx1 + (*fx2 - *fx1) * (wymin-*fy1)/(*fy2 - *fy1);
                   *fy1 = wymin;
	       }
               else
                 if (outcode1[2])
		 {
                     *fy1 = *fy1 + (*fy2 - *fy1) * (wxmax - *fx1)/(*fx2 - *fx1);
                     *fx1 = wxmax;
		 }
                 else
		 {
                     *fy1 = *fy1 + (*fy2 - *fy1) * (wxmin-*fx1)/(*fx2 - *fx1);
                     *fx1 = wxmin;
		 }
                 outcodes(*fx1,*fy1,outcode1);
	 }
      }
    }while (done);
}

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
int vxmax,vymax;
int esc=1,i,j;
int x[DIM],y[DIM]; 
int n=11;

  if(SDL_Init(SDL_INIT_VIDEO)<0)
  {
    fprintf(stderr,"Couldn't init video: %s\n",SDL_GetError());
    return(1);
  }

  vxmax=500;
  vymax=500;

  win= SDL_CreateWindow("LineClip", 0, 0, vxmax, vymax, SDL_WINDOW_RESIZABLE);
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
  printf("Dare due punti sulla finestra grafica con il mouse (button sinistro) \n");
  printf("Uscire dal programma con il tasto <ESC>  \n");
//  color
//SDL_SetRenderDrawColor(ren,255,255,255,255);
//  bcolor
//SDL_SetRenderDrawColor(ren,0,0,0,255);

//Set color
  SDL_SetRenderDrawColor(ren,0,0,0,255);
//Clear back buffer
  SDL_RenderClear(ren);
//Swap back and front buffer
  SDL_RenderPresent(ren);

/*initialization*/
  dt=6.28/n;
  c=cos(dt);
  s=sin(dt);

  wxmin=0;
  wymin=0;
  wxmax=vxmax;
  wymax=vymax;

/*polygon initialization*/
  xp=0.0;
  yp=wxmax / 3;
  cx=wxmax / 2;
  cy=wymax / 2;
  x[0]=(int)(xp+cx +0.5);
  y[0]=(int)(cy-yp +0.5);
//  printf("stampa %d %d \n",x[0],y[0]);
  for (i=1; i<n; i++)
  {
    t=xp*c-yp*s;
    yp=yp*c+xp*s;
    xp=t;
    x[i]=(int)(xp+cx +0.5);
    y[i]=(int)(cy-yp +0.5);
//    printf("stampa %d %d \n",x[i],y[i]);
  }

/* disegno artistico a linee */
   SDL_SetRenderDrawColor(ren,255,255,255,255);
   for (i=0; i<n; i++)
     for (j=i+1; j<n; j++)
        SDL_RenderDrawLine(ren, x[i], y[i], x[j], y[j]);
   SDL_RenderPresent(ren);

   while(esc)
   {
    if (SDL_PollEvent(&event))
      switch(event.type)
      {
       case SDL_MOUSEBUTTONDOWN:
        if(event.button.button==1)
        {
          xw[count]=event.button.x;
          yw[count]=event.button.y;
          count++;
          if (count==2)
          {
             count=0;
             wxmin=xw[0];
             wymin=yw[0];
             wxmax=xw[1];
             wymax=yw[1];
             SDL_SetRenderDrawColor(ren,0,0,0,255);
             SDL_RenderFillRect(ren,NULL);
             SDL_SetRenderDrawColor(ren,255,255,255,255);
             GC_DrawRect(ren,xw[0],yw[0],xw[1]-xw[0],yw[1]-yw[0]);
             for (i=0; i<n; i++)
               for (j=i+1; j<n; j++)
                {
                 fx1=x[i];
                 fy1=y[i];
                 fx2=x[j];
                 fy2=y[j];
                 clip_line(&fx1,&fy1,&fx2,&fy2,wxmin,wymin,wxmax,wymax,&accept);
                 if (accept)
                   SDL_RenderDrawLine(ren,(int)(fx1+0.5),(int)(fy1+0.5),(int)(fx2+0.5),(int)(fy2+0.5));
                }
           }
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
              SDL_SetRenderDrawColor(ren,255,255,255,255);
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
