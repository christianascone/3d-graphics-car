#include <math.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL2/SDL.h>
#define MAX_DEG 7
#define MAX_PT 101
#define MAX_COL 8

float viewAlpha=20, viewBeta=40; // angoli che definiscono la vista
float eyeDist=8.0; // distanza dell'occhio dall'origine
int scrH=500, scrW=500; // altezza e larghezza viewport (in pixels)
GLint r[MAX_COL],g[MAX_COL],b[MAX_COL];
int decast=0;
typedef struct demobezierC
{
 int     deg;                         /* grado della curva */
 double  xcp[MAX_DEG][MAX_DEG];       /* coefficienti */
 double  ycp[MAX_DEG][MAX_DEG];       /* coefficienti */
 double  zcp[MAX_DEG][MAX_DEG];       /* coefficienti */
}DemoBezierC;
typedef struct bezierC
{
 int     deg;                /* grado della curva */
 double  xcp[MAX_DEG];       /* coefficienti */
 double  ycp[MAX_DEG];       /* coefficienti */
 double  zcp[MAX_DEG];       /* coefficienti */
}BezierC;

/*************************************************************
calcola i valore di una curva polinomiale nella base di Bernstein
definita in [a,b] dai punti di controllo nei punti t mediante
l’algoritmo di de Casteljau;
p->xcp,p->ycp,p->zcp: punti di controllo della curva polinomiale
t: punti di valutazione nell'intervallo parametrico
x,y,z: valori della curva nei punti di valutazione
np: numero di punti di valutazione
*************************************************************/
void bezier_values(BezierC *p, double *t, double *x, double *y, double *z, int np)
{
int i,j,k;
double vt,umvt;
double xcp[MAX_DEG], ycp[MAX_DEG], zcp[MAX_DEG];   /* per copia coefficienti */

for (j=0; j<np; j++)
{
 for(i=0; i<=p->deg; i++)
 {
   xcp[i]=p->xcp[i];
   ycp[i]=p->ycp[i];
   zcp[i]=p->zcp[i];
 }
 vt=t[j];
 umvt=1.0-vt;
 for (k=0;k<p->deg;k++)
 {
  for (i=0; i<p->deg-k;i++)
  {
   xcp[i]=xcp[i] * umvt + xcp[i+1] * vt;
   ycp[i]=ycp[i] * umvt + ycp[i+1] * vt;
   zcp[i]=zcp[i] * umvt + zcp[i+1] * vt;
  }
 }
 x[j]=xcp[0];
 y[j]=ycp[0];
 z[j]=zcp[0];
 }
}

/*************************************************************
calcola il valore di una curva polinomiale nella base di Bernstein
definita in [a,b] dai punti di controllo nel punto t mediante
l’algoritmo di de Casteljau;
p->xcp,p->ycp,p->zcp: punti di controllo della curva polinomiale
t: punto di valutazione
x,y,z: valore della curva nel punto di valutazione
ATTENZIONE: i punti di controllo vengono modificati
*************************************************************/
void demo_bezier_value(DemoBezierC *p, double t, double *x, double *y, double *z)
{
int i,k,kp1;
double vt,umvt;

vt=t;
umvt=1.0-t;
for (k=0;k<p->deg;k++)
{
 kp1=k+1;
 for (i=0; i<p->deg-k;i++)
 {
  p->xcp[kp1][i]=p->xcp[k][i] * umvt + p->xcp[k][i+1] * vt;
  p->ycp[kp1][i]=p->ycp[k][i] * umvt + p->ycp[k][i+1] * vt;
  p->zcp[kp1][i]=p->zcp[k][i] * umvt + p->zcp[k][i+1] * vt;
 }
}
 *x=p->xcp[k][0];
 *y=p->ycp[k][0];
 *z=p->zcp[k][0];
}

