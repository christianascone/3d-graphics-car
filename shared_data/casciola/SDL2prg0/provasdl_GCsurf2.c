#include <stdio.h>
#include <math.h>
#include "../GCGraLib2/GCGraLib2.h"

int main(void)
{

SDL_Window *win;
SDL_Renderer *ren;
SDL_Surface *surf;
int i;


  if(SDL_Init(SDL_INIT_VIDEO)!=0)
  {
    fprintf(stderr,"Couldn't init video: %s\n",SDL_GetError());
    return(1);
  }

  win= SDL_CreateWindow("Hello world!", 100, 100, 640, 480, SDL_WINDOW_SHOWN);
  if(win==NULL){
	fprintf(stderr,"SDL_CreateWindow Error: %s\n",SDL_GetError());
	SDL_Quit();
	return 1;
  }

    surf = SDL_GetWindowSurface(win);
    ren = SDL_CreateSoftwareRenderer(surf);
    if (!ren) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Render creation for surface fail : %s\n",SDL_GetError());
        return 1;
    }

//A sleepy rendering loop, wait for 3 seconds and render and present the screen each time
for (i = 0; i < 3; ++i){
        // Clear the rendering surface with the specified color
        SDL_SetRenderDrawColor(ren, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(ren);
        SDL_UpdateWindowSurface(win);
	SDL_Delay(1000);
	//Update the screen
        SDL_SetRenderDrawColor(ren, 0, 0, 0, 0xFF);
        SDL_RenderDrawLine(ren,0,0,640,480);
        SDL_UpdateWindowSurface(win);
	//Take a quick break after all that hard work
	SDL_Delay(1000);
}

SDL_DestroyRenderer(ren);
SDL_DestroyWindow(win);
SDL_Quit();
return 0;
}

