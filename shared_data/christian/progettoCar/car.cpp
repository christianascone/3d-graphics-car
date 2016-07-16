// car.cpp
// implementazione dei metodi definiti in car.h

#include <stdio.h>
#include <math.h>

#include <GL/gl.h>
#include <GL/glu.h>

#include <vector> // la classe vector di STL 

#include "car.h"
#include "point3.h"
#include "mesh.h"
#include <SDL2/SDL.h>

/**
** Inizializzazione mesh
*/
// Corpo Macchina
Mesh antenna((char *)"ape/obj/antenna.obj"); // chiama il costruttore
Mesh asta((char *)"ape/obj/asta.obj");
Mesh asta_brake((char *)"ape/obj/asta_brake.obj");
Mesh backpiruli((char *)"ape/obj/backpiruli.obj"); // chiama il costruttore
Mesh backsits((char *)"ape/obj/backsits.obj");
Mesh bars((char *)"ape/obj/bars.obj"); // chiama il costruttore
Mesh board((char *)"ape/obj/board.obj");
Mesh bottomsits((char *)"ape/obj/bottomsits.obj"); // chiama il costruttore
Mesh brakes((char *)"ape/obj/brakes.obj");
Mesh brake_block((char *)"ape/obj/brake_block.obj");
Mesh carlinga((char *)"ape/obj/carlinga.obj"); // chiama il costruttore
Mesh glasses((char *)"ape/obj/glass.obj");
Mesh interni((char *)"ape/obj/interni.obj"); // chiama il costruttore
Mesh lateral((char *)"ape/obj/lateral.obj");
Mesh lights((char *)"ape/obj/lights.obj"); // chiama il costruttore
Mesh marmitta((char *)"ape/obj/marmitta.obj");
Mesh mirrors((char *)"ape/obj/mirrors.obj"); // chiama il costruttore
Mesh parafango((char *)"ape/obj/parafango.obj"); // chiama il costruttore
Mesh piruli((char *)"ape/obj/piruli.obj");
Mesh portapacchi_piruli((char *)"ape/obj/portapacchi_piruli.obj");
Mesh shades((char *)"ape/obj/shades.obj");

// Ruote
Mesh wheelBR1((char *)"obj/Ferrari_wheel_back_R.obj");
Mesh wheelFR1((char *)"obj/Ferrari_wheel_front_R.obj");
Mesh wheelBR2((char *)"obj/Ferrari_wheel_back_R_metal.obj");
Mesh wheelFR2((char *)"obj/Ferrari_wheel_front_R_metal.obj");

Mesh pista((char *)"obj/pista.obj");

Mesh tree1((char *)"obj/tree1.obj");
Mesh tree2((char *)"obj/tree2.obj");
Mesh tree3((char *)"obj/tree3.obj");

// Billboard
Mesh billboard_base((char *)"obj/billboard/base.obj");
Mesh billboard_face1((char *)"obj/billboard/face1.obj");
Mesh billboard_face2((char *)"obj/billboard/face2.obj");
Mesh billboard_internal((char *)"obj/billboard/internal.obj");
Mesh billboard_lightsupport((char *)"obj/billboard/light_support.obj");

struct GoalCircle {
  float x, y, z;
  float r;
  bool done;
  GoalCircle() {

  }
  GoalCircle(float x, float y, float z, float r) {
    this->x = x;
    this->y = y;
    this->z = z;
    this->r = r;
    this->done = false;
  }
};

extern bool useEnvmap; // var globale esterna: per usare l'evnrionment mapping
extern bool useHeadlight; // var globale esterna: per usare i fari
extern bool useShadow; // var globale esterna: per generare l'ombra

const int goalsNumber = 4;
GoalCircle goals[goalsNumber];

