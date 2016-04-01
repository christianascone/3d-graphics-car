#include <stdio.h>
#include <math.h>
#include "../GCGraLib2/GCGraLib2.h"

#define MAXVERT (100)
#define MAXEDGE (200)
#define MAXFACE (100)
#define DEFAULT_PTSIZE  12

/* Dati per visualizzare un oggetto definito solo da vertici e lati*/
float   x[MAXVERT],y[MAXVERT],z[MAXVERT];       /* coordinate vertici */
int     edge[MAXEDGE][2];                       /* lista lati */
int     face[MAXFACE][4];                       /* lista facce */
float   nx[MAXFACE],ny[MAXFACE],nz[MAXFACE];    /* coordinate vertici */
int     red[MAXVERT],green[MAXVERT],blue[MAXVERT]; /* componenti colori ai vertici */
int     nvert,nedge,nface;                      /* numero vertici, lati e facce */
float   csx,csy,csz;                            /* centro oggetto */
float   D = 25,teta = 20,fi = 20,di;
int     xs[MAXVERT],ys[MAXVERT];                /* coordinate schermo */
float   zeye[MAXVERT];                          /* coordinate z trasformate */

int 	xvmin,xvmax,yvmin,yvmax;	/* coordinate viewport */
float   salpha=0.05;			/* zoomin e zoomout */
float   sr=0.25,ssx,ssy,ssz,ssmod,      /* modifica centro oggetto */
	Dstep=1.0,			/* passo di incremento D */
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

RECT  menu[15];
int   done=0;


/* inizializzazione menu */
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
 
float rad(float angle)
{
    return(angle/180*3.1415);
}

void copy_trasl(int nv,int *nvert,float x[],float y[],float z[],
                int ne,int *nedge,int edge[][2],float dx,float dy,float dz)
{
int i;

 for (i=0; i<nv; i++)
 {
   x[i+*nvert]=x[i]+dx;
   y[i+*nvert]=y[i]+dy;
   z[i+*nvert]=z[i]+dz;
 }
 for (i=0; i<=ne; i++)
 {
   edge[i+*nedge][0]=edge[i][0]+*nvert;
   edge[i+*nedge][1]=edge[i][1]+*nvert;
 }
 *nvert+=nv;
 *nedge+=ne;
}

/* Calcola la normale ad una faccia */
void compute_face_normal(int k)
{
int i1,i2,i3;
float v[3],w[3];

  i1=face[k][0]; i2=face[k][1]; i3=face[k][2];
  v[0]=x[i2]-x[i1]; v[1]=y[i2]-y[i1]; v[2]=z[i2]-z[i1];
  w[0]=x[i3]-x[i2]; w[1]=y[i3]-y[i2]; w[2]=z[i3]-z[i2];
  nx[k]=v[1]*w[2]-v[2]*w[1];
  ny[k]=v[2]*w[0]-v[0]*w[2];
  nz[k]=v[0]*w[1]-v[1]*w[0];
//  printf("%d %f %f %f \n",k,nx[k],ny[k],nz[k]);
}

