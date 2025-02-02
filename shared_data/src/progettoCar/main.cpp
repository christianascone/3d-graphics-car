/*  Copyright 2017 Christian Ascone
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *  */

#include <math.h>
#include <string>
#include <sstream>
#include <iostream>

#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "car.h"

#include <GL/freeglut.h>


#define CAMERA_BACK_CAR 0
#define CAMERA_TOP_FIXED 1
#define CAMERA_TOP_CAR 2
#define CAMERA_PILOT 3
#define CAMERA_MOUSE 4
#define CAMERA_TYPE_MAX 5

#define MENU_BUTTON_WIDTH 140
#define MENU_BUTTON_HEIGHT 40

using namespace std;

float viewAlpha = 20, viewBeta = 40; // angoli che definiscono la vista
float eyeDist = 5.0; // distanza dell'occhio dall'origine
int scrH = 750, scrW = 1050; // altezza e larghezza viewport (in pixels)
bool useWireframe = false;
bool useEnvmap = true;
bool useTransparency = true;
bool useShadow = true;
bool useMap = true;
int cameraType = 0;

Car car; // la nostra macchina
int nstep = 0; // numero di passi di FISICA fatti fin'ora
const int PHYS_SAMPLING_STEP = 10; // numero di millisec che un passo di fisica simula

// Frames Per Seconds
const int fpsSampling = 3000; // lunghezza intervallo di calcolo fps
float fps = 0; // valore di fps dell'intervallo precedente
int fpsNow = 0; // quanti fotogrammi ho disegnato fin'ora nell'intervallo attuale
Uint32 timeLastInterval = 0; // quando e' cominciato l'ultimo intervallo

// Timer in millisecondi per il countdown
long timerInMillisec = 0;
// Ultimo tempo (in millisecondi) raccolto
long lastCheckTimer = 0;
long maxTimer = 0;


extern void setupGoals();

extern void drawPista();
extern void drawTree(bool shadow);
extern void drawBillboard(bool shadow, bool loser);
extern void drawGoals(int num, bool isForMap);

// Struct che rappresenta un bottone del pannello di controllo
struct MenuButton {
  int x, y;
  int w, h;
  MenuButton(int x, int y, int w, int h) {
    this->x = x;
    this->y = y;
    this->w = w;
    this->h = h;
  }
};

/**
* Lista di bottoni per il pannello di controllo
*/
MenuButton cameraButton (5, 5, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT);
MenuButton wireframeButton (5, (MENU_BUTTON_HEIGHT + 5) + 5, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT);
MenuButton textureButton (5, (MENU_BUTTON_HEIGHT + 5) * 2 + 5, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT);

MenuButton lightsButton (5 * 2 + MENU_BUTTON_WIDTH, 5, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT);
MenuButton shadowsButton (5 * 2 + MENU_BUTTON_WIDTH, (MENU_BUTTON_HEIGHT + 5) + 5, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT);
MenuButton mapButton (5 * 2 + MENU_BUTTON_WIDTH, (MENU_BUTTON_HEIGHT + 5) * 2 + 5, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT);

MenuButton resetScoreButton (scrW - MENU_BUTTON_WIDTH - 5, 5, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT);

// Converte un intero in stringa
std::string intToString(int num) {
  std::ostringstream ostr; //output string stream
  ostr << num;
  std::string punteggio = ostr.str();

  return punteggio;
}

// Effettua il reset del countdown a 40 secondi
void resetTimer() {
  // current date/time based on current system
  time_t now = time(0);

  int second = 1000;
  int countdownSeconds = 40;
  //Salvo come tempo iniziale
  timerInMillisec = second * countdownSeconds;
  lastCheckTimer = now;
}

// setta le matrici di trasformazione in modo
// che le coordinate in spazio oggetto siano le coord
// del pixel sullo schemo
void  SetCoordToPixel() {
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(-1, -1, 0);
  glScalef(2.0 / scrW, 2.0 / scrH, 1);
}

