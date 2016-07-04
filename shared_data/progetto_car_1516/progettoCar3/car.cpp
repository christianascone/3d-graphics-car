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

// var globale di tipo mesh
Mesh carlinga((char *)"ape_mod.obj"); // chiama il costruttore
Mesh wheelBR1((char *)"Ferrari_wheel_back_R.obj");
Mesh wheelFR1((char *)"Ferrari_wheel_front_R.obj");
Mesh wheelBR2((char *)"Ferrari_wheel_back_R_metal.obj");
Mesh wheelFR2((char *)"Ferrari_wheel_front_R_metal.obj");

// da invocare quando e' stato premuto/rilasciato il tasto numero "keycode"
void Controller::EatKey(int keycode, int* keymap, bool pressed_or_released)
{
  for (int i=0; i<NKEYS; i++)
    if (keycode==keymap[i]) key[i]=pressed_or_released;
}

// da invocare quando e' stato premuto/rilasciato jbutton"
void Controller::Joy(int keymap, bool pressed_or_released)
{
    key[keymap]=pressed_or_released;
}

// DoStep: facciamo un passo di fisica (a delta-t costante)
//
// Indipendente dal rendering.
//
// ricordiamoci che possiamo LEGGERE ma mai SCRIVERE
// la struttura controller da DoStep
void Car::DoStep(){
  // computiamo l'evolversi della macchina
  
  float vxm, vym, vzm; // velocita' in spazio macchina
  
  // da vel frame mondo a vel frame macchina
  float cosf = cos(facing*M_PI/180.0);
  float sinf = sin(facing*M_PI/180.0);
  vxm = +cosf*vx - sinf*vz;
  vym = vy;
  vzm = +sinf*vx + cosf*vz;
  
  // gestione dello sterzo
  if (controller.key[Controller::LEFT]) sterzo+=velSterzo;
  if (controller.key[Controller::RIGHT]) sterzo-=velSterzo;
  sterzo*=velRitornoSterzo; // ritorno a volante fermo
  
  if (controller.key[Controller::ACC]) vzm-=accMax;// accelerazione in avanti 
  if (controller.key[Controller::DEC]) vzm+=accMax;// accelerazione indietro
  
  // attriti (semplificando)
  vxm*=attritoX;  
  vym*=attritoY;
  vzm*=attritoZ;
  
  // l'orientamento della macchina segue quello dello sterzo
  // (a seconda della velocita' sulla z)
  facing = facing - (vzm*grip)*sterzo;
  
  // rotazione mozzo ruote (a seconda della velocita' sulla z)
  float da ; //delta angolo
  da=(360.0*vzm)/(2.0*M_PI*raggioRuotaA);
  mozzoA+=da;
  da=(360.0*vzm)/(2.0*M_PI*raggioRuotaP);
  mozzoP+=da;
  
  // ritorno a vel coord mondo
  vx = +cosf*vxm + sinf*vzm;
  vy = vym;
  vz = -sinf*vxm + cosf*vzm;
  
  // posizione = posizione + velocita * delta t (ma e' delta t costante)
  px+=vx;
  py+=vy;
  pz+=vz;
} 

//void drawCube(); // questa e' definita altrove (quick hack)
void drawAxis(); // anche questa

/*
// disegna una ruota come due cubi intersecati a 45 gradi
void drawWheel(){
  glPushMatrix();
  glScalef(1, 1.0/sqrt(2.0),  1.0/sqrt(2.0));
  drawCube();
  glRotatef(45,  1,0,0);
  drawCube();
  glPopMatrix();
}
*/

void Controller::Init(){
  for (int i=0; i<NKEYS; i++) key[i]=false;
}