// disegna gli assi nel sist. di riferimento
void drawAxis(){
const float K=0.10;
  glBegin(GL_LINES);
    glColor3f(1,0,0);
    glVertex3f( 0,0,0 );
    glVertex3f( +1,0,0 );

    glColor3f(0,1,0);
    glVertex3f( 0,0,0 );
    glVertex3f( 0,+1,0 );

    glColor3f(0,0,1);
    glVertex3f( 0,0,0 );
    glVertex3f( 0,0,+1 );
  glEnd();
 
  glBegin(GL_TRIANGLES);
    glColor3f(0,1,0);
    glVertex3f( 0,+1  ,0 );
    glVertex3f( K,+1-K,0 );
    glVertex3f(-K,+1-K,0 );

    glColor3f(1,0,0);
    glVertex3f( +1,   0, 0 );
    glVertex3f( +1-K,+K, 0 );
    glVertex3f( +1-K,-K, 0 );

    glColor3f(0,0,1);
    glVertex3f( 0, 0,+1 );
    glVertex3f( 0,+K,+1-K );
    glVertex3f( 0,-K,+1-K );
  glEnd();

}

// disegna un cubo in wireframe
void drawCubeWire()
{
  glBegin(GL_LINE_LOOP); // faccia z=+1
    glVertex3f( +1,+1,+1 );
    glVertex3f( -1,+1,+1 );
    glVertex3f( -1,-1,+1 );
    glVertex3f( +1,-1,+1 );
  glEnd();

  glBegin(GL_LINE_LOOP); // faccia z=-1
    glVertex3f( +1,-1,-1 );
    glVertex3f( -1,-1,-1 );
    glVertex3f( -1,+1,-1 );
    glVertex3f( +1,+1,-1 );
  glEnd();

  glBegin(GL_LINES);  // 4 segmenti da -z a +z
    glVertex3f( -1,-1,-1 );
    glVertex3f( -1,-1,+1 );

    glVertex3f( +1,-1,-1 );
    glVertex3f( +1,-1,+1 );

    glVertex3f( +1,+1,-1 );
    glVertex3f( +1,+1,+1 );

    glVertex3f( -1,+1,-1 );
    glVertex3f( -1,+1,+1 );
  glEnd();
}

// disegna curva di Bezier
void drawCurve(BezierC *p, double *tt,double *xp, double *yp, double *zp, int np){
int i;
//double x,y,z;

  glColor3f(1,0,0);
 
  // sono nel frame Curve
  glPushMatrix();

//valutazione punti curva
//  bezier_values(p, tt, xp, yp, zp, np);

  glTranslatef(0 , 0 , -0.5);
  glScalef(0.75 , 0.75 , 0.75);

//Disegna poligonale di controllo
/**/
  glBegin(GL_LINES);
  glColor3f(1,0,0);
  for (i=0;i<p->deg;i++)
    {
        glVertex3f( p->xcp[i],p->ycp[i],p->zcp[i] );
        glVertex3f( p->xcp[i+1],p->ycp[i+1],p->zcp[i+1] );
//printf("%d %d %f %f %f %f %f %f\n",k,i,p->xcp[i],p->ycp[i],p->zcp[i],p->xcp[i+1],p->ycp[i+1],p->zcp[i+1]);
    }
  glEnd();
/**/
//Disegna curva
  glBegin(GL_LINE_STRIP);
  glColor3f(0,0,0);
  for (i=0; i<np; i++)
   glVertex3f( xp[i],yp[i],zp[i] );
  glEnd();
 
  // torno al frame Cube
  glPopMatrix();
 
}

