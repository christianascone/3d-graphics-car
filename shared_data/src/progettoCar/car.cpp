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
Mesh antenna((char *)"ape/obj/antenna.obj");
Mesh asta((char *)"ape/obj/asta.obj");
Mesh asta_brake((char *)"ape/obj/asta_brake.obj");
Mesh backdetails((char *)"ape/obj/backdetails.obj");
Mesh backsits((char *)"ape/obj/backsits.obj");
Mesh bars((char *)"ape/obj/bars.obj");
Mesh board((char *)"ape/obj/board.obj");
Mesh bottomsits((char *)"ape/obj/bottomsits.obj");
Mesh brakes((char *)"ape/obj/brakes.obj");
Mesh brake_block((char *)"ape/obj/brake_block.obj");
Mesh carlinga((char *)"ape/obj/carlinga.obj");
Mesh glasses((char *)"ape/obj/glass.obj");
Mesh interni((char *)"ape/obj/interni.obj");
Mesh lateral((char *)"ape/obj/lateral.obj");
Mesh lights((char *)"ape/obj/lights.obj");
Mesh marmitta((char *)"ape/obj/marmitta.obj");
Mesh mirrors((char *)"ape/obj/mirrors.obj");
Mesh parafango((char *)"ape/obj/parafango.obj");
Mesh details((char *)"ape/obj/details.obj");
Mesh portapacchi_details((char *)"ape/obj/portapacchi_details.obj");
Mesh shades((char *)"ape/obj/shades.obj");

// Ruote
Mesh wheelBR1((char *)"obj/wheel_back_R.obj");
Mesh wheelFR1((char *)"obj/wheel_front_R.obj");
Mesh wheelBR2((char *)"obj/wheel_back_R_metal.obj");
Mesh wheelFR2((char *)"obj/wheel_front_R_metal.obj");

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

// Struct che rappresenta un cerchio Goal
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
extern bool useTransparency; // var globale esterna: per usare i fari
extern bool useShadow; // var globale esterna: per generare l'ombra

// Goals iniziali
const int goalsNumber = 4;
GoalCircle goals[goalsNumber];