void Car::Init(){
  // inizializzo lo stato della macchina
  px=pz=facing=0; // posizione e orientamento
  py=0.0;
  
  mozzoA=mozzoP=sterzo=0;   // stato
  vx=vy=vz=0;      // velocita' attuale
  // inizializzo la struttura di controllo
  controller.Init();
  
  velSterzo=3.4;         // A
  velRitornoSterzo=0.93; // B, sterzo massimo = A*B / (1-B)
  
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

/*
// disegna carlinga composta da 1 cubo traslato e scalato
static void drawCarlinga(){
  
  glColor3f(1,0,0);
  
  // sono nel frame CAR
  glPushMatrix();
  
  // vado al frame pezzo_A
  glScalef(0.25 , 0.14 , 1);
  drawCube();  
  
  // torno al frame CAR
  glPopMatrix();

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
}
*/

// attiva una luce di openGL per simulare un faro della macchina
void Car::DrawHeadlight(float x, float y, float z, int lightN) const{
  int usedLight=GL_LIGHT1 + lightN;
  
  glEnable(usedLight);
  
  float col0[4]= {0.8,0.8,0.0,  1};
  glLightfv(usedLight, GL_DIFFUSE, col0);
  
  float col1[4]= {0.5,0.5,0.0,  1};
  glLightfv(usedLight, GL_AMBIENT, col1);
   
  float tmpPos[4] = {x,y,z,  1}; // ultima comp=1 => luce posizionale
  glLightfv(usedLight, GL_POSITION, tmpPos );
  
  float tmpDir[4] = {0,0,-1,  0}; // ultima comp=1 => luce posizionale
  glLightfv(usedLight, GL_SPOT_DIRECTION, tmpDir );
  
  glLightf (usedLight, GL_SPOT_CUTOFF, 30);
  glLightf (usedLight, GL_SPOT_EXPONENT,5);
  
  glLightf(usedLight,GL_CONSTANT_ATTENUATION,0);
  glLightf(usedLight,GL_LINEAR_ATTENUATION,1);
}

// disegna a schermo
void Car::Render() const{
  // sono nello spazio mondo
  
  //drawAxis(); // disegno assi spazio mondo
  glPushMatrix();
     
  glTranslatef(px,py,pz);
  glRotatef(facing, 0,1,0);

  // sono nello spazio MACCHINA
  //drawAxis(); // disegno assi spazio macchina
  
//  DrawHeadlight(-0.3,0,-1, 0); // accendi faro sinistro
//  DrawHeadlight(+0.3,0,-1, 1); // accendi faro destro

  // drawCarlinga(); // disegna la carliga con pochi parallelepidedi
  
  // disegna la carliga con una mesh
  glPushMatrix();
  glScalef(-0.05,0.05,-0.05); // patch: riscaliamo la mesh di 1/10 
  glTranslatef(px,-wheelBR1.bbmin.Y(),0);
  glColor3f(1,0,0); // colore rosso
  carlinga.Render();
  
  
  for (int i=0; i<2; i++) {
    // i==0 -> disegno ruote destre.
    // i==1 -> disegno ruote sinistre.
    int sign;
    if (i==0) sign=1; else sign=-1;
    glPushMatrix();
      if (i==1) {
        glTranslatef(0,+wheelFR1.Center().Y(), 0);
        glRotatef(180, 0,0,1 );
        glTranslatef(0,-wheelFR1.Center().Y(), 0);
      }
      
      glTranslate(  wheelFR1.Center() );
      glRotatef( sign*sterzo,0,1,0);
      glRotatef(-sign*mozzoA,1,0,0);
      glTranslate( -wheelFR1.Center() );
      
      glColor3f(.2,.2,.2);
      wheelFR1.Render();
      glColor3f(0.9,0.9,0.9);
      wheelFR2.Render();
    glPopMatrix();
  
    glPushMatrix();
      if (i==1) {
        glTranslatef(0,+wheelBR1.Center().Y(), 0);
        glRotatef(180, 0,0,1 );
        glTranslatef(0,-wheelBR1.Center().Y(), 0);
      }

      glTranslate(  wheelBR1.Center() );
      glRotatef(-sign*mozzoA,1,0,0);
      glTranslate( -wheelBR1.Center() );

      glColor3f(.2,.2,.2);
      wheelBR1.Render();
      glColor3f(0.9,0.9,0.9);
      wheelBR2.Render();
    glPopMatrix();
  }
  glPopMatrix();

  glColor3f(.4,.4,.4);
  
  /*
//vecchio codice ora commentato
  // ruota posteriore D
  glPushMatrix();
  glTranslatef( 0.58,+raggioRuotaP-0.28,+0.8);
  glRotatef(mozzoP,1,0,0);
  // SONO NELLO SPAZIO RUOTA
  glScalef(0.1, raggioRuotaP, raggioRuotaP);
  drawWheel();
  glPopMatrix();
  
  // ruota posteriore S
  glPushMatrix();
  glTranslatef(-0.58,+raggioRuotaP-0.28,+0.8);
  glRotatef(mozzoP,1,0,0);
  glScalef(0.1, raggioRuotaP, raggioRuotaP);
  drawWheel();
  glPopMatrix();
 
  // ruota anteriore D
  glPushMatrix();
  glTranslatef( 0.58,+raggioRuotaA-0.28,-0.55);
  glRotatef(sterzo,0,1,0);
  glRotatef(mozzoA,1,0,0);
  glScalef(0.08, raggioRuotaA, raggioRuotaA);
  drawWheel();
  glPopMatrix();
  
  // ruota anteriore S
  glPushMatrix();
  glTranslatef(-0.58,+raggioRuotaA-0.28,-0.55);
  glRotatef(sterzo,0,1,0);
  glRotatef(mozzoA,1,0,0);
  drawAxis();
  glScalef(0.08, raggioRuotaA, raggioRuotaA);
  drawWheel();
  glPopMatrix(); 
  */
  
  glPopMatrix();
}
