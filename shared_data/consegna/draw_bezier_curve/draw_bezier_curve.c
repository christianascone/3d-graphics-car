#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "GCGraLib2/GCGraLib2.h"

#define DIM 100
#define T_STEP 0.02
#define CIRCLE_RAD 3

/*funzione di redraw*/
void redraw(int n, int x[], int y[], SDL_Renderer *ren)
{
  int i;

  GC_FillCircle(ren, x[0], y[0], 3);
  for (i = 1; i <= n; i++)
  {
    SDL_RenderDrawLine(ren, x[i - 1], y[i - 1], x[i], y[i]);
    GC_FillCircle(ren, x[i], y[i], 3);
  }
}

/*
* Verifica se il punto relativo all'evento (xEvent e yEvent) è nei "confini" del punto esistente (xPoint e yPoint)
*/
int isInBound(int xPoint, int yPoint, int xEvent, int yEvent) {
  if ((xEvent >= (xPoint - CIRCLE_RAD) && xEvent <= (xPoint + CIRCLE_RAD)) && (yEvent >= (yPoint - CIRCLE_RAD) && yEvent <= (yPoint + CIRCLE_RAD))) {
    return 0;
  } else {
    return 1;
  }
}

/**
* Formula per l'algoritmo di De Casteljau
*/
int de_casteljau_formula(int valueI, int valueII, float t) {
  return (1 - t) * valueI + t * valueII;
}

void bezier_point(int rank, int k, int bezX[], int bezY[], float t, int result[]) {
  int i;

  for ( i = 0; i <= rank - k; i++) {
    bezX[i] = de_casteljau_formula(bezX[i], bezX[i + 1], t);
    bezY[i] = de_casteljau_formula(bezY[i], bezY[i + 1], t);
  }

  // Verifico condizioni per l'uscita
  if (rank == k) {
    result[0] = bezX[0];
    result[1] = bezY[0];
    return;
  }

  // Applico ricorsivamente
  bezier_point(rank, k + 1, bezX, bezY, t, result);
}

/*
* Disegna la curva di bezier di grado n-1, dove n è il numero di punti disegnati sullo schermo
*/
void draw_bezier(SDL_Renderer *ren, int n, int x[], int y[], int bezX[], int bezY[]) {
  int i, rank, k;
  float t;
  int oldResult[2];
  int result[2];

  rank = n - 1;
  k = 0;
  printf("Disegno Bezier - grado: %d\n", rank);

  // Copia dei punti di controllo nell'array per l'applicazione dell'algoritmo
  for (i = 0; i < n; i++) {
    bezX[i] = x[i];
    bezY[i] = y[i];
  }

  // Inizializzo con il primo punto
  bezier_point(rank, k + 1, bezX, bezY, 0, oldResult);
  for (t = T_STEP; t < 1; t += T_STEP) {
    bezier_point(rank, k + 1, bezX, bezY, t, result);
    int newX = result[0];
    int newY = result[1];
    // Recupero gli old perché mi servono per tracciare
    // la curva
    int oldX = oldResult[0];
    int oldY = oldResult[1];

    SDL_RenderDrawLine(ren, oldX, oldY, newX, newY);

    oldResult[0] = result[0];
    oldResult[1] = result[1];
  }

}

/*semplice programma di prova di input da mouse e tastiera*/
int main(void)
{
  SDL_Window *win;
  SDL_Renderer *ren;
  SDL_Event event;
  int vxmax, vymax;
  int esc = 1, i, n = 0;
  int x[DIM], y[DIM];
  int bezX[DIM], bezY[DIM];
  Uint32 windowID;
  int editing = 0;
  int editingIndex = 0;
  int ok;


  if (SDL_Init(SDL_INIT_VIDEO) < 0)
  {
    fprintf(stderr, "Couldn't init video: %s\n", SDL_GetError());
    return (1);
  }

  vxmax = 300;
  vymax = 300;

  win = SDL_CreateWindow("Inter_Polygon", 100, 100, vxmax, vymax,
                         SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
  if (win == NULL) {
    fprintf(stderr, "SDL_CreateWindow Error: %s\n", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (ren == NULL) {
    SDL_DestroyWindow(win);
    fprintf(stderr, "SDL_CreateRenderer Error: %s\n", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
  SDL_RenderClear(ren);
  SDL_SetRenderDrawColor(ren, 255, 0, 50, 255);
  SDL_RenderPresent(ren);

  printf("\nDisegno di una poligonale interattivamente: \n");
  printf("Dare i punti sulla finestra grafica con il mouse (button sinistro) \n");
  printf("Per spostare un punto esistente, cliccare su di esso e successivamente cliccare sulla nuova posizione (button sinistro) \n");
  printf("Uscire dal programma con il tasto <ESC>  \n");

  while (esc)
  {
    if (SDL_PollEvent(&event))
      switch (event.type)
      {
      case SDL_MOUSEBUTTONDOWN:
        // Disegna un punto sullo schermo ed eventualmente lo collega al precedente
        if (event.button.button == 1)
        {
          // Se sto editando mi salvo il valore relativo all'evento del mouse
          if (editing == 1) {
            // Resetto editing
            editing = 0;
            // Sposto il vertice 'editingIndex' nelle coordinate indicate dal mouse
            x[editingIndex] = event.button.x;
            y[editingIndex] = event.button.y;
            // Diminuisco il numero di vertici perché incremento dopo
            n--;
          } else {

            for (i = 0; i < n; i++) {
              // Verifico se sto cliccando su un vertice
              ok = isInBound(x[i], y[i], event.button.x, event.button.y);
              // Metto l'editing a true e salvo l'indice del vertice che sto modificando
              if (ok == 0) {
                editing = 1;
                editingIndex = i;
              }
            }

            // Esco perché sono in editing
            if (editing == 1) {
              break;
            }

            x[n] = event.button.x;
            y[n] = event.button.y;
          }

          SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
          SDL_RenderClear(ren);
          SDL_SetRenderDrawColor(ren, 255, 0, 50, 255);
          SDL_RenderPresent(ren);

          for (i = 0; i <= n; i++) {
            GC_FillCircle(ren, x[i], y[i], CIRCLE_RAD);
            if (i > 0) {
              SDL_RenderDrawLine(ren, x[i - 1], y[i - 1], x[i], y[i]);
            }
          }
          n++;

          if (n > 2) {
            draw_bezier(ren, n, x, y, bezX, bezY);
          }
        }

        SDL_RenderPresent(ren);
        break;
      // Exit
      case SDL_KEYDOWN:
        if (event.key.keysym.sym == SDLK_ESCAPE)
          esc = 0;
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
            redraw(n - 1, x, y, ren);

            SDL_RenderPresent(ren);
            break;
          }
          }
        }
        break;

      }
  }

  SDL_Quit();
  return (0);
}
