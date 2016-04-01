#include <stdio.h>
#include <math.h>
//#include <SDL2/SDL.h>
//#include <SDL2/SDL_ttf.h>
//#include <SDL2/SDL_image.h>
//#include <SDL2/SDL_video.h>
#include "../GCGraLib2/GCGraLib2.h"

#define MAXGRID (100)
#define DEFAULT_PTSIZE  14

/* Dati per visualizzare una funzione z=f(x,y)*/
float xmin=1.0,xmax=10.0,ymin=1.0,ymax=10.0; 	/* dominio funzione*/
float zmin,zmax;				/* min max funzione */
int nx=20,ny=20; 				/* dimensioni griglia  */

float 	x[MAXGRID],y[MAXGRID],z[MAXGRID][MAXGRID];			
float	csx,csy,csz;				/* centro oggetto */
float 	D = 25,teta = 20,fi = 20,di;
int	xs[MAXGRID][MAXGRID],ys[MAXGRID][MAXGRID];/* coordinate schermo */

int 	xvmin,xvmax,yvmin,yvmax;	/* coordinate viewport */
float   salpha=0.05;			/* zoomin e zoomout */
float   sr=0.25,ssx,ssy,ssz,ssmod,	 /* modifica centro oggetto */
	Dstep=2.5,			/* passo di incremento D */
	alpha,				/* apertura angolare piramide di vista */
        s;
float 	xwmin,xwmax,ywmin,ywmax; 	/* coordinate window */
float 	Sx, Sy;				/* fattori di scala trasf. window-viewport */

float 	tmpx,tmpy;			/* variabili temporanee */
int 	i,j;

typedef struct RECT RECT;
struct RECT
{
  SDL_Rect rect;
  char     text[10];
};

/* Dati per la costruzione delle finestre */
//int           tetawin,finewin,fiwin,zowin,ziwin,leftwin,rightwin,
//	        upwin,downwin,Dzowin,Dziwin;
RECT            menu[15];
int             done=0;


float sqr(float x)
{
        return(x*x);
}
        
/* inizializzazione X */
void init_menu(SDL_Renderer *ren, RECT menu[],float teta, float fi)
{
menu[0].rect.x = 605;
menu[0].rect.y = 550;
menu[0].rect.w = 100;
menu[0].rect.h = 30;
strcpy(menu[0].text, "QUIT");
//finewin = 0;

menu[1].rect.x = 605;
menu[1].rect.y = 20;
menu[1].rect.w = 100;
menu[1].rect.h = 100;
strcpy(menu[1].text, "THETA");
//tetawin=1;

menu[2].rect.x = 605;
menu[2].rect.y = 150;
menu[2].rect.w = 100;
menu[2].rect.h = 100;
strcpy(menu[2].text, "PHI");
//fiwin = 2;

menu[3].rect.x = 602;
menu[3].rect.y = 280;
menu[3].rect.w = 50;
menu[3].rect.h = 30;
strcpy(menu[3].text, "ZoomI");
//ziwin = 3;

menu[4].rect.x = 662;
menu[4].rect.y = 280;
menu[4].rect.w = 50;
menu[4].rect.h = 30;
strcpy(menu[4].text, "ZoomO");
//zowin = 4;

menu[5].rect.x = 602;
menu[5].rect.y = 320;
menu[5].rect.w = 50;
menu[5].rect.h = 30;
strcpy(menu[5].text, "DZoomI");
//Dziwin = 5;

menu[6].rect.x = 662;
menu[6].rect.y = 320;
menu[6].rect.w = 50;
menu[6].rect.h = 30;
strcpy(menu[6].text, "DZoomO");
//Dzowin = 6;

menu[7].rect.x = 662;
menu[7].rect.y = 400;
menu[7].rect.w = 50;
menu[7].rect.h = 30;
strcpy(menu[7].text, "RIGHT");
//rightwin = 7;

menu[8].rect.x = 602;
menu[8].rect.y = 400;
menu[8].rect.w = 50;
menu[8].rect.h = 30;
strcpy(menu[8].text, "LEFT");
//leftwin = 8;

menu[9].rect.x = 631;
menu[9].rect.y = 360;
menu[9].rect.w = 50;
menu[9].rect.h = 30;
strcpy(menu[9].text, "UP");
//upwin = 9;

menu[10].rect.x = 631;
menu[10].rect.y = 440;
menu[10].rect.w = 50;
menu[10].rect.h = 30;
strcpy(menu[10].text, "DOWN");
//downwin = 10;

SDL_RenderDrawLine(ren, menu[1].rect.x+menu[1].rect.w/2, menu[1].rect.y+menu[1].rect.h/2,
	    (int)(menu[1].rect.w/2)*cos(teta)+menu[1].rect.x+menu[1].rect.w/2,
	    (int)(menu[1].rect.h/2)*sin(teta)+menu[1].rect.y+menu[1].rect.h/2);
SDL_RenderDrawLine(ren, menu[2].rect.x+menu[2].rect.w/2, menu[2].rect.y+menu[2].rect.h/2,
	    (int)(menu[2].rect.w/2)*cos(fi)+menu[2].rect.x+menu[2].rect.w/2,
	    (int)(menu[2].rect.h/2)*sin(fi)+menu[2].rect.y+menu[2].rect.h/2);
}