void setupGoals() {
  goals[goalsNumber];
  goals[0] = GoalCircle(6, 10, -47, 10);
  goals[1] = GoalCircle(-66, 10, -37, 10);
  goals[2] = GoalCircle(-21, 10, 48, 10);
  goals[3] = GoalCircle(26, 10, 30, 10);

  for (int i = 0; i < goalsNumber; i++) {
    GoalCircle goal = goals[i];
    printf("Goal %d %f\n", i, goal.x);
    printf("Goal %d %f\n", i, goal.y);
    printf("Goal %d %f\n", i, goal.z);
    printf("Goal %d %f\n", i, goal.r);
    printf("\n");
  }
}

// da invocare quando e' stato premuto/rilasciato il tasto numero "keycode"
void Controller::EatKey(int keycode, int* keymap, bool pressed_or_released)
{
  for (int i = 0; i < NKEYS; i++) {
    if (keycode == keymap[i]) key[i] = pressed_or_released;
  }
}

// da invocare quando e' stato premuto/rilasciato un jbutton
void Controller::Joy(int keymap, bool pressed_or_released)
{
  key[keymap] = pressed_or_released;
}

// Funzione che prepara tutto per usare un env map
void SetupEnvmapTexture(int textureNum)
{
  // facciamo binding con la texture 1
  glBindTexture(GL_TEXTURE_2D, textureNum);

  glEnable(GL_TEXTURE_2D);
  glEnable(GL_TEXTURE_GEN_S); // abilito la generazione automatica delle coord texture S e T
  glEnable(GL_TEXTURE_GEN_T);
  glTexGeni(GL_S, GL_TEXTURE_GEN_MODE , GL_SPHERE_MAP); // Env map
  glTexGeni(GL_T, GL_TEXTURE_GEN_MODE , GL_SPHERE_MAP);
  glColor3f(1, 1, 1); // metto il colore neutro (viene moltiplicato col colore texture, componente per componente)
  glDisable(GL_LIGHTING); // disabilito il lighting OpenGL standard (lo faccio con la texture)
}

void SetupEnvmapTextureRed()
{
  SetupEnvmapTexture(1);
}

void SetupEnvmapTextureBrown()
{
  SetupEnvmapTexture(3);
}

void SetupEnvmapTextureGlass()
{
  glBindTexture(GL_TEXTURE_2D, 4);

  glEnable(GL_TEXTURE_2D);
  glEnable(GL_TEXTURE_GEN_S); // abilito la generazione automatica delle coord texture S e T
  glEnable(GL_TEXTURE_GEN_T);
  glTexGeni(GL_S, GL_TEXTURE_GEN_MODE , GL_SPHERE_MAP); // Env map
  glTexGeni(GL_T, GL_TEXTURE_GEN_MODE , GL_SPHERE_MAP);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glColor4f (0.5, 0.5, 0.5, 0.3);
  glDisable(GL_LIGHTING); // disabilito il lighting OpenGL standard (lo faccio con la texture)
}

void SetupEnvmapTextureTree()
{
  SetupEnvmapTexture(5);
}

void SetupEnvmapTextureLeaf()
{
  SetupEnvmapTexture(6);
}

void SetupEnvmapTextureLightLeather()
{
  SetupEnvmapTexture(7);
}

void SetupEnvmapTextureDarkLeather()
{
  SetupEnvmapTexture(8);
}

void SetupEnvmapTextureDecorMetal()
{
  SetupEnvmapTexture(10);
}

void SetupEnvmapTextureRoad()
{
  SetupEnvmapTexture(11);
}

// funzione che prepara tutto per creare le coordinate texture (s,t) da (x,y,z)
// Mappo l'intervallo [ minY , maxY ] nell'intervallo delle T [0..1]
//     e l'intervallo [ minZ , maxZ ] nell'intervallo delle S [0..1]
void SetupWheelTexture(Point3 min, Point3 max) {
  glBindTexture(GL_TEXTURE_2D, 0);
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_TEXTURE_GEN_S);
  glEnable(GL_TEXTURE_GEN_T);

  // ulilizzo le coordinate OGGETTO
  // cioe' le coordnate originali, PRIMA della moltiplicazione per la ModelView
  // in modo che la texture sia "attaccata" all'oggetto, e non "proiettata" su esso
  glTexGeni(GL_S, GL_TEXTURE_GEN_MODE , GL_OBJECT_LINEAR);
  glTexGeni(GL_T, GL_TEXTURE_GEN_MODE , GL_OBJECT_LINEAR);
  float sz = 1.0 / (max.Z() - min.Z());
  float ty = 1.0 / (max.Y() - min.Y());
  float s[4] = {0, 0, sz,  - min.Z()*sz };
  float t[4] = {0, ty, 0,  - min.Y()*ty };
  glTexGenfv(GL_S, GL_OBJECT_PLANE, s);
  glTexGenfv(GL_T, GL_OBJECT_PLANE, t);
}