/* funzione che definisce l'oggetto mesh */
void define_object()
{
int k;

/* Determino le coordinate dei vertici dell'oggetto */
nvert=8;
x[0]=-1.0; y[0]=-1.0; z[0]=-1.0;
x[1]=1.0; y[1]=-1.0; z[1]=-1.0;
x[2]=1.0;y[2]=1.0;z[2]=-1.0;
x[3]=-1.0;y[3]=1.0;z[3]=-1.0;
x[4]=-1.0;y[4]=-1.0;z[4]=1.0;
x[5]=1.0;y[5]=-1.0;z[5]=1.0;
x[6]=1.0;y[6]=1.0;z[6]=1.0;
x[7]=-1.0;y[7]=1.0;z[7]=1.0;

/* Determino la lista dei lati dell'oggetto */
nedge=13;
edge[0][0]=0;edge[0][1]=1;
edge[1][0]=1;edge[1][1]=2;
edge[2][0]=2;edge[2][1]=3;
edge[3][0]=3;edge[3][1]=0;
edge[4][0]=4;edge[4][1]=5;
edge[5][0]=5;edge[5][1]=6;
edge[6][0]=6;edge[6][1]=7;
edge[7][0]=7;edge[7][1]=4;
edge[8][0]=0;edge[8][1]=4;
edge[9][0]=1;edge[9][1]=5;
edge[10][0]=2;edge[10][1]=6;
edge[11][0]=3;edge[11][1]=7;
/*diagonale faccia*/
edge[12][0]=1;edge[12][1]=6;

/*Determino la lista delle facce dell'oggetto */
nface=6;
face[0][0]=0; face[0][1]=1; face[0][2]=5; face[0][3]=4;
face[1][0]=1; face[1][1]=2; face[1][2]=6; face[1][3]=5;
face[2][0]=2; face[2][1]=3; face[2][2]=7; face[2][3]=6;
face[3][0]=3; face[3][1]=0; face[3][2]=4; face[3][3]=7;
face[4][0]=4; face[4][1]=5; face[4][2]=6; face[4][3]=7;
face[5][0]=3; face[5][1]=2; face[5][2]=1; face[5][3]=0;

/*Determino la normale alle facce */
for (k=0; k<nface; k++)
  compute_face_normal(k);

/*Definisco componenti colore dei vertici dell'oggetto */
red[0]=0; green[0]=0; blue[0]=0;
red[1]=255; green[1]=0; blue[1]=0;
red[2]=255;green[2]=255;blue[2]=0;
red[3]=0;green[3]=255;blue[3]=0;
red[4]=0;green[4]=0;blue[4]=255;
red[5]=255;green[5]=0;blue[5]=255;
red[6]=255;green[6]=255;blue[6]=255;
red[7]=0;green[7]=255;blue[7]=255;

//copy_trasl(8,&nvert,x,y,z,13,&nedge,edge,2.0,0.0,0.0);
}

/* trasformazione prospettica a tre punti di fuga (generico punto di vista) */
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

/* funzione che determina i fattori di scala */
void define_view()
{
int     k;
float   s,r;
float   xmin,xmax,ymin,ymax,zmin,zmax;

  xmin = x[0];
  ymin = y[0];
  zmin = z[0];
  xmax = xmin;
  ymax = ymin;
  zmax = zmin;
  for (k=1;k<nvert;k++)
  {
        if (x[k] > xmax) xmax = x[k];
                else if (x[k]<xmin) xmin = x[k];
        if (y[k] > ymax) ymax = y[k];
                else if (y[k]<ymin) ymin = y[k];
        if (z[k] > zmax) zmax = z[k];
                else if (z[k]<zmin) zmin = z[k];
  }
/* centro oggetto iniziale */
  csx = (xmin + xmax)/2;
  csy = (ymin + ymax)/2;
  csz = (zmin + zmax)/2;
/* raggio sfera contenete oggetto */
  r = sqrt(powf(xmax-csx,2)+powf(ymax-csy,2)+powf(zmax-csz,2));
/* calcolo semilato window iniziale; il piano di proiezione viene assunto
   ad una distanza D dall'osservatore */
  s = r*D/sqrt(powf(D,2)-powf(r,2));
/* in base al semilato window cosi' calcolato si determina l'apertura
   angolare iniziale */
  alpha=atan(s/D);
/* printf("\nApertura angolare: %f \n",alpha);*/

}

void conv(unsigned long m, int b, int a[])
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

int max(int a,int b,int c)
{
int m;
  m=a;
  if (b>m) m=b;
  if (c>m) m=c;
  return m;
}
int min(int a,int b,int c)
{
int m;
  m=a;
  if (b<m) m=b;
  if (c<m) m=c;
  return m;
} 

float f01(float x0,float y0,float x1,float y1,float x,float y)
{
  return (y0-y1)*x+(x1-x0)*y+x0*y1-x1*y0;
}

