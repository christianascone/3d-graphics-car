#include <stdio.h>
#include <math.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL2/SDL.h>

#include "car.h"

float viewAlpha=20, viewBeta=40; // angoli che definiscono la vista
float eyeDist=3.0; // distanza dell'occhio dall'origine
int scrH=500, scrW=500; // altezza e larghezza viewport (in pixels)

Car car; // la nostra macchina
int nstep=0; // numero di passi di FISICA fatti fin'ora
const int PHYS_SAMPLING_STEP=10; // numero di millisec che un passo di fisica simula

// Frames Per Seconds
const int fpsSampling = 3000; // lunghezza intervallo di calcolo fps
float fps=0; // valore di fps dell'intervallo precedente
int fpsNow=0; // quanti fotogrammi ho disegnato fin'ora nell'intervallo attuale
Uint32 timeLastInterval=0; // quando e' cominciato l'ultimo intervallo

// setta le matrici di trasformazione in modo
// che le coordinate in spazio oggetto siano le coord
// del pixel sullo schemo
void  SetCoordToPixel(){
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(-1,-1,0);
  glScalef(2.0/scrW, 2.0/scrH, 1);
}


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
 
    glNormal3f(  0,0,+1  );
    glVertex3f( +1,+1,+1 );
    glVertex3f( -1,+1,+1 );
    glVertex3f( -1,-1,+1 );
    glVertex3f( +1,-1,+1 );

    glNormal3f(  0,0,-1  );
    glVertex3f( +1,-1,-1 );
    glVertex3f( -1,-1,-1 );
    glVertex3f( -1,+1,-1 );
    glVertex3f( +1,+1,-1 );

    glNormal3f(  0,+1,0  );
    glVertex3f( +1,+1,+1 );
    glVertex3f( -1,+1,+1 );
    glVertex3f( -1,+1,-1 );
    glVertex3f( +1,+1,-1 );

    glNormal3f(  0,-1,0  );
    glVertex3f( +1,-1,-1 );
    glVertex3f( -1,-1,-1 );
    glVertex3f( -1,-1,+1 );
    glVertex3f( +1,-1,+1 );

    glNormal3f( +1,0,0  );
    glVertex3f( +1,+1,+1 );
    glVertex3f( +1,-1,+1 );
    glVertex3f( +1,-1,-1 );
    glVertex3f( +1,+1,-1 );

    glNormal3f( -1,0,0  );
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

void drawFloor()
{
  const float S=20; // size
  const float H=0;   // altezza
  const int K=200; //disegna K x K quads
  
  // disegna un quad solo
  /*
  glBegin(GL_QUADS);
    glColor3f(0.5, 0.2, 0.0);
    glNormal3f(0,1,0);
    glVertex3d(-S, H, -S);
    glVertex3d(+S, H, -S);
    glVertex3d(+S, H, +S);
    glVertex3d(-S, H, +S);
  glEnd();
  */
 
  // disegna KxK quads
  glBegin(GL_QUADS);
    glColor3f(0.6, 0.6, 0.6); // colore uguale x tutti i quads
    glNormal3f(0,1,0);       // normale verticale uguale x tutti
    for (int x=0; x<K; x++)
    for (int z=0; z<K; z++) {
      float x0=-S + 2*(x+0)*S/K;
      float x1=-S + 2*(x+1)*S/K;
      float z0=-S + 2*(z+0)*S/K;
      float z1=-S + 2*(z+1)*S/K;
      glVertex3d(x0, H, z0);
      glVertex3d(x1, H, z0);
      glVertex3d(x1, H, z1);
      glVertex3d(x0, H, z1);
    }
  glEnd();
 
}