// Setup della texture con la foto
void SetupPhotoTexture(Point3 min, Point3 max, bool winner) {
  if (!winner) {
    glBindTexture(GL_TEXTURE_2D, 9);
  } else {
    glBindTexture(GL_TEXTURE_2D, 14);
  }
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_TEXTURE_GEN_S);
  glEnable(GL_TEXTURE_GEN_T);
  glEnable(GL_TEXTURE_GEN_R);

  // ulilizzo le coordinate OGGETTO
  // cioe' le coordnate originali, PRIMA della moltiplicazione per la ModelView
  // in modo che la texture sia "attaccata" all'oggetto, e non "proiettata" su esso
  glTexGeni(GL_S, GL_TEXTURE_GEN_MODE , GL_OBJECT_LINEAR);
  glTexGeni(GL_T, GL_TEXTURE_GEN_MODE , GL_OBJECT_LINEAR);
  glTexGeni(GL_R, GL_TEXTURE_GEN_MODE , GL_OBJECT_LINEAR);


  float sx = 1.0 / (max.X() - min.X()); // Inverto per flip sx - dx
  float ty = 1.0 / (min.Y() - max.Y()); // Inverto per flip up - down
  float rz = 1.0 / (max.Z() - min.Z());

  float s[4] = {sx, 0, 0,  - min.X()*sx };
  float t[4] = {0, ty, 0,  - min.Y()*ty };
  float r[4] = {0, 0, rz,  - min.Z()*rz };

  glTexGenfv(GL_S, GL_OBJECT_PLANE, s);
  glTexGenfv(GL_T, GL_OBJECT_PLANE, t);
  glTexGenfv(GL_R, GL_OBJECT_PLANE, r);
}

// DoStep: facciamo un passo di fisica (a delta_t costante)
//
// Indipendente dal rendering.
//
// ricordiamoci che possiamo LEGGERE ma mai SCRIVERE
// la struttura controller da DoStep
void Car::DoStep() {
  // computiamo l'evolversi della macchina
  static int i = 5;

  float vxm, vym, vzm; // velocita' in spazio macchina

  // da vel frame mondo a vel frame macchina
  float cosf = cos(facing * M_PI / 180.0);
  float sinf = sin(facing * M_PI / 180.0);
  vxm = +cosf * vx - sinf * vz;
  vym = vy;
  vzm = +sinf * vx + cosf * vz;

  // gestione dello sterzo
  if (controller.key[Controller::LEFT]) sterzo += velSterzo;
  if (controller.key[Controller::RIGHT]) sterzo -= velSterzo;
  sterzo *= velRitornoSterzo; // ritorno a volante dritto

  if (controller.key[Controller::ACC]) vzm -= accMax; // accelerazione in avanti
  if (controller.key[Controller::DEC]) vzm += accMax; // accelerazione indietro

  // attirti (semplificando)
  vxm *= attritoX;
  vym *= attritoY;
  vzm *= attritoZ;

  // l'orientamento della macchina segue quello dello sterzo
  // (a seconda della velocita' sulla z)
  facing = facing - (vzm * grip) * sterzo;

  // rotazione mozzo ruote (a seconda della velocita' sulla z)
  float da ; //delta angolo
  da = (360.0 * vzm) / (2.0 * M_PI * raggioRuotaA);
  mozzoA += da;
  da = (360.0 * vzm) / (2.0 * M_PI * raggioRuotaP);
  mozzoP += da;

  // ritorno a vel coord mondo
  vx = +cosf * vxm + sinf * vzm;
  vy = vym;
  vz = -sinf * vxm + cosf * vzm;

  // posizione = posizione + velocita * delta t (ma delta t e' costante)
  px += vx;
  py += vy;
  pz += vz;
}