bool LoadTexture(int textbind, char *filename) {
  SDL_Surface *s = IMG_Load(filename);
  if (!s) return false;

  glBindTexture(GL_TEXTURE_2D, textbind);
  gluBuild2DMipmaps(
    GL_TEXTURE_2D,
    GL_RGB,
    s->w, s->h,
    GL_RGB,
    GL_UNSIGNED_BYTE,
    s->pixels
  );
  glTexParameteri(
    GL_TEXTURE_2D,
    GL_TEXTURE_MAG_FILTER,
    GL_LINEAR );
  glTexParameteri(
    GL_TEXTURE_2D,
    GL_TEXTURE_MIN_FILTER,
    GL_LINEAR_MIPMAP_LINEAR );
  return true;
}

// disegna gli assi nel sist. di riferimento
void drawAxis() {
  const float K = 0.10;
  glColor3f(0, 0, 1);
  glBegin(GL_LINES);
  glVertex3f( -1, 0, 0 );
  glVertex3f( +1, 0, 0 );

  glVertex3f( 0, -1, 0 );
  glVertex3f( 0, +1, 0 );

  glVertex3f( 0, 0, -1 );
  glVertex3f( 0, 0, +1 );
  glEnd();

  glBegin(GL_TRIANGLES);
  glVertex3f( 0, +1  , 0 );
  glVertex3f( K, +1 - K, 0 );
  glVertex3f(-K, +1 - K, 0 );

  glVertex3f( +1,   0, 0 );
  glVertex3f( +1 - K, +K, 0 );
  glVertex3f( +1 - K, -K, 0 );

  glVertex3f( 0, 0, +1 );
  glVertex3f( 0, +K, +1 - K );
  glVertex3f( 0, -K, +1 - K );
  glEnd();

}

// Disegna un bottone a schermo utilizzando la struct
// menuButton passata come parametro
void drawButton(MenuButton menuButton) {

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0.0, scrW, scrH, 0.0 );
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glEnable(GL_TEXTURE_2D);
  glDisable(GL_LIGHTING);

  glBindTexture(GL_TEXTURE_2D, 13);
  glDisable(GL_TEXTURE_GEN_S);
  glDisable(GL_TEXTURE_GEN_T);
  glColor3f(1, 1, 1);

  int Width = menuButton.w;
  int Height = menuButton.h;
  int X = menuButton.x;
  int Y = menuButton.y;

  glBegin(GL_QUADS);
  glTexCoord2f(0.0f, 0.0f); glVertex2f(X, Y);
  glTexCoord2f(1.0f, 0.0f); glVertex2f(X + Width, Y);
  glTexCoord2f(1.0f, 1.0f); glVertex2f(X + Width, Y + Height);
  glTexCoord2f(0.0f, 1.0f); glVertex2f(X, Y + Height);
  glEnd();

  glEnable(GL_LIGHTING);
  glDisable(GL_TEXTURE_2D);
}

// Disegna l'intero pannello di controllo
void drawControlPanel() {
  drawButton(cameraButton);
  drawButton(wireframeButton);
  drawButton(textureButton);
  drawButton(lightsButton);
  drawButton(shadowsButton);
  drawButton(mapButton);

  // Aggiorna la posizione del bottone in base alla width
  // dello schermo
  resetScoreButton.x = scrW - MENU_BUTTON_WIDTH - 5;
  drawButton(resetScoreButton);
}

void drawSphere(double r, int lats, int longs) {
  int i, j;
  for (i = 0; i <= lats; i++) {
    double lat0 = M_PI * (-0.5 + (double) (i - 1) / lats);
    double z0  = sin(lat0);
    double zr0 =  cos(lat0);

    double lat1 = M_PI * (-0.5 + (double) i / lats);
    double z1 = sin(lat1);
    double zr1 = cos(lat1);

    glBegin(GL_QUAD_STRIP);
    for (j = 0; j <= longs; j++) {
      double lng = 2 * M_PI * (double) (j - 1) / longs;
      double x = cos(lng);
      double y = sin(lng);

//le normali servono per l'EnvMap
      glNormal3f(x * zr0, y * zr0, z0);
      glVertex3f(r * x * zr0, r * y * zr0, r * z0);
      glNormal3f(x * zr1, y * zr1, z1);
      glVertex3f(r * x * zr1, r * y * zr1, r * z1);
    }
    glEnd();
  }
}