void draw_menu(RECT menu[], SDL_Renderer *ren, TTF_Font *font)
{
int i;

   for( i=0 ; i<=10 ; i++ )
   {
      GC_DrawText(ren, font, 0, 0, 0, 0, 255, 255, 255, 0, menu[i].text, 
                  menu[i].rect.x, menu[i].rect.y, shaded);
      //GC_DrawRect(ren,menu[i].rect.x,menu[i].rect.y,menu[i].rect.w,menu[i].rect.h);
      SDL_RenderDrawRect(ren,&(menu[i].rect));
   }
}

float rad(float angle)
{
    return(angle/180*3.1415);
}

/* funzione da visualizzare */
float fxy( float x, float y)
{
float result;

result = 9.0*(3.0/4.0*exp((-sqr(x-3.0)-sqr(y-3.0))/4.0)+exp(-sqr(x/7.0)-y/10.0)
-1.0/5.0*exp(-sqr(x-5.0)-sqr(y-8.0))+1.0/2.0*exp((-sqr(x-8.0)-sqr(y-4.0))/4.0));        

return(result);
}

/* funzione che costruisce la griglia di approx */
void define_fun_grid()
{
float hx,hy;
int i,j;

hx = (xmax - xmin)/nx;
hy = (ymax - ymin)/ny;
/* Determino la griglia di punti cross-hatched */
for (i = 0; i<=nx;i++)
	x[i] = xmin + (i*hx);
for (j = 0; j<= ny; j++)
	y[j] = ymin + (j*hy);	
for (i=0;i<=nx;i++)
	for (j=0;j<=ny;j++)
		z[i][j] = fxy(x[i],y[j]);
}		

/* trasformazione prospettica generico punto di vista */
void trasf_prosp_gen(int *init,float x, float y, float z, 
		     float *xe, float *ye, float *ze)
{
static float steta,cteta,cfi,sfi;

if (*init)
 {
   *init=0;
   steta=sin(teta);
   cteta=cos(teta);
   sfi=sin(fi);
   cfi=cos(fi);
 }

/* trasformazione in coordinate del sistema osservatore */	
*xe = -steta*x + y*cteta;
*ye = -cteta*cfi*x - y*steta*cfi + z*sfi;
*ze = -x*cteta*sfi - y *steta*sfi - z*cfi + D;
}

/* funzione che calcola i fattori di scala */
void define_view()
{
int 	t,u;
float 	r;

  zmin = z[1][1];
  zmax = zmin;
  for (t=0;t<=nx;t++)
	for (u=0;u<=ny;u++)
		if (z[t][u] > zmax) zmax = z[t][u];
		else if (z[t][u]<zmin) zmin = z[t][u];
/* centro oggetto iniziale */
  csx = (xmin + xmax)/2;
  csy = (ymin + ymax)/2;
  csz = (zmin + zmax)/2;
/* raggio sfera contenente oggetto */
  r = sqrt(sqr(xmax-csx)+sqr(ymax-csy)+sqr(zmax-csz));
/* calcolo semilato window iniziale; il piano di proiezione viene assunto
   ad una distanza D dall'osservatore */
  s = r*D/sqrt(sqr(D)-sqr(r));
/* in base al semilato window cosi' calcolato si determina l'apertura
   angolare iniziale */
  alpha=atan(s/D);
/* printf("\nApertura angolare: %f \n",alpha); */
}