//void drawCube(); // questa e' definita altrove (quick hack)
void drawAxis(); // anche questa

// Disegno la pista con la mesh pista
void drawPista () {
  glPushMatrix();
  glColor3f(0.5, 0.5, 0.5);
  glScalef(0.70, 0.9, 0.70);
  glTranslatef(0, 0.01, 0);
  pista.RenderNxV();
  glPopMatrix();
}

// Disegno l'albero
void drawTree () {
  bool usecolor = true;
  glPushMatrix();

  // Setup della texture dei tronchi
  if (!useEnvmap)
  {
    if (usecolor) glColor3f(1, 0, 0);   // colore rosso, da usare con Lighting
  }
  else {
    if (usecolor) SetupEnvmapTextureTree();
  }
  tree1.RenderNxV();
  tree3.RenderNxV();
  //if (usecolor) glEnable(GL_LIGHTING);

  // Setup della texture per le foglie dell'albero
  if (!useEnvmap)
  {
    if (usecolor) glColor3f(1, 0, 0);   // colore rosso, da usare con Lighting
  }
  else {
    if (usecolor) SetupEnvmapTextureLeaf();
  }
  tree2.RenderNxV();
  //if (usecolor) glEnable(GL_LIGHTING);
  glDisable(GL_TEXTURE_2D);
  glPopMatrix();
}

// Disegna il cartellone pubblicitario con l'immagine
void drawBillboard (bool winner) {
  bool usecolor = true;
  glPushMatrix();
  glTranslatef(30, 0, 0);
  if (!useEnvmap)
  {
    if (usecolor) glColor3f(0.5, 0.5, 0.5);   // colore nero, da usare con Lighting
  }
  else {
    if (usecolor) glColor3f(0.5, 0.5, 0.5);   // colore rosso, da usare con Lighting

    // Applico la texture della parte "metallica" del cartellone
    glBindTexture(GL_TEXTURE_2D, 10);
    glEnable(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);
    glDisable(GL_LIGHTING);
  }

  billboard_base.RenderNxV();
  billboard_internal.RenderNxV();
  billboard_lightsupport.RenderNxV();
  //if (usecolor) glEnable(GL_LIGHTING);

  if (!useEnvmap)
  {
    if (usecolor) glColor3f(1, 1, 1);   // colore rosso, da usare con Lighting
  }
  else {
    if (usecolor) glColor3f(1, 1, 1);   // colore rosso, da usare con Lighting
    if (usecolor) SetupPhotoTexture(billboard_face1.bbmin, billboard_face1.bbmax, winner);
  }
  billboard_face1.RenderNxV();
  billboard_face2.RenderNxV();
  //if (usecolor) glEnable(GL_LIGHTING);
  glDisable(GL_TEXTURE_2D);
  glPopMatrix();
}

// Disegna un cerchio con coordinate x e y
// e raggio r
void drawCircle(int x, int y, float z, int r, bool rotate) {
  glBegin(GL_LINE_STRIP);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glColor4f (0.5, 0.5, 0.5, 0.3);
  int n = 100;
  float t = 30;
  float dt = 0.1;
  for (int i = 0; i <= n; i++, t += dt) {
    if (rotate) {
      glVertex3f(x + r * cos(t), y + r * sin(t), z);
    } else {
      glVertex3f(x, y + r * sin(t), z + r * cos(t));
    }
  }
  glEnd();
}

// Disegna un "obiettivo" per lo score, con 3 cerchi
void drawGoalCircle(float x, float y, float z, float r) {
  drawCircle(x, y, z, r, false);
  drawCircle(x, y, z, r / 1.5, false);
  drawCircle(x, y, z, r / 4, false);
}