void drawFloor(bool map)
{
  float S = 100; // size
  const float H = 0; // altezza
  const int K = 150; //disegna K x K quads

  glPushMatrix();
  if (!map && useEnvmap && !useWireframe) {
    glBindTexture(GL_TEXTURE_2D, 12);
    glEnable(GL_TEXTURE_2D);
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
  }
  if (map) {
    S = 150;
  }
  glDisable(GL_LIGHTING);

  if (!useEnvmap && !map) {
    glColor3f(0.0f, 0.4f, 0.0f);//Forest Green
  } else {
    glColor3f(1, 1, 1);
  }
  // disegna KxK quads
  glBegin(GL_QUADS);
  //glColor3f(0.6, 0.6, 0.6); // colore uguale x tutti i quads
  glNormal3f(0, 1, 0);     // normale verticale uguale x tutti
  for (int x = 0; x < K; x++)
    for (int z = 0; z < K; z++) {
      float x0 = -S + 2 * (x + 0) * S / K;
      float x1 = -S + 2 * (x + 1) * S / K;
      float z0 = -S + 2 * (z + 0) * S / K;
      float z1 = -S + 2 * (z + 1) * S / K;
      glTexCoord2f(0.0f, 1.0f);
      glVertex3d(x0, H, z0);
      glTexCoord2f(0.0f, 0.0f);
      glVertex3d(x1, H, z0);
      glTexCoord2f(1.0f, 0.0f);
      glVertex3d(x1, H, z1);
      glTexCoord2f(1.0f, 1.0f);
      glVertex3d(x0, H, z1);
    }
  glEnd();

  glDisable(GL_TEXTURE_GEN_S);
  glDisable(GL_TEXTURE_GEN_T);
  glDisable(GL_TEXTURE_2D);
  glEnable(GL_LIGHTING);
  glPopMatrix();
}

// Setup della camera da utilizzare per la visualizzazione della mappa
void setCameraMap() {

  double px = 0;
  double py = 0;
  double pz = 0;
  double angle = 0;
  double cosf = cos(angle * M_PI / 180.0);
  double sinf = sin(angle * M_PI / 180.0);
  double camd, camh, ex, ey, ez;

  camd = 1;
  camh = 130.0;
  ex = px + 0;
  ey = py + camh;
  ez = pz + camd;
  gluLookAt(ex, ey, ez, 0, 0, 0, 0.0, 1.0, 0.0);

}

// setto la posizione della camera
void setCamera() {

  double px = car.px;
  double py = car.py;
  double pz = car.pz;
  double angle = car.facing;
  double cosf = cos(angle * M_PI / 180.0);
  double sinf = sin(angle * M_PI / 180.0);
  double camd, camh, ex, ey, ez, cx, cy, cz;
  double cosff, sinff;

// controllo la posizione della camera a seconda dell'opzione selezionata
  switch (cameraType) {
  case CAMERA_BACK_CAR:
    camd = 2.5;
    camh = 2.0;
    ex = px + camd * sinf;
    ey = py + camh;
    ez = pz + camd * cosf;
    cx = px - camd * sinf;
    cy = py + camh;
    cz = pz - camd * cosf;
    gluLookAt(ex, ey, ez, cx, cy, cz, 0.0, 1.0, 0.0);
    break;
  case CAMERA_TOP_FIXED:
    camd = 0.6;
    camh = 0.70;
    angle = car.facing + 40.0;
    cosff = cos(angle * M_PI / 180.0);
    sinff = sin(angle * M_PI / 180.0);
    ex = px + camd * sinff;
    ey = py + camh;
    ez = pz + camd * cosff;
    cx = px - camd * sinf;
    cy = py + camh;
    cz = pz - camd * cosf;
    gluLookAt(ex, ey, ez, cx, cy, cz, 0.0, 1.0, 0.0);
    break;
  case CAMERA_TOP_CAR:
    camd = 2.5;
    camh = 1.0;
    ex = px + camd * sinf;
    ey = py + camh;
    ez = pz + camd * cosf;
    cx = px - camd * sinf;
    cy = py + camh;
    cz = pz - camd * cosf;
    gluLookAt(ex, ey + 5, ez, cx, cy, cz, 0.0, 1.0, 0.0);
    break;
  case CAMERA_PILOT:
    camd = 0.3;
    camh = 1;
    ex = px + camd * sinf;
    ey = py + camh;
    ez = pz + camd * cosf;
    cx = px - camd * sinf;
    cy = py + camh;
    cz = pz - camd * cosf;
    gluLookAt(ex, ey, ez, cx, cy, cz, 0.0, 1.0, 0.0);
    break;
  case CAMERA_MOUSE:
    glTranslatef(0, 0, -eyeDist);
    glRotatef(viewBeta,  1, 0, 0);
    glRotatef(viewAlpha, 0, 1, 0);
    break;
  }
}