/* Esegue il Rendering della scena */
void rendering(SDL_Window *win){
 
  // un frame in piu'!!!
  fpsNow++;
 
  glLineWidth(3); // linee larghe
 
  // settiamo il viewport
  glViewport(0,0, scrW, scrH);
 
  // colore sfondo = nero
  glClearColor(1,1,1,1);


  // settiamo la matrice di proiezione
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  gluPerspective( 80, //fovy,
		((float)scrW) / scrH,//aspect Y/X,
		0.25,//zNear,
		100  //zFar
                );

  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();

  // riempe tutto lo screen buffer di pixel color sfondo
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
 
  //drawAxis(); // disegna assi frame VISTA
 
  // setto la posizione luce
  float tmpv[4] = {0,1,2,  0}; // ultima comp=0 => luce direzionale
  glLightfv(GL_LIGHT0, GL_POSITION, tmpv );


  // settiamo matrice di vista
  glTranslatef(0,0,-eyeDist);
  glRotatef(viewBeta,  1,0,0);
  glRotatef(viewAlpha, 0,1,0);


  //drawAxis(); // disegna assi frame MONDO
  static float tmpcol[4] = {1,1,1,1};
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, tmpcol);
  glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 127);

  // settiamo matrice di modellazione
 
  //drawAxis(); // disegna assi frame OGGETTO
  //drawCubeWire();

    drawFloor(); // disegna il suolo

    car.Render(); // disegna la macchina

  // attendiamo la fine della rasterizzazione di
  // tutte le primitive mandate

  glDisable(GL_DEPTH_TEST);
  glDisable(GL_LIGHTING);
 

  // disegnamo i fps (frame x sec) come una barra a sinistra.
  // (vuota = 0 fps, piena = 100 fps)
  SetCoordToPixel();
  glBegin(GL_QUADS);
  float y=scrH*fps/100;
  float ramp=fps/100;
  glColor3f(1-ramp,0,ramp); // color ramp: da rosso (0 fps) a blu (100 fps)
  glVertex2d(10,0);
  glVertex2d(10,y);
  glVertex2d(0,y);
  glVertex2d(0,0);
  glEnd();
 
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);
 
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
SDL_Joystick *joystick;
static int keymap[Controller::NKEYS] = {SDLK_a, SDLK_d, SDLK_w, SDLK_s};

  // inizializzazione di SDL
  SDL_Init( SDL_INIT_VIDEO | SDL_INIT_JOYSTICK );

  SDL_JoystickEventState(SDL_ENABLE);
  joystick = SDL_JoystickOpen(0);
 
  SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 );
  SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
 
  // facciamo una finestra di scrW x scrH pixels
  win=SDL_CreateWindow(argv[0], 0, 0, scrW, scrH, SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);

  //Create our opengl context and attach it to our window
  mainContext=SDL_GL_CreateContext(win);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_NORMALIZE); // rinormalizza le normali prima di usarle
  //glEnable(GL_CULL_FACE);
  glFrontFace(GL_CW); // consideriamo Front Facing le facce ClockWise
  glEnable(GL_COLOR_MATERIAL);
  glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
 
  bool done=0;
  while (!done) {
 
    SDL_Event e;
 
    // guardo se c'e' un evento:
    if (SDL_PollEvent(&e)) {
     // se si: processa evento
     switch (e.type) {
      case SDL_KEYDOWN:
        car.controller.EatKey(e.key.keysym.sym, keymap , true);
        break;
      case SDL_KEYUP:
        car.controller.EatKey(e.key.keysym.sym, keymap , false);
        break;
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
                     rendering(win);
                     //redraw(); // richiedi ridisegno
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
          //if (viewBeta<-90) viewBeta=-90;
          if (viewBeta<+5) viewBeta=+5; //per non andare sotto la macchina
          if (viewBeta>+90) viewBeta=+90;
	  rendering(win);
          //redraw(); // richiedi un ridisego
        }
        break;

     case SDL_MOUSEWHEEL:
       if (e.wheel.y < 0 ) {
         // avvicino il punto di vista (zoom in)
         eyeDist=eyeDist*0.9;
         if (eyeDist<1) eyeDist = 1;
       };
       if (e.wheel.y > 0 ) {
         // allontano il punto di vista (zoom out)
         eyeDist=eyeDist/0.9;
       };
     break;

     case SDL_JOYAXISMOTION: /* Handle Joystick Motion */
        if( e.jaxis.axis == 0)
         {
            if ( e.jaxis.value < -3200  )
             {
              car.controller.Joy(0 , true);
              car.controller.Joy(1 , false);
             }
            if ( e.jaxis.value > 3200  )
            {
              car.controller.Joy(0 , false);
              car.controller.Joy(1 , true);
            }
            if ( e.jaxis.value >= -3200 && e.jaxis.value <= 3200 )
             {
              car.controller.Joy(0 , false);
              car.controller.Joy(1 , false);
             }
	    rendering(win);
            //redraw();
          }
/*
        if( e.jaxis.axis == 1)
         {
            if ( e.jaxis.value < -3200  )
             {
              car.controller.Joy(2 , true);
              car.controller.Joy(3 , false);
             }
            if ( e.jaxis.value > 3200  )
            {
              car.controller.Joy(2 , false);
              car.controller.Joy(3 , true);
            }
            if ( e.jaxis.value >= -3200 && e.jaxis.value <= 3200 )
             {
              car.controller.Joy(2 , false);
              car.controller.Joy(3 , false);
             }
	    rendering(win);
            //redraw();
          }
*/
        break;
      case SDL_JOYBUTTONDOWN: /* Handle Joystick Button Presses */
        if ( e.jbutton.button == 0 )
        {
//         done=1;
           car.controller.Joy(2 , true);
           car.controller.Joy(3 , false);
        }
        if ( e.jbutton.button == 1 )
        {
//         done=1;
           car.controller.Joy(2 , false);
           car.controller.Joy(3 , false);
        }
        if ( e.jbutton.button == 2 )
        {
//         done=1;
           car.controller.Joy(2 , false);
           car.controller.Joy(3 , true);
        }
        if ( e.jbutton.button == 3 )
        {
//         done=1;
           car.controller.Joy(2 , false);
           car.controller.Joy(3 , false);
        }
        break;
     }
    } else {
      // nessun evento: siamo IDLE
 
      Uint32 timeNow=SDL_GetTicks(); // che ore sono?
 
      if (timeLastInterval+fpsSampling<timeNow) {
        // e' tempo di ricalcolare i Frame per Sec
        fps = 1000.0*((float)fpsNow) /(timeNow-timeLastInterval);
        fpsNow=0;
        timeLastInterval = timeNow;
      }
 
      bool doneSomething=false;
      int guardia=0; // sicurezza da loop infinito
 
      // finche' il tempo simulato e' rimasto indietro rispetto
      // al tempo reale...
      while (nstep*PHYS_SAMPLING_STEP < timeNow ) {
        car.DoStep();
        nstep++;
        doneSomething=true;
        timeNow=SDL_GetTicks();
        if (guardia++>1000) {done=true; break;} // siamo troppo lenti!
      }
 
      if (doneSomething)
      rendering(win);
      //redraw();
      else {
        // tempo libero!!!
      }
    }
  }
SDL_GL_DeleteContext(mainContext);
SDL_DestroyWindow(win);
SDL_Quit ();
return (0);
}
