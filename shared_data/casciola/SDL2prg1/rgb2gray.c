/* rgb2gray.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../GCGraLib2/GCGraLib2.h"

#define DEFAULT_PTSIZE  18

typedef struct
{
   int centerx;
   int centery;
   int radius;
   char option[16];
} CIRC;

/* conversione di un intero da base 10 a base b */
void conv(Uint32 m, int b, int a[])
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

/* funzione di determinazione opzione del menu' scelta */
void opt_menu(CIRC menu[],int n,int ix,int iy,int *choice)
{
int i;

  *choice=-1;
  for(i=0; i<n; i++)
  {
    if (sqrt(pow((double)ix-menu[i].centerx,2)+pow((double)iy-menu[i].centery,2))<=menu[i].radius)
	    *choice=i;
  }
}

int main()
{
SDL_Window *win;
SDL_Renderer *ren;
SDL_Surface *tux,*tux1;
int res = 0;
SDL_Event event;
TTF_Font *font;
int i,j,pix_int[3],pix[3];
int w,h,dw,px,py;
Uint32 pixelval,pixelvalue1,pixelvalue2,pixelvalue3;
int esc=1,choice;
CIRC menu[10];

/* load an image by file */ 
  tux = GC_LoadImage("foto_rgb.ppm",&res);
  tux1 = GC_LoadImage("foto_rgb.ppm",&res);
  if (res == 0)
    return 1;

/* Initialize the graphics session */
  if(SDL_Init(SDL_INIT_VIDEO)<0)
  {
    fprintf(stderr,"Couldn't init video: %s\n",SDL_GetError());
    return(1);
  }

/* Initialize the TTF library */
  if(TTF_Init() < 0)
  {
    fprintf(stderr, "Couldn't initialize TTF: %s\n",SDL_GetError());
    SDL_Quit();
    return(2);
  }

  font = TTF_OpenFont("FreeSans.ttf", DEFAULT_PTSIZE);
  if(font == NULL)
  {
    fprintf(stderr, "Couldn't load font\n");
  }
//  TTF_SetFontStyle(font, TTF_STYLE_NORMAL);

/* create the image window */
  w=tux->w;
  h=tux->h;
  dw=w+150;

  win= SDL_CreateWindow("Image Processing", 100, 100, w+dw, h, SDL_WINDOW_SHOWN);
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


/*Clear the renderer */
  SDL_RenderClear(ren);
/* copy the image into the window */ 
  GC_DrawImage(tux, 0, 0, w, h, ren, 0, 0);
  SDL_RenderPresent(ren);

/* set menu coordinates: center and radius*/
    for (i=0; i<=4; i++)
    {
      menu[i].centerx=w+20;
      menu[i].centery=20+i*30;
      menu[i].radius=10;
    }
    strcpy(menu[0].option, "Negative");
    strcpy(menu[1].option, "Gray");
    strcpy(menu[2].option, "Neg-Gray");
    strcpy(menu[3].option, "B/W");
    strcpy(menu[4].option, "Neg-B/W");
/* draw menu check-points */
    SDL_SetRenderDrawColor(ren, 240, 240, 240, 255);
    for (i=0; i<=4; i++)
    {
      GC_FillCircle(ren,menu[i].centerx,menu[i].centery,menu[i].radius);
      px=menu[i].centerx+menu[i].radius+5;
      py=menu[i].centery-menu[i].radius;
      GC_DrawText(ren,font,255,255,255,0,0,0,0,0,menu[i].option,px,py,shaded);
    }
    GC_DrawText(ren,font,255,255,255,0,0,0,0,0,"<Esc>",w+30,h-40,shaded);
    SDL_RenderPresent(ren);

/* main loop for events */
    do
    {
     if (SDL_PollEvent(&event))
      switch(event.type)
      {
        case SDL_MOUSEBUTTONDOWN:
           if(event.button.button==1)
           {
            opt_menu(menu,5,event.button.x,event.button.y,&choice);
            switch(choice)
            {
	    case 0:
/* transform image to be negative */
	      SDL_LockSurface(tux1);
	      pixelvalue3 = 256*256*256;
              for (i=0;i<h;i++)
                 for (j=0;j<w;j++)
                 {
	           pixelvalue1=GC_GetPixelImage(tux,j,i);
	           pixelvalue2=pixelvalue3-pixelvalue1;
                   GC_PutPixel1(tux1,j,i,pixelvalue2);
                 }
	      SDL_UnlockSurface(tux1);
              GC_DrawImage(tux1,0,0,w,h,ren,dw,0);
              SDL_RenderPresent(ren);
              break;
            case 1:
/* transform image colors to gray levels */
	      SDL_LockSurface(tux1);
              for (i=0;i<h;i++)
                 for (j=0;j<w;j++)
                 {
	           conv(GC_GetPixelImage(tux,j,i),256,pix);
	           pix_int[0]=pix_int[1]=pix_int[2]=(int)((pix[0]+pix[1]+pix[2])/3.0);
                   pixelval=pix_int[2]*256*256+pix_int[1]*256+pix_int[0];
                   GC_PutPixel1(tux1,j,i,pixelval);
                 }
	      SDL_UnlockSurface(tux1);
              GC_DrawImage(tux1,0,0,w,h,ren,dw,0);
              SDL_RenderPresent(ren);
              break;
	    case 2:
/* transform image colors to negative-gray levels */
	      SDL_LockSurface(tux1);
	      pixelvalue3 = 256*256*256;
              for (i=0;i<h;i++)
                 for (j=0;j<w;j++)
                 {
	           conv(GC_GetPixelImage(tux,j,i),256,pix);
	           pix_int[0]=pix_int[1]=pix_int[2]=(int)((pix[0]+pix[1]+pix[2])/3.0);
                   pixelval=pix_int[2]*256*256+pix_int[1]*256+pix_int[0];
	           pixelvalue2=pixelvalue3-pixelval;
                   GC_PutPixel1(tux1,j,i,pixelvalue2);
                 }
	      SDL_UnlockSurface(tux1);
              GC_DrawImage(tux1,0,0,w,h,ren,dw,0);
              SDL_RenderPresent(ren);
              break;
            case 3:
/* transform image colors to Black and White */
	      SDL_LockSurface(tux1);
              for (i=0;i<h;i++)
                 for (j=0;j<w;j++)
                 {
	           conv(GC_GetPixelImage(tux,j,i),256,pix);
	           pix_int[0]=(int)((pix[0]+pix[1]+pix[2])/3.0);
	           if(pix_int[0]<128)
		       pix_int[0]=pix_int[1]=pix_int[2]=0;
	           else
		       pix_int[0]=pix_int[1]=pix_int[2]=255;
                   pixelval=pix_int[2]*256*256+pix_int[1]*256+pix_int[0];
                   GC_PutPixel1(tux1,j,i,pixelval);
                 }
	      SDL_UnlockSurface(tux1);
              GC_DrawImage(tux1,0,0,w,h,ren,dw,0);
              SDL_RenderPresent(ren);
              break;
            case 4:
/* transform image colors to Negative Black and White */
	      SDL_LockSurface(tux1);
              for (i=0;i<h;i++)
                 for (j=0;j<w;j++)
                 {
	           conv(GC_GetPixelImage(tux,j,i),256,pix);
	           pix_int[0]=(int)((pix[0]+pix[1]+pix[2])/3.0);
	           if(pix_int[0]>=128)
		       pix_int[0]=pix_int[1]=pix_int[2]=0;
	           else
		       pix_int[0]=pix_int[1]=pix_int[2]=255;
                   pixelval=pix_int[2]*256*256+pix_int[1]*256+pix_int[0];
                   GC_PutPixel1(tux1,j,i,pixelval);
                 }
	      SDL_UnlockSurface(tux1);
              GC_DrawImage(tux1,0,0,w,h,ren,dw,0);
              SDL_RenderPresent(ren);
              break;
	    }
          }
	  break;
	  case SDL_KEYDOWN:
           if(event.key.keysym.sym == SDLK_ESCAPE)
	     esc=0;
	  break;
      }
    }while(esc);

    TTF_CloseFont(font);
    TTF_Quit();
    SDL_FreeSurface(tux);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return(0);
}