void drawSky() {
  int H = 100;

  if (useWireframe) {
    glDisable(GL_TEXTURE_2D);
    glColor3f(0, 0, 0);
    glDisable(GL_LIGHTING);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    drawSphere(200.0, 40, 40);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glColor3f(1, 1, 1);
    glEnable(GL_LIGHTING);
  }
  else
  {
    glBindTexture(GL_TEXTURE_2D, 2);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE , GL_SPHERE_MAP); // Env map
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE , GL_SPHERE_MAP);
    glColor3f(1, 1, 1);
    glDisable(GL_LIGHTING);

    drawSphere(200.0, 40, 40);

    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
  }

}

// Renderizza una data stringa nelle coordinate indicate
void renderString(float x, float y, std::string text)
{
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glOrtho(0.0, scrW, scrH, 0.0, -1.0f, 1.0f);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glColor3f(0, 0, 0);
  glRasterPos2f(x, y);
  char tab2[1024];
  strncpy(tab2, text.c_str(), sizeof(tab2));
  tab2[sizeof(tab2) - 1] = 0;
  glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char *)tab2);

  glPopAttrib();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
}

// Renderizza la mappa su schermo con viewport dedicata
void showMap() {
  // settiamo il viewport
  glViewport(0, 5, 100, 80);

  // colore sfondo = bianco
  glClearColor(1, 1, 1, 1);


  // settiamo la matrice di proiezione
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  gluPerspective( 70, //fovy,
                  ((float)100) / 80,//aspect Y/X,
                  0.2,//distanza del NEAR CLIPPING PLANE in coordinate vista
                  1000  //distanza del FAR CLIPPING PLANE in coordinate vista
                );

  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();


  // setto la posizione luce
  float tmpv[4] = {0, 1, 2,  0}; // ultima comp=0 => luce direzionale
  glLightfv(GL_LIGHT0, GL_POSITION, tmpv );

  setCameraMap();

  static float tmpcol[4] = {1, 1, 1,  1};
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, tmpcol);
  glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 127);

  glEnable(GL_LIGHTING);

  drawFloor(true); // disegna il suolo
  drawPista(); // disegna la pista
  drawGoals(car.totalGoals, true);
  car.Render(false, true); // disegna la macchina


  // attendiamo la fine della rasterizzazione di
  // tutte le primitive mandate

  glDisable(GL_DEPTH_TEST);
  glDisable(GL_LIGHTING);


}

// Stampa i comandi possibili sullo schermo
void printCommands() {
  // Stampo i comandi
  float stringY = 20;
  renderString(cameraButton.x + 5, cameraButton.y + stringY, "F1: Camera");
  renderString(wireframeButton.x + 5, wireframeButton.y + stringY, "F2: Wireframe");
  renderString(textureButton.x + 5, textureButton.y + stringY, "F3: Texture");


  renderString(lightsButton.x + 5, lightsButton.y + stringY, "F4: Trasparenza");
  renderString(shadowsButton.x + 5, shadowsButton.y + stringY, "F5: Ombre");
  renderString(mapButton.x + 5, mapButton.y + stringY, "F6: Mappa");

  renderString(resetScoreButton.x + 5, resetScoreButton.y + stringY, "F7: Reset");
}

// Dato un time, si ottiene una stringa con formato mm:ss
std::string timeToString(long time) {
  float mins = time / (60.*1000.);
  int minsToPrint = (int)mins;
  float seconds = (mins - minsToPrint) * 60.;
  int secondsToPrint = (int)seconds;

  std::stringstream ss;
  if (minsToPrint >= 10) {
    ss << minsToPrint << ":";
  } else {
    ss << "0" << minsToPrint << ":";
  }

  if (secondsToPrint >= 10) {
    ss << secondsToPrint;
  } else {
    ss << "0" << secondsToPrint;
  }
  std::string timeString = ss.str();
  return timeString;
}