void draw_color(SDL_Renderer *ren, int i1,int i2,int i3,int i4)
{
int x0,y0,x1,y1,x2,y2;
float z0,z1,z2,unosuz;
int xmin,xmax,ymin,ymax;
int ix,iy,ir,ig,ib;
float alpha, beta, gamma;
int k;
int r0=255, g0=0, b0=0, r1=0, g1=255, b1=0, r2=0, g2=0, b2=255;

/*ogni faccia quadrata viene divisa in due triangoli */
   for (k=0; k<2; k++)
   {
    if (k==0)
    {
     /*triangolo schermo*/
     x0=xs[i4];y0=ys[i4];z0=zeye[i4];
     x1=xs[i3];y1=ys[i3];z1=zeye[i3];
     x2=xs[i1];y2=ys[i1];z2=zeye[i1];
     r0=red[i4]; g0=green[i4]; b0=blue[i4];
     r1=red[i3]; g1=green[i3]; b1=blue[i3];
     r2=red[i1]; g2=green[i1]; b2=blue[i1];
    }
    else
    {
     /*triangolo schermo*/
     x0=xs[i3];y0=ys[i3];z0=zeye[i3];
     x1=xs[i2];y1=ys[i2];z1=zeye[i2];
     x2=xs[i1];y2=ys[i1];z2=zeye[i1];
     /*colori per essere adiacenti al primo triangolo */
     r0=red[i3]; g0=green[i3]; b0=blue[i3];
     r1=red[i2]; g1=green[i2]; b1=blue[i2];
     r2=red[i1]; g2=green[i1]; b2=blue[i1];
    }
    /*extent triangolo schermo*/
    xmin=min(x0,x1,x2);
    xmax=max(x0,x1,x2);
    ymin=min(y0,y1,y2);
    ymax=max(y0,y1,y2);
    for (ix=xmin; ix<=xmax; ix++)
      for (iy=ymin; iy<=ymax; iy++)
       {
             alpha=f01(x1,y1,x2,y2,ix,iy)/f01(x1,y1,x2,y2,x0,y0);
             beta=f01(x2,y2,x0,y0,ix,iy)/f01(x2,y2,x0,y0,x1,y1);
             gamma=f01(x0,y0,x1,y1,ix,iy)/f01(x0,y0,x1,y1,x2,y2);
             if (alpha>=0 && beta>=0 && gamma>=0)
             {
/*Stima il colore interpolando linearmente e tenendo conto della proiezione
 che si effettua; se scommentiamo queste 3 linee e si commentano le successive 5
 si avra' un problema sul colore dell' immagine proiettata, anche se difficile da vedre */
               /*
               ir=(int)(alpha*r0+beta*r1+gamma*r2);
               ig=(int)(alpha*g0+beta*g1+gamma*g2);
               ib=(int)(alpha*b0+beta*b1+gamma*b2);
               */
               unosuz=alpha/z0+beta/z1+gamma/z2;
               ir=(int)((alpha*r0/z0+beta*r1/z1+gamma*r2/z2)/unosuz);
               ig=(int)((alpha*g0/z0+beta*g1/z1+gamma*g2/z2)/unosuz);
               ib=(int)((alpha*b0/z0+beta*b1/z1+gamma*b2/z2)/unosuz);
//               ic=ir*256*256+ig*256+ib;
               SDL_SetRenderDrawColor(ren,ir,ig,ib,255);

               SDL_RenderDrawPoint(ren, ix, iy);
             }
       }
    }
}

void draw_mesh(SDL_Renderer *ren)
{
int t,u,k,init;
float xe,ye,ze;    /* coord. dell'osservatore */
int i;
float vd[3],vp[3];

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
for (k=0;k<nvert;k++)
 {
        trasf_prosp_gen(&init,x[k]-csx,y[k]-csy,z[k]-csz,&xe,&ye,&ze);
/* proiezione e trasformazione in coordinate schermo */
        xs[k] = (int)(Sx * ((di * xe)/ze - xwmin) + xvmin + 0.5);
        ys[k] = (int)(Sy * (ywmin - (di * ye)/ze) + yvmax + 0.5);
        zeye[k] = ze;
 }

/*direzione di vista per poter applicare il back face culling */
vp[0]=D*sin(fi)*cos(teta);
vp[1]=D*sin(fi)*sin(teta);
vp[2]=D*cos(fi);

/* disegno facce visibili */
for (k = 0;k < nface ;k++)
{
  t=face[k][0];
  vd[0]=x[t]-vp[0];
  vd[1]=y[t]-vp[1];
  vd[2]=z[t]-vp[2];
  if (nx[k]*vd[0]+ny[k]*vd[1]+nz[k]*vd[2] < 0)
    draw_color(ren,face[k][0],face[k][1],face[k][2],face[k][3]);
}

SDL_SetRenderDrawColor(ren,0,0,0,255);
/* disegno lati di ogni faccia visibile */
for (k = 0;k < nface ;k++)
{
  t=face[k][0];
  vd[0]=x[t]-vp[0];
  vd[1]=y[t]-vp[1];
  vd[2]=z[t]-vp[2];
  if (nx[k]*vd[0]+ny[k]*vd[1]+nz[k]*vd[2] < 0)
  {
    t=face[k][3];
    u=face[k][0];
    for (i=0; i<4; i++)
    {
	SDL_RenderDrawLine(ren, xs[t],ys[t],xs[u],ys[u]);
        t=face[k][i];
        u=face[k][i+1];
    }
  }
}
/*printf("\n [D,teta,fi]= %f %f %f",D,teta,fi);*/
  SDL_RenderPresent(ren);
}