// Disegna la lista di obiettivi
void drawGoals(int num) {
  for (int i = 0; i < num; i++) {
    GoalCircle goal = goals[i];
    if (!goal.done) {
      drawGoalCircle(goal.x, goal.y, goal.z, goal.r);
    }
  }
}


void Controller::Init() {
  for (int i = 0; i < NKEYS; i++) key[i] = false;
}

void Car::Init() {
  goalsReached = 0;
  totalGoals = goalsNumber;
  // inizializzo lo stato della macchina
  px = pz = facing = 0; // posizione e orientamento
  py = 0.0;

  mozzoA = mozzoP = sterzo = 0; // stato
  vx = vy = vz = 0; // velocita' attuale
  // inizializzo la struttura di controllo
  controller.Init();

  //velSterzo=3.4;         // A
  velSterzo = 2.4;       // A
  velRitornoSterzo = 0.93; // B, sterzo massimo = A*B / (1-B)

  accMax = 0.0011;

  // attriti: percentuale di velocita' che viene mantenuta
  // 1 = no attrito
  // <<1 = attrito grande
  attritoZ = 0.991;  // piccolo attrito sulla Z (nel senso di rotolamento delle ruote)
  attritoX = 0.8;  // grande attrito sulla X (per non fare slittare la macchina)
  attritoY = 1.0;  // attrito sulla y nullo

  // Nota: vel max = accMax*attritoZ / (1-attritoZ)

  raggioRuotaA = 0.25;
  raggioRuotaP = 0.35;

  grip = 0.45; // quanto il facing macchina si adegua velocemente allo sterzo
}


// attiva una luce di openGL per simulare un faro della macchina
void Car::DrawHeadlight(float x, float y, float z, int lightN, bool useHeadlight) const {
  int usedLight = GL_LIGHT1 + lightN;

  if (useHeadlight)
  {
    glEnable(usedLight);
    float col0[4] = {0.8, 0.8, 0.0,  1};
    glLightfv(usedLight, GL_DIFFUSE, col0);

    float col1[4] = {0.5, 0.5, 0.0,  1};
    glLightfv(usedLight, GL_AMBIENT, col1);

    float tmpPos[4] = {x, y, z,  1}; // ultima comp=1 => luce posizionale
    glLightfv(usedLight, GL_POSITION, tmpPos );

    float tmpDir[4] = {0, 0, -1,  0}; // ultima comp=1 => luce posizionale
    glLightfv(usedLight, GL_SPOT_DIRECTION, tmpDir );

    glLightf (usedLight, GL_SPOT_CUTOFF, 30);
    glLightf (usedLight, GL_SPOT_EXPONENT, 1);

    glLightf(usedLight, GL_CONSTANT_ATTENUATION, 0);
    glLightf(usedLight, GL_LINEAR_ATTENUATION, 1);
  }
  else
    glDisable(usedLight);
}


