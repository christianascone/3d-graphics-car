#include <stdio.h>
#include <math.h>
#include <GL/gl.h>
#include <GL/glu.h>
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
uint	font_id=-1;

typedef struct RECT RECT;
struct RECT
{
  SDL_Rect rect;
  char     text[10];
  int      ox,oy;
};

/* Dati per la costruzione delle finestre */
RECT            menu[15];
int             done=0;

void SDL_GL_DrawText(TTF_Font *font,char fgR, char fgG, char fgB, char fgA,
              char bgR, char bgG, char bgB, char bgA, char *text, int x, int y,
	                    enum textquality quality)
{
	SDL_Color tmpfontcolor = {fgR,fgG,fgB,fgA};
	SDL_Color tmpfontbgcolor = {bgR, bgG, bgB, bgA};\
	SDL_Surface *initial;
	SDL_Surface *intermediary;
	SDL_Rect location;
	int w,h;

/* Use SDL_TTF to render our text */
	initial=NULL;
	if (quality == solid)
	  initial = TTF_RenderText_Solid(font, text, tmpfontcolor);
	else
	  if (quality == shaded)
	     initial = TTF_RenderText_Shaded(font, text, tmpfontcolor, tmpfontbgcolor);
          else
            if (quality == blended)
               initial = TTF_RenderText_Blended(font, text, tmpfontcolor);

/* Convert the rendered text to a known format */
	w = initial->w;
	h = initial->h;

	intermediary = SDL_CreateRGBSurface(0, w, h, 32,
		0x000000ff,0x0000ff00, 0x00ff0000,0xff000000);

        SDL_BlitSurface(initial, 0, intermediary, 0);

/* Tell GL about our new texture */
	glBindTexture(GL_TEXTURE_2D, font_id);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, w, h, 0, GL_RGBA,
			GL_UNSIGNED_BYTE, intermediary->pixels );

/* GL_NEAREST looks horrible, if scaled... */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

/* prepare to render our texture */
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, font_id);
	glColor3f(1.0f, 1.0f, 1.0f);

        if ( initial != NULL )
        {
         location.x = x;
         location.y = y;
        }

/* Draw a text in un quad */
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 1.0f);
		glVertex2f((GLfloat)location.x, (GLfloat)location.y);
		glTexCoord2f(1.0f, 1.0f);
		glVertex2f((GLfloat)location.x + w, (GLfloat)location.y);
		glTexCoord2f(1.0f, 0.0f);
		glVertex2f((GLfloat)location.x + w, (GLfloat)location.y + h);
		glTexCoord2f(0.0f, 0.0f);
		glVertex2f((GLfloat)location.x    , (GLfloat)location.y + h);
	glEnd();

/* Draw a quad at location */
	glColor3f(0.0f, 0.0f, 0.0f);
	glBegin(GL_LINE_STRIP);
		glVertex2f((GLfloat)location.x-1, (GLfloat)location.y-1);
		glVertex2f((GLfloat)location.x + w +1, (GLfloat)location.y-1);
		glVertex2f((GLfloat)location.x + w +1, (GLfloat)location.y + h +1);
		glVertex2f((GLfloat)location.x-1    , (GLfloat)location.y + h +1);
		glVertex2f((GLfloat)location.x-1, (GLfloat)location.y-1);
	glEnd();

/* Bad things happen if we delete the texture before it finishes */
	glFinish();

/* return the deltas in the unused w,h part of the rect */
        location.w = initial->w;
	location.h = initial->h;

/* Clean up */
	glDisable(GL_TEXTURE_2D);
	SDL_FreeSurface(initial);
	SDL_FreeSurface(intermediary);
}

float sqr(float x)
{
        return(x*x);
}
        
/* inizializzazione X */
void init_menu(RECT menu[])
{
int i,w;

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

  for (i=0; i<=10; i++)
  {
    w=(DEFAULT_PTSIZE-4)*strlen(menu[i].text);
    menu[i].ox=(int)((menu[i].rect.w-w)/2.0+0.5);
    menu[i].oy=(int)((menu[i].rect.h-DEFAULT_PTSIZE)/2.0+0.5);
  }

}