/* Esegue il Rendering della scena */
void rendering(SDL_Window *win) {
  // un frame in piu'!!!
  fpsNow++;

  time_t now = time(0);

  // Calcolo la differenza tra il tempo attuale e quello
  // registrato nell'ultimo render
  long newTime = now;
  long difference = (newTime - lastCheckTimer) * 1000;

  // Rimuovo la differenza dal tempo del countdown e lo setto
  // a 0 se negativo
  timerInMillisec -= difference;
  if (timerInMillisec < 0) {
    timerInMillisec = 0;
  }
  lastCheckTimer = newTime;
  std::string timeString = timeToString(timerInMillisec);

  glLineWidth(3); // linee larghe

  // settiamo il viewport
  glViewport(0, 0, scrW, scrH);

  // colore sfondo = bianco
  glClearColor(1, 1, 1, 1);


  // settiamo la matrice di proiezione
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  gluPerspective( 70, //fovy,
                  ((float)scrW) / scrH,//aspect Y/X,
                  0.2,//distanza del NEAR CLIPPING PLANE in coordinate vista
                  1000  //distanza del FAR CLIPPING PLANE in coordinate vista
                );

  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();

  // riempe tutto lo screen buffer di pixel color sfondo
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );


  // setto la posizione luce
  float tmpv[4] = {0, 1, 2,  0}; // ultima comp=0 => luce direzionale
  glLightfv(GL_LIGHT0, GL_POSITION, tmpv );


  // settiamo matrice di vista
  setCamera();


  static float tmpcol[4] = {1, 1, 1,  1};
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, tmpcol);
  glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 127);

  glEnable(GL_LIGHTING);

  drawSky(); // disegna il cielo come sfondo

  drawFloor(false); // disegna il suolo

  drawTree(false); 

  car.checkCollision(car.px, car.pz);

  bool loser = false;
  if (timerInMillisec > 0 && car.goalsReached == car.totalGoals) {
    printf("\nAggiornamento livello difficoltà \n");
    car.updateDifficultyLevel();
    resetTimer();
    timerInMillisec = maxTimer * 1.3;
    maxTimer = timerInMillisec;
  } else if (timerInMillisec <= 0) {
    loser = true;
  }

  drawBillboard(false, loser); // disegna il cartellone
  drawPista(); // disegna la pista

  car.Render(); // disegna la macchina

  drawGoals(car.totalGoals, false);

  if (useShadow)
  {
    glPushMatrix();
    glColor3f(0.1, 0.1, 0.1); // colore fisso
    glTranslatef(0, 0.01, -2); // alzo l'ombra di un epsilon per evitare z-fighting con il pavimento
    glScalef(1.01, 0, 1.01); // appiattisco sulla Y, ingrandisco dell'1% sulla Z e sulla X
    glDisable(GL_LIGHTING); // niente lighing per l'ombra

    drawTree(true); 
    drawBillboard(true, loser); // disegna il cartellone

    glEnable(GL_LIGHTING);
    glPopMatrix();
  }

  renderString(scrW / 2, 20, timeString);
  std::stringstream ss;
  ss << "Score: " << car.goalsReached << "/" << car.totalGoals;
  std::string score = ss.str();
  renderString(scrW / 2, 40, score);

  // attendiamo la fine della rasterizzazione di
  // tutte le primitive mandate

  glDisable(GL_DEPTH_TEST);
  glDisable(GL_LIGHTING);

  drawControlPanel();
  printCommands();

  if (useMap) {
    showMap();
  }

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);


  glFinish();

  // ho finito: buffer di lavoro diventa visibile
  SDL_GL_SwapWindow(win);
}

void redraw() {
  // ci automandiamo un messaggio che (s.o. permettendo)
  // ci fara' ridisegnare la finestra
  SDL_Event e;
  e.type = SDL_WINDOWEVENT;
  e.window.event = SDL_WINDOWEVENT_EXPOSED;
  SDL_PushEvent(&e);
}