int main()
{
SDL_Window *win;
SDL_Renderer *ren;
SDL_Rect sub_v, v, bsub_v;
TTF_Font *font;
SDL_Event myevent;
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
sub_v.x = xvmin;
sub_v.y = yvmin;
sub_v.w = xvmax;
sub_v.h = yvmax;
bsub_v.x=sub_v.x-2;
bsub_v.y=sub_v.y-2;
bsub_v.w=sub_v.w+4;
bsub_v.h=sub_v.h+4;

  win= SDL_CreateWindow("View Cube Model", 0, 0, v.w, v.h, SDL_WINDOW_RESIZABLE);
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
define_object();   /* determinazione mesh oggetto */
define_view();
draw_mesh(ren);

//Disegnare tutte le volte, anche per eventi intermedi,
//rallenta tutto senza dare un contributo all'immagine finale.
//Gli eventi si possono scremare in questo modo:
//viene usata la costante 0xff perchè nel codice sorgente delle SDL è usato
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
                draw_menu(menu,ren,font);
                SDL_SetRenderDrawColor(ren,255,255,255,255);
		SDL_RenderFillRect(ren,&sub_v);
		draw_mesh(ren);
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
                draw_menu(menu,ren,font);
                SDL_SetRenderDrawColor(ren,255,255,255,255);
		SDL_RenderFillRect(ren,&sub_v);
  		fi = atan2(tmpy,tmpx);
  		draw_mesh(ren);
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
//                SDL_SetRenderDrawColor(ren,0,0,0,255);
		draw_mesh(ren);
	    break;
	    case 8: //choice == leftwin
                SDL_SetRenderDrawColor(ren,255,255,255,255);
                SDL_RenderFillRect(ren,&sub_v);
		ssx=-sin(teta);
		ssy=cos(teta);
		csx+=sr*ssx;
		csy+=sr*ssy;
//                SDL_SetRenderDrawColor(ren,0,0,0,255);
		draw_mesh(ren);
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
//                SDL_SetRenderDrawColor(ren,0,0,0,255);
		draw_mesh(ren);
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
//                SDL_SetRenderDrawColor(ren,0,0,0,255);
		draw_mesh(ren);
	     break;
	     case 3: //choice == ziwin
		if (alpha-salpha >0)
  		   alpha-=salpha;
                SDL_SetRenderDrawColor(ren,255,255,255,255);
                SDL_RenderFillRect(ren,&sub_v);
//                SDL_SetRenderDrawColor(ren,0,0,0,255);
 		draw_mesh(ren);
             break;
	     case 4: //choice == zowin
	        if (alpha+salpha <1.57)
  		   alpha+=salpha;
                SDL_SetRenderDrawColor(ren,255,255,255,255);
                SDL_RenderFillRect(ren,&sub_v);
//                SDL_SetRenderDrawColor(ren,0,0,0,255);
		draw_mesh(ren);
             break;
	     case 5: //choice == Dziwin
	        if (D-Dstep>0)
	     	  D-=Dstep;
                SDL_SetRenderDrawColor(ren,255,255,255,255);
                SDL_RenderFillRect(ren,&sub_v);
//                SDL_SetRenderDrawColor(ren,0,0,0,255);
  		draw_mesh(ren);
             break;
	     case 6: //choice == Dzowin
	     	D+=Dstep;
                SDL_SetRenderDrawColor(ren,255,255,255,255);
                SDL_RenderFillRect(ren,&sub_v);
//                SDL_SetRenderDrawColor(ren,0,0,0,255);
		draw_mesh(ren);
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