// funzione che disegna tutti i pezzi della macchina
// (carlinga, + 4 route)
// (da invocarsi due volte: per la macchina, e per la sua ombra)
// (se usecolor e' falso, NON sovrascrive il colore corrente
//  e usa quello stabilito prima di chiamare la funzione)
void Car::RenderAllParts(bool usecolor, bool allParts) const {

  // drawCarlinga(); // disegna la carliga con pochi parallelepidedi

  // disegna la carliga con una mesh
  glPushMatrix();
  glScalef(-0.05, 0.05, -0.05); // patch: riscaliamo la mesh di 1/10
  // Codice per piegare
  // glTranslate(  asta.Center() );
  // glRotatef( -1 * sterzo/2, 0, 0, 1);
  // glTranslate( -asta.Center() );

  if (!useEnvmap)
  {
    if (usecolor) glColor3f(1, 0, 0);   // colore rosso, da usare con Lighting
  }
  else {
    if (usecolor) SetupEnvmapTextureRed();
  }
  carlinga.RenderNxV(); // rendering delle mesh carlinga usando normali per vertice
  if (usecolor) glEnable(GL_LIGHTING);

  // Se all parts è false non disegno tutto perché sto facendo la mappa
  if (allParts) {
    glPushMatrix();
    glTranslate(  asta.Center() );
    glRotatef( 1 * sterzo, 0, 1, 0);
    //glRotatef(-1 * mozzoA, 1, 0, 0);
    glTranslate( -asta.Center() );
    glColor3f(0, 0, 0);
    brakes.RenderNxV();
    asta.RenderNxV();
    glPopMatrix();

    glPushMatrix();
    glColor3f(0, 0, 0);
    brake_block.RenderNxV();
    asta_brake.RenderNxV();
    antenna.RenderNxV();
    glPopMatrix();

    if (!useEnvmap)
    {
      if (usecolor) glColor3f(1, 0, 0);   // colore rosso, da usare con Lighting
    }
    else {
      if (usecolor) SetupEnvmapTextureRed();
    }

    backpiruli.RenderNxV();
    board.RenderNxV();
    interni.RenderNxV();

    lights.RenderNxV();
    marmitta.RenderNxV();
    parafango.RenderNxV();
    piruli.RenderNxV();
    portapacchi_piruli.RenderNxV();
    shades.RenderNxV();
    if (usecolor) glEnable(GL_LIGHTING);


    if (!useEnvmap)
    {
      if (usecolor) glColor3f(1, 0, 0);   // colore rosso, da usare con Lighting
    }
    else {
      if (usecolor) SetupEnvmapTextureBrown();
    }
    bars.RenderNxV();
    if (usecolor) glEnable(GL_LIGHTING);

    if (!useEnvmap)
    {
      if (usecolor) glColor3f(1, 0, 0);   // colore rosso, da usare con Lighting
    }
    else {
      if (usecolor) SetupEnvmapTextureGlass();
    }
    glasses.RenderNxV();
    mirrors.RenderNxV();
    if (usecolor) glEnable(GL_LIGHTING);


    if (!useEnvmap)
    {
      if (usecolor) glColor3f(1, 0, 0);   // colore rosso, da usare con Lighting
    }
    else {
      if (usecolor) SetupEnvmapTextureLightLeather();
    }
    lateral.RenderNxV();
    bottomsits.RenderNxV();
    if (usecolor) glEnable(GL_LIGHTING);

    if (!useEnvmap)
    {
      if (usecolor) glColor3f(1, 0, 0);   // colore rosso, da usare con Lighting
    }
    else {
      if (usecolor) SetupEnvmapTextureDarkLeather();
    }
    backsits.RenderNxV();
    if (usecolor) glEnable(GL_LIGHTING);



    for (int i = 0; i < 2; i++) {
      // i==0 -> disegno ruote destre.
      // i==1 -> disegno ruote sinistre.
      int sign;
      if (i == 0) sign = 1;
      else sign = -1;

      /*
      ** Inizio a disegnare la ruota davanti
      **/

      if (i == 0) {
        glPushMatrix();
        // if (i == 1) {
        //   glTranslatef(0, +wheelFR1.Center().Y(), 0);
        //   glRotatef(180, 0, 0, 1 );
        //   glTranslatef(0, -wheelFR1.Center().Y(), 0);
        // }

        glTranslatef(-4.7, +wheelFR1.Center().Y() - 0.5, 0 + 5);
        glTranslatef(-4.7, -wheelFR1.Center().Y() - 0.5, 0 + 5);

        glTranslate(  wheelFR1.Center() );
        glRotatef( sign * sterzo, 0, 1, 0);
        glRotatef(-sign * mozzoA, 1, 0, 0);
        glTranslate( -wheelFR1.Center() );

        if (usecolor) glColor3f(.6, .6, .6);
        if (usecolor) SetupWheelTexture(wheelFR1.bbmin, wheelFR1.bbmax);
        wheelFR1.RenderNxF(); // la ruota viene meglio FLAT SHADED - normali per faccia
        // provare x credere
        glDisable(GL_TEXTURE_2D);
        if (usecolor) glColor3f(0.9, 0.9, 0.9);
        wheelFR2.RenderNxV();
        glPopMatrix();
      }

      /*
      ** Inizio a disegnare la ruota dietro
      **/
      glPushMatrix();
      if (i == 1) {
        glTranslatef(0, +wheelBR1.Center().Y(), 0);
        glRotatef(180, 0, 0, 1 );
        glTranslatef(0, -wheelBR1.Center().Y(), 0);
      }

      if (i == 0) {
        glTranslatef(2, +wheelFR1.Center().Y(), 0 - 2);
        glTranslatef(2, -wheelFR1.Center().Y(), 0 - 2);
      }

      if (i == 1) {
        glTranslatef(3.5, +wheelFR1.Center().Y(), 0 - 2);
        glTranslatef(3.5, -wheelFR1.Center().Y(), 0 - 2);
      }

      glTranslate(  wheelBR1.Center() );
      glRotatef(-sign * mozzoA, 1, 0, 0);
      glTranslate( -wheelBR1.Center() );

      if (usecolor) glColor3f(.6, .6, .6);
      if (usecolor) SetupWheelTexture(wheelBR1.bbmin, wheelBR1.bbmax);
      wheelBR1.RenderNxF();
      glDisable(GL_TEXTURE_2D);
      if (usecolor) glColor3f(0.9, 0.9, 0.9);
      wheelBR2.RenderNxV();
      glPopMatrix();
    }
  }
  glPopMatrix();
}