void draw_fun(SDL_Renderer *ren)
{
int t,u,init;
float xe,ye,ze; 			/* coord. dell'osservatore */

/* si ricalcola il semilato della window in base a D e ad alpha */
  s=D*tan(alpha);
xwmin = -s ;
xwmax = s;
ywmin = -s;
ywmax = s;

/* fattori di scala per trasf. Window-Viewport */
Sx = ((float)(xvmax) - (float)(xvmin))/(xwmax - xwmin);
Sy = ((float)(yvmax) -(float)(yvmin))/(ywmax - ywmin);

/* il piano di proiezione viene aggiornato a passare per l'origine */  
di=D;
init=1;
for (i=0;i<=nx;i++)
   	for(j=0;j<=ny;j++)
        {
		trasf_prosp_gen(&init,x[i]-csx,y[j]-csy,z[i][j]-csz,&xe,&ye,&ze);
		/* proiezione e trasformazione in coordinate schermo */
		xs[i][j] = (int)(Sx * ((di * xe)/ze - xwmin) + xvmin + 0.5);
		ys[i][j] = (int)(Sy * (ywmin - (di * ye)/ze) + yvmax + 0.5);	
  	}

/* disegno mesh */
for (t = 0;t <=nx;t++)
	for (u=1;u<=ny;u++)
	        SDL_RenderDrawLine(ren, xs[t][u-1],ys[t][u-1],xs[t][u],ys[t][u]);

for(u=0;u<=ny;u++)
	for(t=1;t<=nx;t++)
	        SDL_RenderDrawLine(ren, xs[t-1][u],ys[t-1][u],xs[t][u],ys[t][u]);

SDL_RenderPresent(ren);
}				

/* funzione di determinazione opzione del menu' scelta */
void opt_menu(RECT menu[],int n,int ix,int iy,int *choice)
{
int i;

  *choice=-1;
  for(i=0; i<=n; i++)
  {
    if (ix>=menu[i].rect.x && iy>=menu[i].rect.y &&
        ix-menu[i].rect.x<=menu[i].rect.w && iy-menu[i].rect.y<=menu[i].rect.h)
            *choice=i;
  }
}
 

