#include <stdio.h>
#include <math.h>
#include "../GCGraLib2/GCGraLib2.h"

int main(void)
{

SDL_Window *win;
SDL_Renderer *ren;
SDL_Surface *image;
int i,w,h;


if(SDL_Init(SDL_INIT_VIDEO)!=0)
  {
    fprintf(stderr,"Couldn't init video: %s\n",SDL_GetError());
    return(1);
  }

  win= SDL_CreateWindow("Load Image", 100, 100, 640, 480, SDL_WINDOW_SHOWN);
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

//A sleepy rendering loop, wait for 3 seconds and render and present the screen each time
  for (i = 0; i < 3; ++i){
	//First clear the renderer
	SDL_RenderClear(ren);
        image = IMG_Load("WOLF1.bmp");
        w=image->w;
        h=image->h;
        GC_DrawImage(image,0,0,w,h,ren,0,0);
	//Update the screen
	SDL_RenderPresent(ren);
	//Take a quick break after all that hard work
	SDL_Delay(1000);
  }

SDL_FreeSurface(image);
SDL_DestroyRenderer(ren);
SDL_DestroyWindow(win);
SDL_Quit();
return 0;
}