void Car::Render() const {
  Car::Render(true);
}
// disegna a schermo
void Car::Render(bool allParts) const {
  // sono nello spazio mondo
  //drawAxis(); // disegno assi spazio mondo
  glPushMatrix();

  glTranslatef(px, py, pz);
  glRotatef(facing, 0, 1, 0);

  // sono nello spazio MACCHINA
  //  drawAxis(); // disegno assi spazio macchina

  DrawHeadlight(0, 2.2, -1, 0, useHeadlight); // accendi faro centrale

  RenderAllParts(true, allParts);

  // ombra!
  if (useShadow)
  {
    glColor3f(0.4, 0.4, 0.4); // colore fisso
    glTranslatef(0, 0.01, 0); // alzo l'ombra di un epsilon per evitare z-fighting con il pavimento
    glScalef(1.01, 0, 1.01); // appiattisco sulla Y, ingrandisco dell'1% sulla Z e sulla X
    glDisable(GL_LIGHTING); // niente lighing per l'ombra
    RenderAllParts(false, allParts);  // disegno la macchina appiattita

    glEnable(GL_LIGHTING);
  }
  glPopMatrix();

  glPopMatrix();
}

void Car::checkCollision(float px, float pz) {
  int reached = 0;

  for (int i = 0; i < totalGoals; i++) {
    GoalCircle goal = goals[i];
    if (goal.done) {
      continue;
    }

    // 10/2 perché mi assicuro di essere all'interno del cerchio e non a lato
    bool isInBoundX = px < goal.x + 3 && px > goal.x - 3;
    bool isInBoundZ = pz < goal.z + 3 && pz > goal.z - 3;

    if (isInBoundX && isInBoundZ) {
      printf("collision %d\n", i);
      goal.done = true;
      goals[i] = goal;
      reached++;
    }
  }

  goalsReached += reached;
}

void Car::resetScore() {
  setupGoals();
  totalGoals = goalsNumber;
  goalsReached = 0;
  for (int i = 0; i < totalGoals; i++) {
    GoalCircle goal = goals[i];
    goal.done = false;
    goals[i] = goal;
  }
}

void Car::updateDifficultyLevel() {
  totalGoals *= 1.5;
  goalsReached = 0;
  goals[totalGoals];

  const int MIN = -90;
  const int MAX = 90;

  for (int i = 0; i < totalGoals; i++) {
    int ranX = MIN + (rand() % (int)(MAX - MIN + 1));
    int ranZ = MIN + (rand() % (int)(MAX - MIN + 1));
    GoalCircle goal = GoalCircle(ranX, 10, ranZ, 10);
    goal.done = false;
    goals[i] = goal;
  }
}