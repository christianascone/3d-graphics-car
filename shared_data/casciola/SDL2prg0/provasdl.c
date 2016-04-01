#include <stdio.h>
#include <math.h>
#include <SDL2/SDL.h>
int main(void)
{

SDL_Window *win;
SDL_Renderer *ren;
SDL_Surface *bmp;
SDL_Texture *tex;
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

ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
if (ren == NULL){
	SDL_DestroyWindow(win);
	fprintf(stderr,"SDL_CreateRenderer Error: %s\n",SDL_GetError());
	SDL_Quit();
	return 1;
}

bmp = SDL_LoadBMP("WOLF1.bmp");
if (bmp == NULL){
	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	fprintf(stderr,"SDL_LoadBMP Error: %s\n",SDL_GetError());
	SDL_Quit();
	return 1;
}

tex = SDL_CreateTextureFromSurface(ren, bmp);
SDL_FreeSurface(bmp);
if (tex == NULL){
	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	fprintf(stderr,"SDL_CreateTextureFromSurface Error: %s\n",SDL_GetError());
	SDL_Quit();
	return 1;
}

//A sleepy rendering loop, wait for 3 seconds and render and present the screen each time
for (i = 0; i < 3; ++i){
	//First clear the renderer
	SDL_RenderClear(ren);
	//Draw the texture
	SDL_RenderCopy(ren, tex, NULL, NULL);
	//Draw a Line with a color
        SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
        SDL_RenderDrawLine(ren,0,0,640,480);
	//Update the screen
	SDL_RenderPresent(ren);
	//Take a quick break after all that hard work
	SDL_Delay(1000);
}

SDL_DestroyTexture(tex);
SDL_DestroyRenderer(ren);
SDL_DestroyWindow(win);
SDL_Quit();
return 0;
}