// disegna Curva di Bezier simulando algoritmo di de Casteljau
void drawDemoCurve(DemoBezierC *p, double *tt,double *xp, double *yp, double *zp, int j){
int i,k;
double x,y,z;

  glColor3f(1,0,0);
 
  // sono nel frame Curve
  glPushMatrix();

//valutazione punto curva
  demo_bezier_value(p, tt[j], &x, &y, &z);
  xp[j]=x; yp[j]=y; zp[j]=z;

  glTranslatef(0 , 0 , -0.5);
  glScalef(0.75 , 0.75 , 0.75);

  glBegin(GL_LINES);
  for (k=0;k<p->deg;k++)
  {
   glColor3f(r[k],g[k],b[k]);
   for (i=0;i<p->deg-k;i++)
    {
        glVertex3f( p->xcp[k][i],p->ycp[k][i],p->zcp[k][i] );
        glVertex3f( p->xcp[k][i+1],p->ycp[k][i+1],p->zcp[k][i+1] );
//printf("%d %d %f %f %f %f %f %f\n",k,i,p->xcp[k][i],p->ycp[k][i],p->zcp[k][i],p->xcp[k][i+1],p->ycp[k][i+1],p->zcp[k][i+1]);
    }
  }
  glEnd();
  glBegin(GL_LINE_STRIP);
  glColor3f(r[7],g[7],b[7]);
  for (i=0; i<=j; i++)
   glVertex3f( xp[i],yp[i],zp[i] );
  glEnd();
 
  // torno al frame Cube
  glPopMatrix();
 
}

/* Esegue il Rendering della scena */
void rendering(SDL_Window *win,DemoBezierC *dp,BezierC *p,double *t,double *xp,double *yp,double *zp,int np,int i){
 
  // linee spesse
  glLineWidth(3);
 
  // settiamo il viewport
  glViewport(0, 0, scrW, scrH);
 
  // colore sfondo = bianco
  glClearColor(1,1,1,1);

  // settiamo la matrice di proiezione
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  gluPerspective( 60, //fovy,
		((float)scrW) / scrH,//aspect Y/X,
		1,//zNear,
		100//zFar
		);

  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();

  // riempe tutto lo screen buffer di pixel color sfondo
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
 
  //drawAxis(); // disegna assi frame VISTA
 
  // settiamo matrice di vista
  glTranslatef(0,0,-eyeDist);
  glRotatef(viewBeta,  1,0,0);
  glRotatef(viewAlpha, 0,1,0);

  //drawAxis(); // disegna assi frame MONDO
 
  // settiamo matrice di modellazione
//  glScalef(1.5,1.5,1.5);
 
  drawAxis(); // disegna assi frame OGGETTO
  //disegna il cubo base
  glScalef(2.0,2.0,2.0);
  drawCubeWire();
  if (decast)
    drawDemoCurve(dp,t,xp,yp,zp,i);
  else
    drawCurve(p,t,xp,yp,zp,np);

  // attesa fine primitive mandate
  glFinish();
 
  SDL_GL_SwapWindow(win);
}

void redraw(){
  // ci automandiamo un messaggio che (s.o. permettendo)
  // ci fara' ridisegnare la finestra
  SDL_Event e;
  e.type=SDL_WINDOWEVENT;
  e.window.event=SDL_WINDOWEVENT_EXPOSED;
  SDL_PushEvent(&e);
}

int SetOpenGLAttributes()
{
//SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 2 );
SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 );
return(1);
}