int main(int argc, char* argv[])
{
  SDL_Window *win;
  SDL_GLContext mainContext;
  Uint32 windowID;
  SDL_Joystick *joystick;
  static int keymap[Controller::NKEYS] = {SDLK_a, SDLK_d, SDLK_w, SDLK_s, SDLK_x};

  // inizializzazione di SDL
  SDL_Init( SDL_INIT_VIDEO | SDL_INIT_JOYSTICK);

  SDL_JoystickEventState(SDL_ENABLE);
  joystick = SDL_JoystickOpen(0);

  SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 );
  SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

  // facciamo una finestra di scrW x scrH pixels
  win = SDL_CreateWindow(argv[0], 0, 0, scrW, scrH, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

  //Create our opengl context and attach it to our window
  mainContext = SDL_GL_CreateContext(win);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_NORMALIZE); // opengl, per favore, rinormalizza le normali
  // prima di usarle
  //glEnable(GL_CULL_FACE);
  glFrontFace(GL_CW); // consideriamo Front Facing le facce ClockWise
  glEnable(GL_COLOR_MATERIAL);
  glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
  glEnable(GL_POLYGON_OFFSET_FILL); // caro openGL sposta i
  // frammenti generati dalla
  // rasterizzazione poligoni
  glPolygonOffset(1, 1);            // indietro di 1

  /*
  * Carico le texture
  */
  if (!LoadTexture(0, (char *)"texture/wheel.jpg")) return 0;
  if (!LoadTexture(1, (char *)"texture/red.jpg")) return 0;
  if (!LoadTexture(2, (char *)"sky_ok.jpg")) return -1;
  if (!LoadTexture(3, (char *)"texture/brown.jpg")) return 0;
  if (!LoadTexture(4, (char *)"texture/glass.jpg")) return 0;
  if (!LoadTexture(5, (char *)"texture/tree.jpg")) return 0;
  if (!LoadTexture(6, (char *)"texture/leaf.jpg")) return 0;
  if (!LoadTexture(7, (char *)"texture/light_leather.jpg")) return 0;
  if (!LoadTexture(8, (char *)"texture/dark_leather.jpg")) return 0;
  if (!LoadTexture(9, (char *)"texture/selfie.jpg")) return 0;
  if (!LoadTexture(10, (char *)"texture/decor_metal.jpg")) return 0;
  if (!LoadTexture(12, (char *)"texture/grass.jpg")) return 0;
  if (!LoadTexture(13, (char *)"texture/button.jpg")) return 0;
  if (!LoadTexture(14, (char *)"texture/lose.jpg")) return 0;

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);

  setupGoals();

  resetTimer();
  maxTimer = timerInMillisec;

  bool done = 0;
  while (!done) {

    SDL_Event e;

    // guardo se c'e' un evento:
    if (SDL_PollEvent(&e)) {
      // se si: processa evento
      switch (e.type) {
      case SDL_KEYDOWN:
        car.controller.EatKey(e.key.keysym.sym, keymap , true);
        if (e.key.keysym.sym == SDLK_F1) cameraType = (cameraType + 1) % CAMERA_TYPE_MAX;
        if (e.key.keysym.sym == SDLK_F2) useWireframe = !useWireframe;
        if (e.key.keysym.sym == SDLK_F3) useEnvmap = !useEnvmap;
        if (e.key.keysym.sym == SDLK_F4) useTransparency = !useTransparency;
        if (e.key.keysym.sym == SDLK_F5) useShadow = !useShadow;
        if (e.key.keysym.sym == SDLK_F6) useMap = !useMap;
        if (e.key.keysym.sym == SDLK_F7) {car.resetScore(); resetTimer(); maxTimer = timerInMillisec;}
        break;
      case SDL_KEYUP:
        car.controller.EatKey(e.key.keysym.sym, keymap , false);
        break;
      case SDL_QUIT:
        done = 1;   break;
      case SDL_WINDOWEVENT:
        // dobbiamo ridisegnare la finestra
        if (e.window.event == SDL_WINDOWEVENT_EXPOSED)
          rendering(win);
        else {
          windowID = SDL_GetWindowID(win);
          if (e.window.windowID == windowID)  {
            switch (e.window.event)  {
            case SDL_WINDOWEVENT_SIZE_CHANGED:  {
              scrW = e.window.data1;
              scrH = e.window.data2;
              glViewport(0, 0, scrW, scrH);
              rendering(win);
              //redraw(); // richiedi ridisegno
              break;
            }
            }
          }
        }
        break;

      case SDL_MOUSEMOTION:
        if (e.motion.state & SDL_BUTTON(1) & cameraType == CAMERA_MOUSE) {
          viewAlpha += e.motion.xrel;
          viewBeta += e.motion.yrel;
//          if (viewBeta<-90) viewBeta=-90;
          if (viewBeta < +5) viewBeta = +5; //per non andare sotto la macchina
          if (viewBeta > +90) viewBeta = +90;
          // redraw(); // richiedi un ridisegno (non c'e' bisongo: si ridisegna gia'
          // al ritmo delle computazioni fisiche)
        }
        break;

      case SDL_MOUSEWHEEL:
        if (e.wheel.y < 0 ) {
          // avvicino il punto di vista (zoom in)
          eyeDist = eyeDist * 0.9;
          if (eyeDist < 1) eyeDist = 1;
        };
        if (e.wheel.y > 0 ) {
          // allontano il punto di vista (zoom out)
          eyeDist = eyeDist / 0.9;
        };
        break;

      case SDL_MOUSEBUTTONDOWN:
        if (e.button.button == SDL_BUTTON_LEFT) {
          int x = e.button.x;
          int y = e.button.y;

          // Check di ogni bottone
          MenuButton buttons[] = {cameraButton, wireframeButton, textureButton, lightsButton, shadowsButton, mapButton, resetScoreButton};

          for (int index = 0; index < (sizeof(buttons) / sizeof(*buttons)); index++) {
            if (x > buttons[index].x & x < buttons[index].x + buttons[index].w & y > buttons[index].y & y < buttons[index].y + buttons[index].h) {
              if (index == 0) {
                cameraType = (cameraType + 1) % CAMERA_TYPE_MAX;
              } else if (index == 1) {
                useWireframe = !useWireframe;
              } else if (index == 2) {
                useEnvmap = !useEnvmap;
              } else if (index == 3) {
                useTransparency = !useTransparency;
              } else if (index == 4) {
                useShadow = !useShadow;
              } else if (index == 5) {
                useMap = !useMap;
              } else if (index == 6) {
                car.resetScore();
                resetTimer();
                maxTimer = timerInMillisec;
              }
            }
          }
          redraw();
        }
        break;

      case SDL_JOYAXISMOTION: /* Handle Joystick Motion */
        if ( e.jaxis.axis == 0)
        {
          if ( e.jaxis.value < -3200  )
          {
            car.controller.Joy(0 , true);
            car.controller.Joy(1 , false);
//        printf("%d <-3200 \n",e.jaxis.value);
          }
          if ( e.jaxis.value > 3200  )
          {
            car.controller.Joy(0 , false);
            car.controller.Joy(1 , true);
//        printf("%d >3200 \n",e.jaxis.value);
          }
          if ( e.jaxis.value >= -3200 && e.jaxis.value <= 3200 )
          {
            car.controller.Joy(0 , false);
            car.controller.Joy(1 , false);
//        printf("%d in [-3200,3200] \n",e.jaxis.value);
          }
          rendering(win);
          //redraw();
        }
        break;
      case SDL_JOYBUTTONDOWN: /* Handle Joystick Button Presses */
        if ( e.jbutton.button == 0 )
        {
          car.controller.Joy(2 , true);
//     printf("jbutton 0\n");
        }
        if ( e.jbutton.button == 2 )
        {
          car.controller.Joy(3 , true);
//     printf("jbutton 2\n");
        }
        break;
      case SDL_JOYBUTTONUP: /* Handle Joystick Button Presses */
        car.controller.Joy(2 , false);
        car.controller.Joy(3 , false);
        break;
      }
    } else {
      // nessun evento: siamo IDLE

      Uint32 timeNow = SDL_GetTicks(); // che ore sono?

      if (timeLastInterval + fpsSampling < timeNow) {
        fps = 1000.0 * ((float)fpsNow) / (timeNow - timeLastInterval);
        fpsNow = 0;
        timeLastInterval = timeNow;
      }

      bool doneSomething = false;
      int guardia = 0; // sicurezza da loop infinito

      // finche' il tempo simulato e' rimasto indietro rispetto
      // al tempo reale...
      while (nstep * PHYS_SAMPLING_STEP < timeNow ) {
        car.DoStep();
        nstep++;
        doneSomething = true;
        timeNow = SDL_GetTicks();
        if (guardia++ > 1000) {done = true; break;} // siamo troppo lenti!
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

