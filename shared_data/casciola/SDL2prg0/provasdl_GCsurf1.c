#include <stdio.h>
#include <math.h>
#include "../GCGraLib2/GCGraLib2.h"

int main(void)
{

SDL_Window *win;
SDL_Surface *surf,*screen;
Uint32 color;
SDL_Rect srcrect;

srcrect.x = 0;
srcrect.y = 0;
srcrect.w = 100;
srcrect.h = 100;


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

surf = SDL_CreateRGBSurface(0,640,480,24,0,0,0,0);
if(surf == NULL) {
      fprintf(stderr, "CreateRGBSurface failed: %s\n", SDL_GetError());
      exit(1);
}

screen = SDL_GetWindowSurface(win);

        // Clear the rendering surface with the specified color
        SDL_FillRect(surf, NULL, SDL_MapRGB(surf->format, 255, 255, 255));
        color=SDL_MapRGB(surf->format, 255, 0, 0);
        SDL_FillRect(surf, &srcrect, color);
        GC_DrawLine1(surf, 0,0,640,480,color);
        color=SDL_MapRGB(surf->format, 0, 255, 0);
        GC_DrawLine1(surf, 640,0,0,480,color);
	//Update the screen
        SDL_BlitSurface(surf, NULL, screen, NULL); // blit it to the screen
        SDL_FreeSurface(surf);

        SDL_UpdateWindowSurface(win);
	//Take a quick break after all that hard work
	SDL_Delay(2000);

SDL_DestroyWindow(win);
SDL_Quit();
return 0;
}

