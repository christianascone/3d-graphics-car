#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <locale.h>
#include "../GCGraLib2/GCGraLib2.h"

/* Definizione di costanti */
#define RAGGIO 80
#define DIFFORA 30
#define DIFFMIN 15

/* Funzioni statiche*/
static void disegnaOrologio(SDL_Renderer *ren, SDL_Surface *points);
static void disegna(SDL_Renderer *ren,SDL_Surface *points);
static void updateSecondi(SDL_Renderer *ren, int secondo);
static void updateMinuti(SDL_Renderer *ren, int minuto);
static void updateOre(SDL_Renderer *ren, int ora, int minuto);

/* variabili globali */
int width=200,height=200;
int pwidth=8,pheight=8;
int cx=100,cy=100;

/* Funzione main */
int main (int argc, char *argv[])
{
SDL_Window *win;
SDL_Renderer *ren;
SDL_Event event;
int done = 0;
int exitstate=0;

    if (SDL_Init(SDL_INIT_VIDEO) == -1)
    {
        fprintf(stderr, "Errore di inizializzazione di SDL : %s\n", SDL_GetError());
        exit (EXIT_FAILURE);
    }

    win= SDL_CreateWindow("Orologio", 0, 0, width, height, SDL_WINDOW_SHOWN);
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

//carica immagine per punti con IMG_Load di SDL_image
//    SDL_Surface *points = IMG_Load("points.gif");
//carica immagine per punti con GC_LoadImage di GC_GraLib2
    SDL_Surface *points = GC_LoadImage("points.ppm",&exitstate);
    if (!points)
    {
        fprintf(stderr, "Errore nel caricare l'immagine : %s.\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    while(!done)
    {
        while(SDL_PollEvent(&event))
        {
            switch(event.type)
            {
                case SDL_QUIT:
                done = 1;
                break;

                default:
                break;
            }
        }
        disegna(ren,points);
    }

    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return EXIT_SUCCESS;
}

/* Disegna l'orologio; il quadrante e' composto da 12 punti*/
void disegnaOrologio(SDL_Renderer *ren, SDL_Surface *points)
{
SDL_Rect Pixel;
int conta_ore;
int angolo = 30;  /* angolo in cui sono posizionati i punti sul cerchio/quadrante*/
int trasla = 10;  /* per centrare il quadrante */

SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
SDL_RenderClear(ren);

Pixel.w=8;
Pixel.h=8;
SDL_SetRenderDrawColor(ren, 200, 0, 0, 255);
/*Il ciclo permette di calcolare la posizione dei punti (trigonometria)*/
    for (conta_ore = 12; conta_ore > 0; conta_ore --)
    {
        Pixel.x = (int)RAGGIO * cos((M_PI * (angolo) / 180) * conta_ore) + RAGGIO
                          + pwidth + trasla;
        Pixel.y = (int)RAGGIO * sin((M_PI * (angolo) / 180) * conta_ore) + RAGGIO
                          + pheight + trasla;
//disegna i punti caricati come immagine con la GC_DrawImage della GC_GraLib2
        GC_DrawImage(points,0,0,points->w,points->h,ren,Pixel.x,Pixel.y);
//disegna i punti anziche' usarne l'immagine
        //SDL_RenderFillRect(ren,&Pixel);
    }
}

/*lancetta dei secondi*/
void updateSecondi(SDL_Renderer *ren, int secondo)
{
int fx,fy;
double R, A; /* raggio e angolo */ 

    SDL_SetRenderDrawColor(ren, 255, 4, 255, 255);

    R = RAGGIO;
    A = (M_PI_2)-secondo*(M_PI)/30;

    /* calcolo della posizione finale della lancetta */
    /* la lancetta consiste in un segmento che parte dal centro del quadrante */
    fx = (int)(cos(A)*R + cx);
    fy = (int)(-sin(A)*R + cy);

    SDL_RenderDrawLine(ren, cx, cy, fx, fy);
}

/*lancetta dei minuti*/
void updateMinuti(SDL_Renderer *ren, int minuto)
{
int fx,fy;
double R, A;

    SDL_SetRenderDrawColor(ren, 4, 4, 255, 255);

    R = RAGGIO - DIFFMIN;
    A = (M_PI_2)-minuto*(M_PI)/30;
    fx = (int)(cos(A)*R + cx);
    fy = (int)(-sin(A)*R + cy);

    SDL_RenderDrawLine(ren, cx, cy, fx, fy);
}

/*lancetta delle ore*/
void updateOre(SDL_Renderer *ren, int ora, int minuto)
{
int fx,fy;
double R, AInter;
int min;

    SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
    R = RAGGIO-DIFFORA;
    
    /*Interpolazione*/
    min = minuto/60.0;
    
    /*Calcola l'ora attuale*/
    AInter =(M_PI_2)-(ora + min)*(M_PI)/6;
       
    /* contrariamente ai minuti e ai secondi, si fa avanzare la lancetta delle */ 
    /* in modo diverso. Sono movimenti basati sui giri dei minuti */
    fx = (int)(cos(AInter)*R + cx);
    fy = (int)(-sin(AInter)*R + cy);
    
    SDL_RenderDrawLine(ren, cx, cy, fx, fy);
}

void disegna(SDL_Renderer *ren,SDL_Surface *points)
{
struct tm *tPre; /* tm e time_t sono predefinite in time.h */
time_t timePresent;

/* legge l'ora attuale*/
    timePresent = time (NULL);
    tPre = localtime(&timePresent);

    disegnaOrologio(ren,points);
    updateMinuti(ren, tPre->tm_min);
    updateOre(ren, tPre->tm_hour, tPre->tm_min);
    updateSecondi(ren,tPre->tm_sec);
    SDL_RenderPresent(ren);
}