// Setup iniziale dei goals
void setupGoals() {
  goals[goalsNumber];
  goals[0] = GoalCircle(6, 10, -47, 10);
  goals[1] = GoalCircle(-66, 10, -37, 10);
  goals[2] = GoalCircle(-21, 10, 48, 10);
  goals[3] = GoalCircle(26, 10, 30, 10);

  for (int i = 0; i < goalsNumber; i++) {
    GoalCircle goal = goals[i];
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

  if (useTransparency) {
    // Abilito la trasparenza
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f (0.5, 0.5, 0.5, 0.3); // fattore alpha 0.3
  }
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

// funzione che prepara tutto per creare le coordinate texture (s,t) da (x,y,z)
// Mappo l'intervallo [ minY , maxY ] nell'intervallo delle T [0..1]
//     e l'intervallo [ minZ , maxZ ] nell'intervallo delle S [0..1]
void SetupWheelTexture(Point3 min, Point3 max) {
  glBindTexture(GL_TEXTURE_2D, 0);
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_TEXTURE_GEN_S);
  glEnable(GL_TEXTURE_GEN_T);

  // utilizzo le coordinate OGGETTO
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
void SetupPhotoTexture(Point3 min, Point3 max, bool loser, bool flipHorizontal) {
  // Seleziono la texture in base a "loser"
  if (!loser) {
    glBindTexture(GL_TEXTURE_2D, 9);
  } else {
    glBindTexture(GL_TEXTURE_2D, 14);
  }
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_TEXTURE_GEN_S);
  glEnable(GL_TEXTURE_GEN_T);

  // utilizzo le coordinate OGGETTO
  // cioe' le coordnate originali, PRIMA della moltiplicazione per la ModelView
  // in modo che la texture sia "attaccata" all'oggetto, e non "proiettata" su esso
  glTexGeni(GL_S, GL_TEXTURE_GEN_MODE , GL_OBJECT_LINEAR);
  glTexGeni(GL_T, GL_TEXTURE_GEN_MODE , GL_OBJECT_LINEAR);

  int flip = 1;
  if (flipHorizontal) {
    flip = -1;
  }

  float sx = flip * 1.0 / (max.X() - min.X()); // Inverto per flip sx - dx

  float ty = 1.0 / (min.Y() - max.Y()); // Inverto per flip up - down

  float s[4] = {sx, 0, 0,  - min.X()*sx };
  float t[4] = {0, ty, 0,  - min.Y()*ty };

  glTexGenfv(GL_S, GL_OBJECT_PLANE, s);
  glTexGenfv(GL_T, GL_OBJECT_PLANE, t);
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

  // Attrito per il freno
  if (controller.key[Controller::BRAKE]) {
    freno += velFreno;
    if (freno > 45) {
      freno = 45;
    }
    // Applico maggiore attrito
    vzm *= 0.9;
    vxm *= 0.8;
    vym *= 1.0;
  } else {
    // attirti (semplificando)
    vzm *= attritoZ;
    vxm *= attritoX;
    vym *= attritoY;
  }
  freno *= velRitornoFreno;

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


// Disegno la pista con la mesh
void drawPista () {
  glPushMatrix();
  glColor3f(0.5, 0.5, 0.5);
  glScalef(0.70, 0.9, 0.70);
  glTranslatef(0, 0.01, 0);
  pista.RenderNxF();
  glPopMatrix();
}

// Disegno l'albero
void drawTree (bool shadow) {
  glPushMatrix();

  // Setup della texture dei tronchi
  if (!useEnvmap)
  {
    if (!shadow) glColor3f(0.1f, 0.0f, 0.0f);//Brown
  }
  else {
    if (!shadow) SetupEnvmapTextureTree();
  }
  tree1.RenderNxV();
  tree3.RenderNxV();
  //if (!shadow) glEnable(GL_LIGHTING);

  // Setup della texture per le foglie dell'albero
  if (!useEnvmap)
  {
    if (!shadow) glColor3f(0, 1, 0);
  }
  else {
    if (!shadow) SetupEnvmapTextureLeaf();
  }
  tree2.RenderNxV();
  //if (shadow) glEnable(GL_LIGHTING);
  glDisable(GL_TEXTURE_2D);
  glPopMatrix();

}

// Disegna il cartellone pubblicitario con l'immagine
void drawBillboard (bool shadow, bool loser) {
  glPushMatrix();
  glTranslatef(30, 0, 0);
  if (!useEnvmap)
  {
    if (!shadow) glColor3f(0.1f, 0.1f, 0.1f);
  }
  else {
    if (!shadow) glColor3f(0.5, 0.5, 0.5);

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
  //if (shadow) glEnable(GL_LIGHTING);

  if (!useEnvmap)
  {
    if (!shadow) glColor3f(1, 1, 1);
  }
  else {
    if (!shadow) glColor3f(1, 1, 1);
    if (!shadow) SetupPhotoTexture(billboard_face1.bbmin, billboard_face1.bbmax, loser, false);
  }
  billboard_face1.RenderNxV();
  if (!useEnvmap)
  {
    if (!shadow) glColor3f(1, 1, 1);
  }
  else {
    if (!shadow) glColor3f(1, 1, 1);
    if (!shadow) SetupPhotoTexture(billboard_face2.bbmin, billboard_face2.bbmax, loser, true);
  }
  billboard_face2.RenderNxV();
  //if (shadow) glEnable(GL_LIGHTING);
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

// Disegna un quadraato con coordinate e colore passati come parametri
void drawGoalSquare(float x, float y, float z, float r, float g, float b) {
  glDisable(GL_LIGHTING);
  glPushMatrix();
  glTranslatef(x, y, z);
  glScalef(3, 0, 3);

  glBegin(GL_QUADS);                // Begin drawing the color cube with 6 quads
  // Top face (y = 1.0f)
  glColor3f(r, g, b);
  glVertex3f( 1.0f, 1.0f, -1.0f);
  glVertex3f(-1.0f, 1.0f, -1.0f);
  glVertex3f(-1.0f, 1.0f,  1.0f);
  glVertex3f( 1.0f, 1.0f,  1.0f);

  // Bottom face (y = -1.0f)
  glVertex3f( 1.0f, -1.0f,  1.0f);
  glVertex3f(-1.0f, -1.0f,  1.0f);
  glVertex3f(-1.0f, -1.0f, -1.0f);
  glVertex3f( 1.0f, -1.0f, -1.0f);

  // Front face  (z = 1.0f)
  glVertex3f( 1.0f,  1.0f, 1.0f);
  glVertex3f(-1.0f,  1.0f, 1.0f);
  glVertex3f(-1.0f, -1.0f, 1.0f);
  glVertex3f( 1.0f, -1.0f, 1.0f);

  // Back face (z = -1.0f)
  glVertex3f( 1.0f, -1.0f, -1.0f);
  glVertex3f(-1.0f, -1.0f, -1.0f);
  glVertex3f(-1.0f,  1.0f, -1.0f);
  glVertex3f( 1.0f,  1.0f, -1.0f);

  // Left face (x = -1.0f)
  glVertex3f(-1.0f,  1.0f,  1.0f);
  glVertex3f(-1.0f,  1.0f, -1.0f);
  glVertex3f(-1.0f, -1.0f, -1.0f);
  glVertex3f(-1.0f, -1.0f,  1.0f);

  // Right face (x = 1.0f)
  glVertex3f(1.0f,  1.0f, -1.0f);
  glVertex3f(1.0f,  1.0f,  1.0f);
  glVertex3f(1.0f, -1.0f,  1.0f);
  glVertex3f(1.0f, -1.0f, -1.0f);
  glEnd();  // End of drawing color-cube

  glPopMatrix();
}

// Disegna la lista di obiettivi
void drawGoals(int num, bool isForMap) {
  for (int i = 0; i < num; i++) {
    GoalCircle goal = goals[i];
    if (!goal.done) {
      if (!isForMap) {
        drawGoalCircle(goal.x, goal.y, goal.z, goal.r);
      } else {
        drawGoalSquare(goal.x, goal.y, goal.z, 1, 0, 0);
      }
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
  velFreno = 4.4;       // A
  velRitornoFreno = 0.93; // B, sterzo massimo = A*B / (1-B)

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

  if (!useEnvmap)
  {
    if (usecolor) glColor3f(1, 0, 0);
  }
  else {
    if (usecolor) SetupEnvmapTextureRed();
  }
  carlinga.RenderNxV(); // rendering delle mesh carlinga usando normali per vertice
  lights.RenderNxV();
  parafango.RenderNxV();
  shades.RenderNxV();
  glDisable(GL_TEXTURE_2D);
  if (usecolor) glEnable(GL_LIGHTING);

  // Se all parts è false non disegno tutto perché sto facendo la mappa
  if (allParts) {
    glPushMatrix();
    glTranslate(  asta.Center() );
    glRotatef( 1 * sterzo, 0, 1, 0);
    glTranslate( -asta.Center() );
    glColor3f(0, 0, 0);
    brakes.RenderNxV();
    asta.RenderNxV();
    glPopMatrix();


    if (usecolor) glColor3f(0.1f, 0.1f, 0.1f);
    lateral.RenderNxV();
    bottomsits.RenderNxV();
    marmitta.RenderNxV();
    mirrors.RenderNxV();
    //if (usecolor) glEnable(GL_LIGHTING);

    if (usecolor) glColor3f(0.0f, 0.0f, 0.0f);
    backsits.RenderNxV();
    //if (usecolor) glEnable(GL_LIGHTING);



    glPushMatrix();
    if (controller.key[Controller::BRAKE]) {
      glTranslate(  asta_brake.bbmin );
      glRotatef( 1 * -freno, 1, 0, 0);
      glTranslate( -asta_brake.bbmin );
    }
    asta_brake.RenderNxV();
    glPopMatrix();


    glColor3f(0, 0, 0);
    brake_block.RenderNxV();
    antenna.RenderNxV();
    portapacchi_details.RenderNxV();



    if (usecolor) glColor3f(0.3, 0.3, 0.3);
    board.RenderNxV();
    interni.RenderNxV();


    if (!useEnvmap)
    {
      if (usecolor) glColor3f(0, 0, 0);
    }
    else {
      if (usecolor) SetupEnvmapTextureDarkLeather();
    }
    details.RenderNxV();
    backdetails.RenderNxV();
    if (usecolor) glEnable(GL_LIGHTING);


    if (!useEnvmap)
    {
      if (usecolor) glColor3f(0.1f, 0.0f, 0.0f);//Brown
    }
    else {
      if (usecolor) SetupEnvmapTextureBrown();
    }
    bars.RenderNxV();
    if (usecolor) glEnable(GL_LIGHTING);

    if (useEnvmap)
    {
      if (usecolor) SetupEnvmapTextureGlass();
      glasses.RenderNxV();
      if (usecolor) glEnable(GL_LIGHTING);
    }


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
  Car::Render(true, false);
}
// disegna a schermo
void Car::Render(bool allParts, bool isForMap) const {
  if (isForMap) {
    drawGoalSquare(px, py, pz, 0, 0, 0);
    return;
  }
// sono nello spazio mondo
//drawAxis(); // disegno assi spazio mondo
  glPushMatrix();

  glTranslatef(px, py, pz);
  glRotatef(facing, 0, 1, 0);

// sono nello spazio MACCHINA
//  drawAxis(); // disegno assi spazio macchina


  RenderAllParts(true, allParts);

// ombra!
  if (useShadow)
  {
    glColor3f(0.1, 0.1, 0.1); // colore fisso
    glTranslatef(0, 0.01, 0); // alzo l'ombra di un epsilon per evitare z-fighting con il pavimento
    glScalef(1.01, 0, 1.01); // appiattisco sulla Y, ingrandisco dell'1% sulla Z e sulla X
    glDisable(GL_LIGHTING); // niente lighing per l'ombra
    RenderAllParts(false, allParts);  // disegno la macchina appiattita

    glEnable(GL_LIGHTING);
  }
  glPopMatrix();

  glPopMatrix();
}

// Verifica la collisione tra l'oggetto car e tutti goals
void Car::checkCollision(float px, float pz) {
  int reached = 0;

  for (int i = 0; i < totalGoals; i++) {
    GoalCircle goal = goals[i];
    if (goal.done) {
      continue;
    }

    int tolerance = 3;
    bool isInBoundX = px < goal.x + tolerance && px > goal.x - tolerance;
    bool isInBoundZ = pz < goal.z + tolerance && pz > goal.z - tolerance;

    if (isInBoundX && isInBoundZ) {
      printf("Collisione con goal %d\n", i);
      goal.done = true;
      goals[i] = goal;
      reached++;
    }
  }

  goalsReached += reached;
}

// Reset dello score e dei goals.
// Ritorna alla situazione iniziale
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

// Aggiorna la difficoltà
void Car::updateDifficultyLevel() {
  totalGoals *= 1.7;
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