int main(int argc, char* argv[])
{
const int np=MAX_PT;
SDL_Window *win;
SDL_Event e;
SDL_GLContext mainContext;
Uint32 windowID;
DemoBezierC dp;
BezierC p;
int i,done;
double t[MAX_PT],xp[MAX_PT],yp[MAX_PT],zp[MAX_PT];
Uint32 ms=100;

//inizializza vettori colore
  r[0]=1; g[0]=0; b[0]=0;
  r[1]=0; g[1]=1; b[1]=0;
  r[2]=0; g[2]=0; b[2]=1;
  r[3]=1; g[3]=1; b[3]=0;
  r[4]=0; g[4]=1; b[4]=1;
  r[5]=1; g[5]=0; b[5]=1;
  r[6]=1; g[6]=1; b[6]=1;

// inizializzazione di SDL
  SDL_Init( SDL_INIT_VIDEO );

  win= SDL_CreateWindow("Bezier 3D", 0, 0, scrW, scrH, SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);
  if(win==NULL){
        fprintf(stderr,"SDL_CreateWindow Error: %s\n",SDL_GetError());
        SDL_Quit();
        return 1;
  }

  SetOpenGLAttributes();
//Create our opengl context and attach it to our window
  mainContext=SDL_GL_CreateContext(win);

 
  glEnable(GL_DEPTH_TEST);

//inizializzazione struttura BezierC: grado e punti di controllo
  p.deg=6;
  p.xcp[0]=1; p.ycp[0]=0; p.zcp[0]=0;
  p.xcp[1]=1; p.ycp[1]=1; p.zcp[1]=0;
  p.xcp[2]=1; p.ycp[2]=1; p.zcp[2]=2;
  p.xcp[3]=-1; p.ycp[3]=1; p.zcp[3]=2;
  p.xcp[4]=-1; p.ycp[4]=-1; p.zcp[4]=2;
  p.xcp[5]=-1; p.ycp[5]=-1; p.zcp[5]=0;
  p.xcp[6]=-1; p.ycp[6]=0; p.zcp[6]=0;
//copia curva in DemoBezierC 
  dp.deg=p.deg;
  for (i=0; i<=dp.deg; i++)
  {
   dp.xcp[0][i]=p.xcp[i];
   dp.ycp[0][i]=p.ycp[i];
   dp.zcp[0][i]=p.zcp[i];
  }

//inizializzaione punti valutazione
  for (i=0; i<np; i++)
    t[i]=(double)i/(np-1);
 
//valutazione punti curva
  bezier_values(&p, t, xp, yp, zp, np);

  i=0;
  done=0;
  while (!done) {
    // guardo se c'e' un evento:
    if (SDL_PollEvent(&e)) {
     // se si: processa evento
     switch (e.type) {
      case SDL_KEYDOWN:
        if (e.key.keysym.sym==SDLK_F1) decast=!decast;
        if (e.key.keysym.sym==SDLK_F2)
        {
         if (p.deg<6)
            p.deg++;
          else
            p.deg=2;
         i=0;
         dp.deg=p.deg;
         bezier_values(&p, t, xp, yp, zp, np);
        }
        if (e.key.keysym.sym==SDLK_F3) ms=100-ms;
        break;
      case SDL_QUIT: 
          done=1;   break;
      case SDL_WINDOWEVENT:
          windowID = SDL_GetWindowID(win);
          if (e.window.windowID == windowID)  {
            switch (e.window.event)  {
                  case SDL_WINDOWEVENT_SIZE_CHANGED:
                     scrW = e.window.data1;
                     scrH = e.window.data2;
                     glViewport(0,0,scrW,scrH);
                     //commentare/scommentare
	             rendering(win,&dp,&p,t,xp,yp,zp,np,i);
                     //redraw(); // richiedi ridisegno
                     break;
                  case SDL_WINDOWEVENT_EXPOSED:
                     // dobbiamo ridisegnare la finestra
                     rendering(win,&dp,&p,t,xp,yp,zp,np,i);
                     break;
           }
         }
      case SDL_MOUSEMOTION:
        if (e.motion.state & SDL_BUTTON(1) ) {
          viewAlpha+=e.motion.xrel;
          viewBeta +=e.motion.yrel;
          if (viewBeta<-90) viewBeta=-90;
          if (viewBeta>+90) viewBeta=+90;
          //commentare/scommentare
	  rendering(win,&dp,&p,t,xp,yp,zp,np,i);
          //redraw(); // richiedi ridisegno
        }
        break;

     case SDL_MOUSEWHEEL:
       if (e.wheel.y < 0 ) {
         // avvicino il punto di vista (zoom in)
         eyeDist=eyeDist*0.9;
         if (eyeDist<1) eyeDist = 1;
       }
       if (e.wheel.y > 0 ) {
         // allontano il punto di vista (zoom out)
         eyeDist=eyeDist/0.9;
       }
     break;
     }
    } else {
      // nessun evento: siamo IDLE
      if(i+1>np-1)
        i=0;
      else
        i++;
      SDL_Delay(ms);
      rendering(win,&dp,&p,t,xp,yp,zp,np,i);
      //redraw(); // richiedi ridisegno
    }
  }

SDL_GL_DeleteContext(mainContext);
SDL_DestroyWindow(win);
SDL_Quit ();
return (0);
}