int main()
{
SDL_Window *win;
SDL_Renderer *ren;
SDL_Rect sub_v, v, bsub_v;
TTF_Font *font;
SDL_Event myevent;
int choice, done=0;
//int color, back0;

if (SDL_Init (SDL_INIT_VIDEO) < 0)
{
	fprintf (stderr, "Couldn't init video: %s\n", SDL_GetError ());
	return (1);
}

/* Initialize the TTF library */
if (TTF_Init () < 0)
{
	fprintf (stderr, "Couldn't initialize TTF: %s\n", SDL_GetError ());
	SDL_Quit ();
	return (2);
}

font = TTF_OpenFont ("FreeSans.ttf", DEFAULT_PTSIZE);
if (font == NULL)
{
	fprintf (stderr, "Couldn't load font\n");
}

v.x = 0;
v.y = 0;
v.w = 720;
v.h = 600;
xvmin = v.x+5;
yvmin = v.y+5;
xvmax = v.w-130;
yvmax = v.h-10;
sub_v.x = xvmin;
sub_v.y = yvmin;
sub_v.w = xvmax;
sub_v.h = yvmax;
bsub_v.x=sub_v.x-2;
bsub_v.y=sub_v.y-2;
bsub_v.w=sub_v.w+4;
bsub_v.h=sub_v.h+4;

  win= SDL_CreateWindow("View Bivariate Function", 0, 0, v.w, v.h, SDL_WINDOW_RESIZABLE);
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

//Set color
  SDL_SetRenderDrawColor(ren,255,255,255,255);
//Clear back buffer
  SDL_RenderClear(ren);
//Swap back and front buffer
  SDL_RenderPresent(ren);

  SDL_SetRenderDrawColor(ren,0,0,0,255);
  //GC_DrawRect(ren,sub_v.x-2,sub_v.y-2,sub_v.w+4,sub_v.h+4);
  SDL_RenderDrawRect(ren,&bsub_v);

init_menu(ren,menu,teta,fi);
draw_menu(menu,ren,font);
define_fun_grid();
define_view(); /* calcolo dei parametri di vista */
draw_fun(ren);


//Disegnare tutte le volte, anche per eventi intermedi,
//rallenta tutto senza dare un contributo all'immagine finale.
//In questo modo si possono scremare gli eventi.
//Viene usata la costante 0xff perchè nel codice sorgente delle SDL è usato
//come costante all'interno delle funzioni che accettano interi 8 bit come input
//per rappresentare SDL_ALLEVENTS (che invece e' a 32 bit)

while (done == 0)
{
 if (SDL_PollEvent(&myevent))
 {
  SDL_EventState(0xff, SDL_IGNORE); //0xff is all events
  switch(myevent.type)
  {
   case SDL_MOUSEMOTION:
    if(myevent.motion.state==1)
    {
       opt_menu(menu,11,myevent.motion.x,myevent.motion.y,&choice);
       switch(choice)
       {
        case 1: //choice==tetawin
  		tmpx = myevent.motion.x-(menu[1].rect.x+menu[1].rect.w/2);
  		tmpy = myevent.motion.y-(menu[1].rect.y+menu[1].rect.h/2);
  		teta = atan2(tmpy,tmpx);
                SDL_SetRenderDrawColor(ren,255,255,255,255);
		SDL_RenderFillRect(ren,&(menu[1].rect));
                SDL_SetRenderDrawColor(ren,0,0,0,255);
                GC_DrawText(ren, font, 0, 0, 0, 0, 255, 255, 255, 0, menu[1].text, 
		            menu[1].rect.x, menu[1].rect.y, shaded);
                //GC_DrawRect(ren,menu[1].rect.x,menu[1].rect.y,menu[1].rect.w,menu[1].rect.h);
                SDL_RenderDrawRect(ren,&(menu[1].rect));
                SDL_RenderDrawLine(ren, myevent.motion.x,myevent.motion.y,
                        menu[1].rect.x+menu[1].rect.w/2,menu[1].rect.y+menu[1].rect.h/2);

                SDL_SetRenderDrawColor(ren,255,255,255,255);
		SDL_RenderFillRect(ren,&sub_v);
                SDL_SetRenderDrawColor(ren,0,0,0,255);
		draw_fun(ren);
	 break;
	 case 2: //choice==fiwin 	  
  		tmpx = myevent.motion.x-(menu[2].rect.x+menu[2].rect.w/2);
  		tmpy = myevent.motion.y-(menu[2].rect.y+menu[2].rect.h/2);
                SDL_SetRenderDrawColor(ren,255,255,255,255);
		SDL_RenderFillRect(ren,&(menu[2].rect));
                SDL_SetRenderDrawColor(ren,0,0,0,255);
                GC_DrawText(ren, font, 0, 0, 0, 0, 255, 255, 255, 0, menu[2].text, 
		            menu[2].rect.x, menu[2].rect.y, shaded);
                //GC_DrawRect(ren,menu[2].rect.x,menu[2].rect.y,menu[2].rect.w,menu[2].rect.h);
                SDL_RenderDrawRect(ren,&(menu[2].rect));
                SDL_RenderDrawLine (ren, myevent.motion.x,myevent.motion.y,
                        menu[2].rect.x+menu[2].rect.w/2,menu[2].rect.y+menu[2].rect.h/2);

                SDL_SetRenderDrawColor(ren,255,255,255,255);
		SDL_RenderFillRect(ren,&sub_v);
                SDL_SetRenderDrawColor(ren,0,0,0,255);
  		fi = atan2(tmpy,tmpx);
  		draw_fun(ren);
         break;
        }
    }
    break;
  
    case SDL_MOUSEBUTTONDOWN:
        if(myevent.button.button==1)
        {
           opt_menu(menu,11,myevent.button.x,myevent.button.y,&choice);
           switch(choice)
           {
            case 0: //choice == finewin 
	        done = 1;
	    break;
	    case 7: //choice == rightwin
                SDL_SetRenderDrawColor(ren,255,255,255,255);
	        SDL_RenderFillRect(ren,&sub_v);
                ssx=-sin(teta);
		ssy=cos(teta);
		csx-=sr*ssx;
		csy-=sr*ssy;
                SDL_SetRenderDrawColor(ren,0,0,0,255);
		draw_fun(ren);
	    break;
	    case 8: //choice == leftwin
                SDL_SetRenderDrawColor(ren,255,255,255,255);
                SDL_RenderFillRect(ren,&sub_v);
		ssx=-sin(teta);
		ssy=cos(teta);
		csx+=sr*ssx;
		csy+=sr*ssy;
                SDL_SetRenderDrawColor(ren,0,0,0,255);
		draw_fun(ren);
	    break;
	    case 9: //choice == upwin
                SDL_SetRenderDrawColor(ren,255,255,255,255);
		SDL_RenderFillRect(ren,&sub_v);
		ssx=-cos(fi)*cos(teta);
		ssy=-cos(fi)*sin(teta);
		ssz=sin(fi);
		csx+=sr*ssx;
		csy+=sr*ssy;
		csz+=sr*ssz;
                SDL_SetRenderDrawColor(ren,0,0,0,255);
		draw_fun(ren);
	     break;	
	     case 10: //choice == downwin
                SDL_SetRenderDrawColor(ren,255,255,255,255);
		SDL_RenderFillRect(ren,&sub_v);
		ssx=-cos(fi)*cos(teta);
		ssy=-cos(fi)*sin(teta);
		ssz=sin(fi);
		csx-=sr*ssx;
		csy-=sr*ssy;
		csz-=sr*ssz;
                SDL_SetRenderDrawColor(ren,0,0,0,255);
		draw_fun(ren);
	     break;
	     case 3: //choice == ziwin
		if (alpha-salpha >0)
  		   alpha-=salpha;
                SDL_SetRenderDrawColor(ren,255,255,255,255);
		SDL_RenderFillRect(ren,&sub_v);
                SDL_SetRenderDrawColor(ren,0,0,0,255);
 		draw_fun(ren);
             break;
	     case 4: //choice == zowin
	        if (alpha+salpha <1.57)
  		   alpha+=salpha;
                SDL_SetRenderDrawColor(ren,255,255,255,255);
	        SDL_RenderFillRect(ren,&sub_v);
                SDL_SetRenderDrawColor(ren,0,0,0,255);
		draw_fun(ren);
             break;
	     case 5: //choice == Dziwin
	        if (D-Dstep>0)
	     	  D-=Dstep;
                SDL_SetRenderDrawColor(ren,255,255,255,255);
	        SDL_RenderFillRect(ren,&sub_v);
                SDL_SetRenderDrawColor(ren,0,0,0,255);
  		draw_fun(ren);
             break;
	     case 6: //choice == Dzowin
	     	D+=Dstep;
                SDL_SetRenderDrawColor(ren,255,255,255,255);
	        SDL_RenderFillRect(ren,&sub_v);
                SDL_SetRenderDrawColor(ren,0,0,0,255);
		draw_fun(ren);
             break;
            }
       }
      break;     
   case SDL_KEYDOWN:
     if(myevent.key.keysym.sym == SDLK_ESCAPE)
   	done = 1;
   break; 
   }/* fine switch */ 	
   SDL_EventState(0xff, SDL_ENABLE);
 }
} /* fine while */

TTF_CloseFont (font);
TTF_Quit ();
SDL_DestroyRenderer(ren);
SDL_DestroyWindow(win);
SDL_Quit ();
return (0);

} /* fine main */

