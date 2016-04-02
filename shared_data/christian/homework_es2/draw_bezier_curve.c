#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../GCGraLib2/GCGraLib2.h"

#define DIM 100

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

void bezier_point(int rank, int k, int bezX[], int bezY[], float t, int result[]){
  int i;

  printf("*** Bezier ***\n");
  printf("rank %d, k %d\n", rank, k);

  if(rank == k){
    result[0] = bezX[0];
    result[1] = bezY[0];

    return;
  }
  
  // sizeof x - k mi dice quanti punti ho da analizzare

  for( i = 0; i <= rank-k; i++){
    bezX[i] = (1-t)*bezX[i] + t*bezX[i+1];
    bezY[i] = (1-t)*bezY[i] + t*bezY[i+1];

    printf("Bezier point: k=%d - P%d -> x=%d, y=%d \n", k, i, bezX[i], bezY[i]);
  }

  bezier_point(rank, k+1, bezX, bezY, t, result);
}

void draw_bezier(SDL_Renderer *ren, int n, int x[], int y[], int bezX[], int bezY[]){
  int i, rank, k, t;
  int oldResult[2];
  int result[2];

  rank = n-1;
  k = 0;
  printf("Bezier rank: %d\n", rank);

  for(i = 0; i < n; i++){
        bezX[i]=x[i];
        bezY[i]=y[i];
    }


bezier_point(rank, k+1, bezX, bezY, 0, oldResult);
printf("OldX %d, OldY %d \n", oldResult[0], oldResult[1]);
//for(t = 0.1; t < 1; t+=0.1){
t = 1;
  bezier_point(rank, k+1, bezX, bezY, t, result);
  int newX = result[0];
  int newY = result[1];

  int oldX = oldResult[0];
  int oldY = oldResult[1];

  printf("OldX %d, OldY %d \n", oldX, oldY);
  printf("newX %d, newY %d \n", newX, newY);

  SDL_RenderDrawLine(ren, x[0], y[0], oldX, oldY);
  SDL_RenderDrawLine(ren, oldX, oldY, newX, newY);
  SDL_RenderDrawLine(ren, newX, newY, x[n], y[n]);
//}

    
// FOR con t che va da 0 a 1
    //printf("Bezier point finale %d,%d\n", *(result + 0), *(result + 1));
}

/*semplice programma di prova di input da mouse e tastiera*/
int main(void) 
{
  SDL_Window *win;
  SDL_Renderer *ren;
  SDL_Event event;
  int vxmax,vymax;
  int esc=1,i,j,n=0;
  int x[DIM],y[DIM];
  int bezX[DIM],bezY[DIM]; 
  Uint32 windowID;


  if(SDL_Init(SDL_INIT_VIDEO)<0)
  {
    fprintf(stderr,"Couldn't init video: %s\n",SDL_GetError());
    return(1);
  }

  vxmax=300;
  vymax=300;

  win= SDL_CreateWindow("Inter_Polygon", 100, 100, vxmax, vymax, 
   SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
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

 SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
 SDL_RenderClear(ren);
 SDL_SetRenderDrawColor(ren, 255, 0, 50, 255);
 SDL_RenderPresent(ren);

 printf("\nDisegno di una poligonale interattivamente: \n");
 printf("Dare i punti sulla finestra grafica con il mouse (button sinistro) \n");
 printf("Finire l'inserimento con il button destro \n");
 printf("Uscire dal programma con il tasto <ESC>  \n");

 while(esc)
 {
  if (SDL_PollEvent(&event))
    switch(event.type)
  {
   case SDL_MOUSEBUTTONDOWN:
    // Disegna un punto sullo schermo ed eventualmente lo collega al precedente
   if(event.button.button==1)
   {
    x[n]=event.button.x;
    y[n]=event.button.y;
    GC_FillCircle(ren,x[n],y[n],3);
    if(n>0) 
      SDL_RenderDrawLine(ren, x[n-1], y[n-1], x[n], y[n]);
    n++;

    if(n>2)
    draw_bezier(ren, n, x, y, bezX, bezY);
  }

  // Tasto destro: chiude il poligono
  if(event.button.button==3)
  {
   SDL_RenderDrawLine(ren, x[n-1], y[n-1], x[0], y[0]);

  /* disegno artistico: definire DEBUG nel Makefile */
  #ifdef DEBUG 
     for (i=0; i<n; i++)
      for (j=i+1; j<n; j++)
        SDL_RenderDrawLine(ren, x[i], y[i], x[j], y[j]);
      n=0;
  #endif
  }
  SDL_RenderPresent(ren);
  break;
  // Exit
  case SDL_KEYDOWN:
  if(event.key.keysym.sym == SDLK_ESCAPE)
    esc=0;
  break;

  case SDL_WINDOWEVENT:
  windowID = SDL_GetWindowID(win);
  if (event.window.windowID == windowID)  {
   switch (event.window.event)  {
     case SDL_WINDOWEVENT_SIZE_CHANGED:  {
       vxmax = event.window.data1;
       vymax = event.window.data2;
//          printf("vxmax= %d \n vymax= %d \n", vxmax,vymax);

       SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
       SDL_RenderClear(ren);
       SDL_SetRenderDrawColor(ren, 255, 0, 50, 255);
       redraw(n-1,x,y,ren);

       SDL_RenderPresent(ren);
       break;
     }
   }
 }
 break;

}
}

SDL_Quit();
return(0);
}