void draw_menu(RECT menu[], TTF_Font *font)
{
int i,ox;

   for( i=0 ; i<=10 ; i++ )
   {
      ox=menu[i].ox;
      SDL_GL_DrawText(font, 0, 0, 0, 0, 255, 255, 255, 0, menu[i].text, 
                  menu[i].rect.x+ox, 570-menu[i].rect.y, shaded);
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
int 	i,j;
float 	r;
//float   xmin,xmax,ymin,ymax,zmin,zmax;

  zmin = z[1][1];
  zmax = zmin;
  for (i=0;i<=nx;i++)
	for (j=0;j<=ny;j++)
		if (z[i][j] > zmax) zmax = z[i][j];
		else if (z[i][j]<zmin) zmin = z[i][j];
/* centro oggetto iniziale */
  csx = (xmin + xmax)/2;
  csy = (ymin + ymax)/2;
  csz = (zmin + zmax)/2;
/* raggio sfera contenete oggetto */
  r = sqrt(sqr(xmax-csx)+sqr(ymax-csy)+sqr(zmax-csz));
/* calcolo semilato window iniziale; il piano di proiezione viene assunto
   ad una distanza D dall'osservatore */
  s = r*D/sqrt(sqr(D)-sqr(r));
/* in base al semilato window cosi' calcolato si determina l'apertura
   angolare iniziale */
  alpha=atan(s/D);
/* printf("\nApertura angolare: %f \n",alpha); */

}

void draw_fun()
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
/* proiezione e trasformazione in coordinate schermo dei punti */
for (i=0;i<=nx;i++)
   	for(j=0;j<=ny;j++)
        {
		trasf_prosp_gen(&init,x[i]-csx,y[j]-csy,z[i][j]-csz,&xe,&ye,&ze);
		xs[i][j] = (int)(Sx * ((di * xe)/ze - xwmin) + xvmin + 0.5);
//		ys[i][j] = (int)(Sy * (ywmin - (di * ye)/ze) + yvmax + 0.5);	
		ys[i][j] = (int)(Sy * ((di * ye)/ze - ywmin) + yvmin + 0.5);	
  	}

    glBegin(GL_LINES);

/* disegno mesh */
      for (t = 0;t <=nx;t++)
	for (u=1;u<=ny;u++)
	{
	  glVertex2f((GLfloat)xs[t][u-1],(GLfloat)ys[t][u-1]);
	  glVertex2f((GLfloat)xs[t][u],(GLfloat)ys[t][u]);
	}

      for (u=0;u<=ny;u++)
	for (t=1;t<=nx;t++)
	{
	  glVertex2f((GLfloat)xs[t-1][u],(GLfloat)ys[t-1][u]);
	  glVertex2f((GLfloat)xs[t][u],(GLfloat)ys[t][u]);
	}
   glEnd();
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
 
void display(SDL_Window *win,RECT menu[], TTF_Font *font, SDL_Rect v)
{
  glClearColor(1, 1, 1, 1);
  glClear(GL_COLOR_BUFFER_BIT);
  glColor3f(0.0, 0.0, 0.0);
//glLineWidth(3);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  gluOrtho2D((GLfloat)v.x,(GLfloat)v.w,(GLfloat)v.y,(GLfloat)v.h);

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  draw_menu(menu, font);
  glPopMatrix();

  draw_fun();
  SDL_GL_SwapWindow(win);
}

int SetOpenGLAttributes()
{
//SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 2 );
SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 );
return(1);
}

int main()
{
SDL_Window *win;
SDL_Rect v;
TTF_Font *font;
SDL_Event myevent;
SDL_GLContext mainContext;
int choice, done=0;

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

  win= SDL_CreateWindow("View Bivariate Function", 0, 0, v.w, v.h, SDL_WINDOW_OPENGL);
  if(win==NULL){
        fprintf(stderr,"SDL_CreateWindow Error: %s\n",SDL_GetError());
        SDL_Quit();
        return 1;
  }

SetOpenGLAttributes();
//Create our opengl context and attach it to our window
mainContext=SDL_GL_CreateContext(win);

init_menu(menu);
define_fun_grid();   /* griglia di approssimazione funzione bivariata*/
define_view(); /* calcolo dei parametri di vista */
display(win,menu,font,v);

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
                display(win,menu,font,v);
	 break;
	 case 2: //choice==fiwin 	  
  		tmpx = myevent.motion.x-(menu[2].rect.x+menu[2].rect.w/2);
  		tmpy = myevent.motion.y-(menu[2].rect.y+menu[2].rect.h/2);
  		fi = atan2(tmpy,tmpx);
                display(win,menu,font,v);
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
                ssx=-sin(teta);
		ssy=cos(teta);
		csx-=sr*ssx;
		csy-=sr*ssy;
                display(win,menu,font,v);
	    break;
	    case 8: //choice == leftwin
		ssx=-sin(teta);
		ssy=cos(teta);
		csx+=sr*ssx;
		csy+=sr*ssy;
                display(win,menu,font,v);
	    break;
	    case 9: //choice == upwin
		ssx=-cos(fi)*cos(teta);
		ssy=-cos(fi)*sin(teta);
		ssz=sin(fi);
		csx+=sr*ssx;
		csy+=sr*ssy;
		csz+=sr*ssz;
                display(win,menu,font,v);
	     break;	
	     case 10: //choice == downwin
		ssx=-cos(fi)*cos(teta);
		ssy=-cos(fi)*sin(teta);
		ssz=sin(fi);
		csx-=sr*ssx;
		csy-=sr*ssy;
		csz-=sr*ssz;
                display(win,menu,font,v);
	     break;
	     case 3: //choice == ziwin
		if (alpha-salpha >0)
  		   alpha-=salpha;
                display(win,menu,font,v);
             break;
	     case 4: //choice == zowin
	        if (alpha+salpha <1.57)
  		   alpha+=salpha;
                display(win,menu,font,v);
             break;
	     case 5: //choice == Dziwin
	        if (D-Dstep>0)
	     	  D-=Dstep;
                display(win,menu,font,v);
             break;
	     case 6: //choice == Dzowin
	     	D+=Dstep;
                display(win,menu,font,v);
             break;
            }
       }
     break;     
  case SDL_KEYDOWN:
    if(myevent.key.keysym.sym == SDLK_ESCAPE)
   	done = 1;
  break; 
  SDL_EventState(0xff, SDL_ENABLE);
  }/* fine switch */ 	
  SDL_EventState(0xff, SDL_ENABLE);
 }
} /* fine while */

TTF_CloseFont (font);
TTF_Quit ();
SDL_GL_DeleteContext(mainContext);
SDL_DestroyWindow(win);
SDL_Quit ();
return (0);

} /* fine main */

