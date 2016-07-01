#include <math.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL2/SDL.h>

float viewAlpha=20, viewBeta=40; // angoli che definiscono la vista
int scrH=500, scrW=500; // altezza e larghezza viewport (in pixels)
float mozzo=0;

// disegna gli assi nel sist. di riferimento
void drawAxis(){
const float K=0.10;
  glColor3f(0,0,1);
  glBegin(GL_LINES);
    glVertex3f( -1,0,0 );
    glVertex3f( +1,0,0 );

    glVertex3f( 0,-1,0 );
    glVertex3f( 0,+1,0 );

    glVertex3f( 0,0,-1 );
    glVertex3f( 0,0,+1.5 );
  glEnd();
 
  glBegin(GL_TRIANGLES);
    glVertex3f( 0,+1  ,0 );
    glVertex3f( K,+1-K,0 );
    glVertex3f(-K,+1-K,0 );

    glVertex3f( +1,   0, 0 );
    glVertex3f( +1-K,+K, 0 );
    glVertex3f( +1-K,-K, 0 );

    glVertex3f( 0, 0,+1.5 );
    glVertex3f( 0,+K,+1.5-K );
    glVertex3f( 0,-K,+1.5-K );
  glEnd();
}

// disegna un cubo rasterizzando quads
void drawCubeFill()
{
  glBegin(GL_QUADS);
 
    glVertex3f( +1,+1,+1 );
    glVertex3f( -1,+1,+1 );
    glVertex3f( -1,-1,+1 );
    glVertex3f( +1,-1,+1 );

    glVertex3f( +1,-1,-1 );
    glVertex3f( -1,-1,-1 );
    glVertex3f( -1,+1,-1 );
    glVertex3f( +1,+1,-1 );

    glVertex3f( +1,+1,+1 );
    glVertex3f( -1,+1,+1 );
    glVertex3f( -1,+1,-1 );
    glVertex3f( +1,+1,-1 );

    glVertex3f( +1,-1,-1 );
    glVertex3f( -1,-1,-1 );
    glVertex3f( -1,-1,+1 );
    glVertex3f( +1,-1,+1 );

    glVertex3f( +1,+1,+1 );
    glVertex3f( +1,-1,+1 );
    glVertex3f( +1,-1,-1 );
    glVertex3f( +1,+1,-1 );

    glVertex3f( -1,+1,-1 );
    glVertex3f( -1,-1,-1 );
    glVertex3f( -1,-1,+1 );
    glVertex3f( -1,+1,+1 );

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

void drawCube()
{
  glColor3f(.9,.9,.9);
  drawCubeFill();
  glColor3f(0,0,0);
  drawCubeWire();
}

// disegna carlinga composta da 1 cubo traslato e scalato
void drawCarlinga(){
 
  glColor3f(1,0,0);
 
  // sono nel frame CAR
  glPushMatrix();
 
  // vado al frame pezzo_A
  glScalef(0.25 , 0.14 , 1);
  drawCube();
 
  // torno al frame CAR
  glPopMatrix();
 
/*
// disegna altri 3 cubi traslati escalati per carlinga
  // vado frame pezzo_B
  glPushMatrix();
  glTranslatef(0,-0.11,-0.95);
  glScalef(0.6, 0.05, 0.15);
  drawCube();
  glPopMatrix();

  // vado frame pezzo_C
  glPushMatrix();
  glTranslatef(0,-0.11,0);
  glScalef(0.6, 0.05, 0.3);
  drawCube();
  glPopMatrix();

  // vado frame pezzo_D
  glPushMatrix();
  glRotatef(-5,1,0,0);
  glTranslatef(0,+0.2,+0.95);
  glScalef(0.6, 0.05, 0.3);
  drawCube();
  glPopMatrix();
*/
}

// disegna una macchina con le ruote sterzate
void drawCar(float mozzo){

  // disegna corpo macchina
  drawCarlinga();
 
  glColor3f(.4,.4,.4);
 
  // ruota posteriore D
  glPushMatrix();
  glTranslatef( 0.58,-0.05,+0.8);
  glRotatef(mozzo,1,0,0);
  // drawAxis(); // disegna assi frame ruota posteriore D
  glScalef(0.1, 0.20, 0.20);
  drawCube();
  glPopMatrix();
 
  // ruota posteriore S
  glPushMatrix();
  glTranslatef(-0.58,-0.05,+0.8);
  glRotatef(mozzo,1,0,0);
  // drawAxis(); // disegna assi frame ruota posteriore S
  glScalef(0.1, 0.20, 0.20);
  drawCube();
  glPopMatrix();

  // ruota anteriore D
  glPushMatrix();
  glTranslatef( 0.58,-0.05,-0.55);
  glRotatef(mozzo,1,0,0);
  // drawAxis(); // disegna assi frame ruota anteriore D
  glScalef(0.08, 0.15, 0.15);
  drawCube();
  glPopMatrix();
 
  // ruota anteriore S
  glPushMatrix();
  glTranslatef(-0.58,-0.05,-0.55);
  glRotatef(mozzo,1,0,0);
  drawAxis(); // disegna assi frame ruota anteriore S
  glScalef(0.08, 0.15, 0.15);
  drawCube();
  glPopMatrix();
}

/* Esegue il Rendering della scena */
void rendering(SDL_Window *win){
 
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
  glTranslatef(0,0,-3);
  glRotatef(viewBeta,  1,0,0);
  glRotatef(viewAlpha, 0,1,0);

  //drawAxis(); // disegna assi frame MONDO
 
  // settiamo matrice di modellazione
  glScalef(.5,.5,.5);
 
  drawAxis(); // disegna assi frame OGGETTO
  //disegna il cubo base anziche' la macchina
  //drawCubeWire();
  drawCar(mozzo);

  // attendiamo la fine della rasterizzazione di
  // tutte le primitive mandate
  glFinish();
 
  // ho finito: buffer di lavoro diventa visibile
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


int main(int argc, char* argv[])
{
SDL_Window *win;
SDL_GLContext mainContext;
Uint32 windowID;

  // inizializzazione di SDL
  SDL_Init( SDL_INIT_VIDEO );

  SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 );
  SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
 
  // facciamo una finestra di scrW x scrH pixels
  win=SDL_CreateWindow(argv[0], 0, 0, scrW, scrH, SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);

  //Create our opengl context and attach it to our window
  mainContext=SDL_GL_CreateContext(win);

  glEnable(GL_DEPTH_TEST);
 
  int done=0;
  while (!done) {
 
    SDL_Event e;
 
    // guardo se c'e' un evento:
    if (SDL_PollEvent(&e)) {
     // se si: processa evento
     switch (e.type) {
      case SDL_QUIT: 
          done=1;   
          break;
      case SDL_WINDOWEVENT:
          // dobbiamo ridisegnare la finestra
          if (e.window.event==SDL_WINDOWEVENT_EXPOSED)
            rendering(win);
          else{
          windowID = SDL_GetWindowID(win);
          if (e.window.windowID == windowID)  {
            switch (e.window.event)  {
                  case SDL_WINDOWEVENT_SIZE_CHANGED:  {
                     scrW = e.window.data1;
                     scrH = e.window.data2;
                     glViewport(0,0,scrW,scrH);
	             //rendering(win);
                     redraw(); // richiedi ridisegno
                     break;
                  }
            }
           }
          }
      break;
      case SDL_MOUSEMOTION:
        if (e.motion.state & SDL_BUTTON(1) ) {
          viewAlpha+=e.motion.xrel;
          viewBeta +=e.motion.yrel;
          if (viewBeta<-90) viewBeta=-90;
          if (viewBeta>+90) viewBeta=+90;
          //commentare/scommentare
	  //rendering(win);
          redraw(); // richiedi ridisegno
        }
        break;
     }
    } else {
      // nessun evento: siamo IDLE
      mozzo--;
      //Aliasing temporale; sembra che le ruote girino all'indietro!!
      //mozzo-=15;
      //Per ruotare la camera intorno alla macchina
      //viewAlpha+=0.25;
      //commentare/scommentare
      //rendering(win);
      redraw(); // richiedi ridisegno
    }
  }

SDL_GL_DeleteContext(mainContext);
SDL_DestroyWindow(win);
SDL_Quit ();
return (0);
}
