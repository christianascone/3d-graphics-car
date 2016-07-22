#include <stdio.h>
#include <math.h>
#include "../GCGraLib2/GCGraLib2.h"

#define MAXVERT (100)
#define MAXEDGE (200)
#define MAXFACE (100)
#define DEFAULT_PTSIZE  12

#define AREA_ZBUFFER 594

/* Dati per visualizzare un oggetto definito solo da vertici e lati*/
float   x[MAXVERT], y[MAXVERT], z[MAXVERT];     /* coordinate vertici */
int     edge[MAXEDGE][2];                       /* lista lati */
int     face[MAXFACE][4];                       /* lista facce */
float   nx[MAXFACE], ny[MAXFACE], nz[MAXFACE];  /* coordinate vertici */
int     tx[MAXFACE][4], ty[MAXFACE][4];         /* coordinate texture */
int     nvert, nedge, nface;                    /* numero vertici, lati e facce */
int     flag_texture[MAXFACE];                  /* indicatore facce con texture */
float   csx, csy, csz;                          /* centro oggetto */
float   D = 25, teta = 20, fi = 20, di;
int     xs[MAXVERT], ys[MAXVERT];               /* coordinate schermo */
float   zeye[MAXVERT];                          /* coordinate z trasformate */

int   xvmin, xvmax, yvmin, yvmax; /* coordinate viewport */
float   salpha = 0.05;    /* zoomin e zoomout */
float   sr = 0.25, ssx, ssy, ssz, ssmod, /* modifica centro oggetto */
        Dstep = 1.0,    /* passo di incremento D */
        alpha,        /* apertura angolare piramide di vista */
        s;
float   xwmin, xwmax, ywmin, ywmax; /* coordinate window */
float   Sx, Sy;       /* fattori di scala trasf. window-viewport */

float   tmpx, tmpy;     /* variabili temporanee */
int   i, j;
int     wim, him;

typedef struct RECT RECT;
struct RECT
{
  SDL_Rect rect;
  char     text[10];
};

RECT  menu[15];
int   done = 0;

/*added variables for scanline and z-buffer */
float z_observer[MAXVERT];    /*coordinate z spazio dell'osservatore */
float z_prosp_corr[MAXVERT];   /* coordinate con correzione prospettica */
typedef float rvector[4];
rvector yymax, yymin, xa, dx;
rvector za, dz;
int edges, startedge, endedge, scandec;
float scan;

float near = 20; // near plane
float far = 30;  // far plane
float B_correction;
float A_correction;
int polx[4], poly[4];
float polz[4];
float polz_e[4], polx_e[4], poly_e[4];
float z_buffer[AREA_ZBUFFER][AREA_ZBUFFER];

int xim0, yim0, xim1, yim1, xim2, yim2;
SDL_Surface *tux;

/* inizializzazione menu */
void init_menu(SDL_Renderer *ren, RECT menu[], float teta, float fi) {

  menu[0].rect.x = 605;
  menu[0].rect.y = 550;
  menu[0].rect.w = 100;
  menu[0].rect.h = 30;
  strcpy(menu[0].text, "QUIT");
  // finewin = 0;

  menu[1].rect.x = 605;
  menu[1].rect.y = 20;
  menu[1].rect.w = 100;
  menu[1].rect.h = 100;
  strcpy(menu[1].text, "THETA");
  // tetawin=1;

  menu[2].rect.x = 605;
  menu[2].rect.y = 150;
  menu[2].rect.w = 100;
  menu[2].rect.h = 100;
  strcpy(menu[2].text, "PHI");
  // fiwin = 2;

  menu[3].rect.x = 602;
  menu[3].rect.y = 280;
  menu[3].rect.w = 50;
  menu[3].rect.h = 30;
  strcpy(menu[3].text, "ZoomI");
  // ziwin = 3;

  menu[4].rect.x = 662;
  menu[4].rect.y = 280;
  menu[4].rect.w = 50;
  menu[4].rect.h = 30;
  strcpy(menu[4].text, "ZoomO");
  // zowin = 4;

  menu[5].rect.x = 602;
  menu[5].rect.y = 320;
  menu[5].rect.w = 50;
  menu[5].rect.h = 30;
  strcpy(menu[5].text, "DZoomI");
  // Dziwin = 5;

  menu[6].rect.x = 662;
  menu[6].rect.y = 320;
  menu[6].rect.w = 50;
  menu[6].rect.h = 30;
  strcpy(menu[6].text, "DZoomO");
  // Dzowin = 6;

  menu[7].rect.x = 662;
  menu[7].rect.y = 400;
  menu[7].rect.w = 50;
  menu[7].rect.h = 30;
  strcpy(menu[7].text, "RIGHT");
  // rightwin = 7;

  menu[8].rect.x = 602;
  menu[8].rect.y = 400;
  menu[8].rect.w = 50;
  menu[8].rect.h = 30;
  strcpy(menu[8].text, "LEFT");
  // leftwin = 8;

  menu[9].rect.x = 631;
  menu[9].rect.y = 360;
  menu[9].rect.w = 50;
  menu[9].rect.h = 30;
  strcpy(menu[9].text, "UP");
  // upwin = 9;

  menu[10].rect.x = 631;
  menu[10].rect.y = 440;
  menu[10].rect.w = 50;
  menu[10].rect.h = 30;
  strcpy(menu[10].text, "DOWN");
  //downwin = 10;

  SDL_RenderDrawLine(ren, menu[1].rect.x + menu[1].rect.w / 2, menu[1].rect.y + menu[1].rect.h / 2,
                     (int)(menu[1].rect.w / 2)*cos(teta) + menu[1].rect.x + menu[1].rect.w / 2,
                     (int)(menu[1].rect.h / 2)*sin(teta) + menu[1].rect.y + menu[1].rect.h / 2);
  SDL_RenderDrawLine(ren, menu[2].rect.x + menu[2].rect.w / 2, menu[2].rect.y + menu[2].rect.h / 2,
                     (int)(menu[2].rect.w / 2)*cos(fi) + menu[2].rect.x + menu[2].rect.w / 2,
                     (int)(menu[2].rect.h / 2)*sin(fi) + menu[2].rect.y + menu[2].rect.h / 2);
}

void draw_menu(RECT menu[], SDL_Renderer *ren, TTF_Font *font)
{
  int i;

  for ( i = 0 ; i <= 10 ; i++ )
  {
    GC_DrawText(ren, font, 0, 0, 0, 0, 255, 255, 255, 0, menu[i].text,
                menu[i].rect.x, menu[i].rect.y, shaded);
    //GC_DrawRect(ren,menu[i].rect.x,menu[i].rect.y,menu[i].rect.w,menu[i].rect.h);
    SDL_RenderDrawRect(ren, &(menu[i].rect));
  }
}

/* funzione di determinazione opzione del menu' scelta */
void opt_menu(RECT menu[], int n, int ix, int iy, int *choice)
{
  int i;

  *choice = -1;
  for (i = 0; i <= n; i++)
  {
    if (ix >= menu[i].rect.x && iy >= menu[i].rect.y &&
        ix - menu[i].rect.x <= menu[i].rect.w && iy - menu[i].rect.y <= menu[i].rect.h)
      *choice = i;
  }
}

float rad(float angle)
{
  return (angle / 180 * 3.1415);
}

void copy_trasl(int nv, int *nvert, float x[], float y[], float z[],
                int ne, int *nedge, int edge[][2], float dx, float dy, float dz)
{
  int i;

  for (i = 0; i < nv; i++)
  {
    x[i + *nvert] = x[i] + dx;
    y[i + *nvert] = y[i] + dy;
    z[i + *nvert] = z[i] + dz;
  }
  for (i = 0; i <= ne; i++)
  {
    edge[i + *nedge][0] = edge[i][0] + *nvert;
    edge[i + *nedge][1] = edge[i][1] + *nvert;
  }
  *nvert += nv;
  *nedge += ne;
}



/* funzione che definisce l'oggetto mesh */
void define_object()
{

  /* Determino le coordinate dei vertici dell'oggetto */
  nvert = 8;
  x[0] = -1.0; y[0] = -1.0; z[0] = -1.0;
  x[1] = 1.0; y[1] = -1.0; z[1] = -1.0;
  x[2] = 1.0; y[2] = 1.0; z[2] = -1.0;
  x[3] = -1.0; y[3] = 1.0; z[3] = -1.0;
  x[4] = -1.0; y[4] = -1.0; z[4] = 1.0;
  x[5] = 1.0; y[5] = -1.0; z[5] = 1.0;
  x[6] = 1.0; y[6] = 1.0; z[6] = 1.0;
  x[7] = -1.0; y[7] = 1.0; z[7] = 1.0;

  /* Determino la lista dei lati dell'oggetto */
  nedge = 13;
  edge[0][0] = 0; edge[0][1] = 1;
  edge[1][0] = 1; edge[1][1] = 2;
  edge[2][0] = 2; edge[2][1] = 3;
  edge[3][0] = 3; edge[3][1] = 0;
  edge[4][0] = 4; edge[4][1] = 5;
  edge[5][0] = 5; edge[5][1] = 6;
  edge[6][0] = 6; edge[6][1] = 7;
  edge[7][0] = 7; edge[7][1] = 4;
  edge[8][0] = 0; edge[8][1] = 4;
  edge[9][0] = 1; edge[9][1] = 5;
  edge[10][0] = 2; edge[10][1] = 6;
  edge[11][0] = 3; edge[11][1] = 7;
  /*diagonale faccia*/
  edge[12][0] = 1; edge[12][1] = 6;

  /*Determino la lista delle facce dell'oggetto */
  nface = 6;
  face[0][0] = 0; face[0][1] = 1; face[0][2] = 5; face[0][3] = 4;
  face[1][0] = 1; face[1][1] = 2; face[1][2] = 6; face[1][3] = 5;
  face[2][0] = 2; face[2][1] = 3; face[2][2] = 7; face[2][3] = 6;
  face[3][0] = 3; face[3][1] = 0; face[3][2] = 4; face[3][3] = 7;
  face[4][0] = 4; face[4][1] = 5; face[4][2] = 6; face[4][3] = 7;
  face[5][0] = 3; face[5][1] = 2; face[5][2] = 1; face[5][3] = 0;



  /*Definisco coordinate texture dell'immagine */
  flag_texture[0] = 1;
  tx[0][0] = wim; tx[0][1] = 0; tx[0][2] = 0; tx[0][3] = wim;
  ty[0][0] = 0; ty[0][1] = 0; ty[0][2] = him; ty[0][3] = him;
  /**/
  flag_texture[1] = 1;
  tx[1][0] = wim; tx[1][1] = 0; tx[1][2] = 0; tx[1][3] = wim;
  ty[1][0] = 0; ty[1][1] = 0; ty[1][2] = him; ty[1][3] = him;
  flag_texture[2] = 1;
  tx[2][0] = wim; tx[2][1] = 0; tx[2][2] = 0; tx[2][3] = wim;
  ty[2][0] = 0; ty[2][1] = 0; ty[2][2] = him; ty[2][3] = him;
  flag_texture[3] = 1;
  tx[3][0] = wim; tx[3][1] = 0; tx[3][2] = 0; tx[3][3] = wim;
  ty[3][0] = 0; ty[3][1] = 0; ty[3][2] = him; ty[3][3] = him;
  flag_texture[4] = 1;
  tx[4][0] = wim; tx[4][1] = 0; tx[4][2] = 0; tx[4][3] = wim;
  ty[4][0] = 0; ty[4][1] = 0; ty[4][2] = him; ty[4][3] = him;
  flag_texture[5] = 1;
  tx[5][0] = wim; tx[5][1] = 0; tx[5][2] = 0; tx[5][3] = wim;
  ty[5][0] = 0; ty[5][1] = 0; ty[5][2] = him; ty[5][3] = him;
  /**/

//copy_trasl(8,&nvert,x,y,z,13,&nedge,edge,2.0,0.0,0.0);
}


/* trasformazione prospettica a tre punti di fuga (generico punto di vista) */
void trasf_prosp_gen(int k, int *init, float x, float y, float z,
                     float *xe, float *ye, float *ze)
{
  static float steta, cteta, cfi, sfi;

  if (*init)
  {
    *init = 0;
    steta = sin(teta);
    cteta = cos(teta);
    sfi = sin(fi);
    cfi = cos(fi);
  }


  /* trasformazione in coordinate del sistema osservatore */
  *xe = -steta * x + y * cteta;
  *ye = -cteta * cfi * x - y * steta * cfi + z * sfi;

  // Osservatore Z
  z_observer[k] = *ze = -x * cteta * sfi - y * steta * sfi - z * cfi + D;

  /* scala piramide di vista */
  *xe = *xe / tan(alpha);
  *ye = *ye / tan(alpha);
  /* correzione prospettica */
  *xe = *xe / *ze;
  *ye = *ye / *ze;
  *ze = A_correction + B_correction / *ze;
}

/* funzione che determina i fattori di scala */
void define_view() {
  int k;
  float s, r;
  float xmin, xmax, ymin, ymax, zmin, zmax;

  xmin = x[0];
  ymin = y[0];
  zmin = z[0];
  xmax = xmin;
  ymax = ymin;
  zmax = zmin;
  for (k = 1; k < nvert; k++) {
    if (x[k] > xmax)
      xmax = x[k];
    else if (x[k] < xmin)
      xmin = x[k];
    if (y[k] > ymax)
      ymax = y[k];
    else if (y[k] < ymin)
      ymin = y[k];
    if (z[k] > zmax)
      zmax = z[k];
    else if (z[k] < zmin)
      zmin = z[k];
  }
  /* centro oggetto iniziale */
  csx = (xmin + xmax) / 2;
  csy = (ymin + ymax) / 2;
  csz = (zmin + zmax) / 2;
  /* raggio sfera contenete oggetto */
  r = sqrt(powf(xmax - csx, 2) + powf(ymax - csy, 2) + powf(zmax - csz, 2));
  /* calcolo semilato window iniziale; il piano di proiezione viene assunto
     ad una distanza D dall'osservatore */
  s = r * D / sqrt(powf(D, 2) - powf(r, 2));
  /* in base al semilato window cosi' calcolato si determina l'apertura
     angolare iniziale */
  alpha = atan(s / D);
  /* printf("\nApertura angolare: %f \n",alpha);*/
  /* valori per correzione prospettica */
  A_correction = far / (far - near);
  B_correction = -(near * far) / (far - near);
}

void conv(unsigned long m, int b, int a[])
{
  int p;

  a[0] = a[1] = a[2] = 0;
  p = -1;
  while (m != 0)
  {
    p++;
    a[p] = m % b;
    m = m / b;
  }
}


int max(int a, int b, int c)
{
  int m;
  m = a;
  if (b > m) m = b;
  if (c > m) m = c;
  return m;
}
int min(int a, int b, int c) {
  int m;
  m = a;
  if (b < m)
    m = b;
  if (c < m)
    m = c;
  return m;
}


float f01(float x0, float y0, float x1, float y1, float x, float y)
{
  return (y0 - y1) * x + (x1 - x0) * y + x0 * y1 - x1 * y0;
}


void clear_zbuffer() {
  for (int i = 0; i < AREA_ZBUFFER; i++) {
    for (int j = 0; j < AREA_ZBUFFER; j++) {
      // 2 serve perché pulisco z, visto che il range è tra 0 e 1
      z_buffer[i][j] = 2.0;
    }
  }
}

/*                                                                    */
/*                          POLYINSERT                                */
/*                                                                    */
/*  Questa procedura ordina i lati del poligono e memorizza le infor- */
/*  mazioni del nuovo lato;e" un ordinamento ad inserzione.           */
/*  DATI IN INGRESSO                                                  */
/*    x21 , y21 , x22 , y22 : coordinate degli estremi del lato.      */
/*                                                                    */
void polyinsert(int edges, float x21, float y21, float x22, float y22,
                float z21, float z22) {
  int j1;
  float ym;
  j1 = edges;
  if (y21 > y22) {
    ym = y21;
  } else {
    ym = y22;
  }
  while ((j1 != 1) && (yymax[j1 - 1] < ym)) {
    yymax[j1] = yymax[j1 - 1];
    yymin[j1] = yymin[j1 - 1];
    dx[j1] = dx[j1 - 1];
    xa[j1] = xa[j1 - 1];
    dz[j1] = dz[j1 - 1];
    za[j1] = za[j1 - 1];
    j1 = j1 - 1;
  }
  yymax[j1] = ym;
  dx[j1] = (x22 - x21) / (y22 - y21);
  dz[j1] = (z22 - z21) / (y22 - y21);
  if (y21 > y22) {
    yymin[j1] = y22;
    xa[j1] = x21;
    za[j1] = z21;
  } else {
    yymin[j1] = y21;
    xa[j1] = x22;
    za[j1] = z22;
  }
}

/*                                                                    */
/*                             LOADPOL                                */
/*                                                                    */
/*  Questa procedura ordina i lati del poligono secondo il valore del-*/
/*  le ordinate piu" grande.Inoltre memorizza e recupera delle infor- */
/*  mazioni relative ai lati eccetto quelli orrizontali.              */
/*                                                                    */
void loadpol(int n) {
  float x1, x2, y1, y2, z1, z2;
  int k;
  x1 = polx[n];
  y1 = poly[n];
  z1 = polz[n];
  edges = 0;
  for (k = 1; k <= n; k++) {
    x2 = polx[k];
    y2 = poly[k];
    z2 = polz[k];
    if (y1 == y2) {
      x1 = x2;
      z1 = z2;
    } else {
      edges = edges + 1;
      polyinsert(edges, x1, y1, x2, y2, z1, z2);
      x1 = x2;
      y1 = y2;
      z1 = z2;
    }
  }
}

/*                                                                    */
/*                            INCLUDE                                 */
/*                                                                    */
/*  Questa procedura aggiunge nuovi lati al gruppo che si sta conside-*/
/*  rando.                                                            */
/*                                                                    */
void includ() {
  while ((endedge <= edges) && (yymax[endedge] >= scan)) {
    dx[endedge] = dx[endedge] * (-scandec);
    dz[endedge] = dz[endedge] * (-scandec);
    endedge = endedge + 1;
  }
}

void xchange(float *a, float *b) {
  float t;

  t = *a;
  *a = *b;
  *b = t;
}

/*                                                                    */
/*                           XSORT                                    */
/*                                                                    */
/*  Questa procedura mette un elemento nella posizione corretta usando*/
/*  un algoritmo a bolla.                                             */
/*                                                                    */
void xsort(int kk) {
  int l;
  l = kk;
  while ((l > startedge) && (xa[l] < xa[l - 1])) {
    xchange(&yymin[l], &yymin[l - 1]);
    xchange(&xa[l], &xa[l - 1]);
    xchange(&dx[l], &dx[l - 1]);
    xchange(&za[l], &za[l - 1]);
    xchange(&dz[l], &dz[l - 1]);
    l = l - 1;
  }
}

/*                                                                    */
/*                        UPDATEXVALUES                               */
/*                                                                    */
/*  Questa procedura rimuove i lati che non devono essere piu" consi- */
/*  derati;inoltre determina i punti di intersezione dei restanti lati*/
/*  con la scanline.                                                  */
/*                                                                    */
void updatevalues() {
  int stopedge, beginedge, k, i;

  stopedge = endedge - 1;
  beginedge = startedge;
  for (k = beginedge; k <= stopedge; k++) {
    if (yymin[k] < scan) {
      xa[k] = xa[k] + dx[k];
      za[k] = za[k] + dz[k];
      xsort(k);
    } else {
      startedge = startedge + 1;
      if (startedge <= k)
        for (i = k; i >= startedge; i--) {
          yymin[i] = yymin[i - 1];
          xa[i] = xa[i - 1];
          dx[i] = dx[i - 1];
          za[i] = za[i - 1];
          dz[i] = dz[i - 1];
        }
    }
  }
}

/*                                                                    */
/*                               FILLSCAN                             */
/*                                                                    */
/*  Questa procedura disegna i segmenti individuati dalle intersezioni*/
/*  trovate con la procedura updatexvalues.                           */
/*                                                                    */
void fillscan(SDL_Renderer *ren) {
  int aa, bb;
  float fx, fy;
  float fz;
  int y, x1, x2;
  float z1, z2, dz;
  float alpha, beta, gamma;
  int pix[3], pix1[3], pix2[3], pix3[3], pix4[3];
  int i, j;

  if (xa[startedge + 1] - xa[startedge] >= 0.5) {
    y = (int)scan;
    x1 = (int)(xa[startedge] + 0.5);
    x2 = (int)(xa[startedge + 1] + 0.5);
    z1 = za[startedge];
    z2 = za[startedge + 1];
    dz = (z2 - z1) / (x2 - x1);
    while (x1 <= x2) {
      if (z1 < z_buffer[y][x1]) {
        z_buffer[y][x1] = z1;
        alpha = f01(polx[1], poly[1], polx[2], poly[2], x1, y) /
                f01(polx[1], poly[1], polx[2], poly[2], polx[0], poly[0]);
        beta = f01(polx[2], poly[2], polx[0], poly[0], x1, y) /
               f01(polx[2], poly[2], polx[0], poly[0], polx[1], poly[1]);
        gamma = f01(polx[0], poly[0], polx[1], poly[1], x1, y) /
                f01(polx[0], poly[0], polx[1], poly[1], polx[2], poly[2]);
        fz = alpha / polz_e[0] + beta / polz_e[1] + gamma / polz_e[2];
        alpha = alpha / polz_e[0] / fz;
        beta = beta / polz_e[1] / fz;
        gamma = gamma / polz_e[2] / fz;
        // if (alpha >= 0 && beta >= 0 && gamma >= 0) {
        fx = alpha * xim0 + beta * xim1 + gamma * xim2;
        fy = alpha * yim0 + beta * yim1 + gamma * yim2;
        j = (int)fx;
        i = (int)fy;
        aa = fx - j;
        bb = fy - i;
        if ((j + 1) <= tux->w && (i + 1) <= tux->h && i >= 0 && j >= 0) {
          conv(GC_GetPixelImage(tux, j, i), 256, pix1);
          conv(GC_GetPixelImage(tux, j + 1, i), 256, pix2);
          conv(GC_GetPixelImage(tux, j, i + 1), 256, pix3);
          conv(GC_GetPixelImage(tux, j + 1, i + 1), 256, pix4);
          pix[0] =
            (int)((1 - aa) * (1 - bb) * pix1[0] + aa * (1 - bb) * pix2[0] +
                  bb * (1 - aa) * pix3[0] + aa * bb * pix4[0]);
          pix[1] =
            (int)((1 - aa) * (1 - bb) * pix1[1] + aa * (1 - bb) * pix2[1] +
                  bb * (1 - aa) * pix3[1] + aa * bb * pix4[1]);
          pix[2] =
            (int)((1 - aa) * (1 - bb) * pix1[2] + aa * (1 - bb) * pix2[2] +
                  bb * (1 - aa) * pix3[2] + aa * bb * pix4[2]);
          SDL_SetRenderDrawColor(ren, pix[2], pix[1], pix[0], 255);
          SDL_RenderDrawPoint(ren, x1, y);
          // }
        }
      }
      x1++;
      z1 += dz;
    }
  }
}

/*                             SCANCONV                               */
/*                                                                    */
/*  Questa procedura fa lo scan conversion di un poligono.            */
/*  DATI IN INGRESSO                                                  */
/*    n     : numero dei vertici del poligono.                        */
/*    polx  : vettori contenenti le ascisse e le ordinate dei vertici */
/*    poly    del poligono.                                           */
/*                                                                    */
void scanconv(SDL_Renderer *ren, int n) {
  scandec = 1;
  loadpol(n);
  if (edges > 1) {
    scan = yymax[1] - 0.5;
    startedge = 1;
    endedge = 1;
    includ();
    while (endedge != startedge) {
      fillscan(ren);
      scan = scan - scandec;
      includ();
      updatevalues();
    }
  }
}

void draw_texture(SDL_Renderer *ren, int kf, int i1, int i2, int i3, int i4) {
  int x0, y0, x1, y1, x2, y2;
  float z0, z1, z2;

  /*ogni faccia quadrata viene divisa in due triangoli */
  for (int k = 0; k < 2; k++) {
    if (k == 0) {
      /*triangolo immagine con cui texturare il triangolo schermo */
      xim0 = tx[kf][3]; yim0 = ty[kf][3];
      xim1 = tx[kf][2]; yim1 = ty[kf][2];
      xim2 = tx[kf][0]; yim2 = ty[kf][0];

      /*triangolo schermo*/
      x0 = xs[i4]; y0 = ys[i4]; z0 = z_prosp_corr[i4];
      x1 = xs[i3]; y1 = ys[i3]; z1 = z_prosp_corr[i3];
      x2 = xs[i1]; y2 = ys[i1]; z2 = z_prosp_corr[i1];

      polz_e[0] = z_observer[i4];
      polz_e[1] = z_observer[i3];
      polz_e[2] = z_observer[i1];
      polz_e[3] = z_observer[i4];
    } else {
      /*triangolo immagine con cui texturare il triangolo schermo */
      xim0 = tx[kf][2]; yim0 = ty[kf][2];
      xim1 = tx[kf][1]; yim1 = ty[kf][1];
      xim2 = tx[kf][0]; yim2 = ty[kf][0];

      /*triangolo schermo*/
      x0 = xs[i3]; y0 = ys[i3]; z0 = z_prosp_corr[i3];
      x1 = xs[i2]; y1 = ys[i2]; z1 = z_prosp_corr[i2];
      x2 = xs[i1]; y2 = ys[i1]; z2 = z_prosp_corr[i1];

      polz_e[0] = z_observer[i3];
      polz_e[1] = z_observer[i2];
      polz_e[2] = z_observer[i1];
      polz_e[3] = z_observer[i3];
    }
    polx[0] = x0;
    polx[1] = x1;
    polx[2] = x2;
    polx[3] = x0;

    poly[0] = y0;
    poly[1] = y1;
    poly[2] = y2;
    poly[3] = y0;
    
    polz[0] = z0;
    polz[1] = z1;
    polz[2] = z2;
    polz[3] = z0;
    scanconv(ren, 3);
  }
}

void draw_mesh(SDL_Renderer *ren, SDL_Surface *tux) 
{
  int init;
  float xe, ye, ze; /* coord. dell'osservatore */
  init = 1;
  for (int k = 0; k < nvert; k++) {
    trasf_prosp_gen(k, &init, x[k] - csx, y[k] - csy, z[k] - csz, &xe, &ye,
                    &ze);
    /* proiezione e trasformazione in coordinate schermo */
    xs[k] = (int)(Sx * (xe - xwmin) + xvmin + 0.5);
    ys[k] = (int)(Sy * (ywmin - ye) + yvmax + 0.5);
    z_prosp_corr[k] = ze;
  }
  clear_zbuffer();
  SDL_SetRenderDrawColor(ren, 100, 0, 0, 255);
  for (int k = 0; k < nface; k++) {
    draw_texture(ren, k, face[k][0], face[k][1], face[k][2], face[k][3]);
  }
  SDL_RenderPresent(ren);
}

int main() {
  SDL_Window *win;
  SDL_Renderer *ren;
  SDL_Rect sub_v, v, bsub_v;
  TTF_Font *font;
  SDL_Event myevent;
  int res = 0;
  int choice, done = 0;

  /* load an image by file */
  // tux = GC_LoadImage("grid.ppm", &res);
  tux = GC_LoadImage("ganesha.ppm", &res);
  if (res == 0)
    return 1;

  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    fprintf(stderr, "Couldn't init video: %s\n", SDL_GetError());
    return (1);
  }

  /* Initialize the TTF library */
  if (TTF_Init() < 0) {
    fprintf(stderr, "Couldn't initialize TTF: %s\n", SDL_GetError());
    SDL_Quit();
    return (2);
  }

  font = TTF_OpenFont("FreeSans.ttf", DEFAULT_PTSIZE);
  if (font == NULL) {
    fprintf(stderr, "Couldn't load font\n");
  }

  v.x = 0;
  v.y = 0;
  v.w = 720;
  v.h = 600;
  xvmin = v.x + 5;
  yvmin = v.y + 5;
  xvmax = v.w - 130;
  yvmax = v.h - 10;
  sub_v.x = xvmin;
  sub_v.y = yvmin;
  sub_v.w = xvmax;
  sub_v.h = yvmax;
  bsub_v.x = sub_v.x - 2;
  bsub_v.y = sub_v.y - 2;
  bsub_v.w = sub_v.w + 4;
  bsub_v.h = sub_v.h + 4;
  wim = tux->w;
  him = tux->h;

  win =
    SDL_CreateWindow("View Cube Model", 0, 0, v.w, v.h, SDL_WINDOW_RESIZABLE);
  if (win == NULL) {
    fprintf(stderr, "SDL_CreateWindow Error: %s\n", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED |
                           SDL_RENDERER_PRESENTVSYNC);
  if (ren == NULL) {
    SDL_DestroyWindow(win);
    fprintf(stderr, "SDL_CreateRenderer Error: %s\n", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  /* clear z-buffer */
  clear_zbuffer();

  /* compute parameters */
  xwmin = -1;
  xwmax = 1;
  ywmin = -1;
  ywmax = 1;

  /* fattori di scala per trasf. Window-Viewport */
  Sx = ((float)(xvmax) - (float)(xvmin)) / (xwmax - xwmin);
  Sy = ((float)(yvmax) - (float)(yvmin)) / (ywmax - ywmin);

  // Set color
  SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
  // Clear back buffer
  SDL_RenderClear(ren);
  // Swap back and front buffer
  SDL_RenderPresent(ren);

  SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
  // GC_DrawRect(ren,sub_v.x-2,sub_v.y-2,sub_v.w+4,sub_v.h+4);
  SDL_RenderDrawRect(ren, &bsub_v);

  init_menu(ren, menu, teta, fi);
  draw_menu(menu, ren, font);
  define_object(); /* determinazione mesh oggetto */
  define_view();
  draw_mesh(ren, tux);

  // Disegnare tutte le volte, anche per eventi intermedi,
  // rallenta tutto senza dare un contributo all'immagine finale.
  // Gli eventi si possono scremare in questo modo:
  // viene usata la costante 0xff perchè nel codice sorgente delle SDL è usato
  // come costante all'interno delle funzioni che accettano interi 8 bit come
  // input
  // per rappresentare SDL_ALLEVENTS (che invece e' a 32 bit)

  while (done == 0) {
    if (SDL_PollEvent(&myevent)) {
      //  SDL_EventState(0xff, SDL_IGNORE); //0xff is all events
      SDL_EventState(SDL_MOUSEMOTION, SDL_IGNORE);
      switch (myevent.type) {
      case SDL_MOUSEMOTION:
        if (myevent.motion.state == 1) {
          opt_menu(menu, 11, myevent.motion.x, myevent.motion.y, &choice);
          switch (choice) {
          case 1: // choice==tetawin
            tmpx = myevent.motion.x - (menu[1].rect.x + menu[1].rect.w / 2);
            tmpy = myevent.motion.y - (menu[1].rect.y + menu[1].rect.h / 2);
            teta = atan2(tmpy, tmpx);
            SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
            SDL_RenderFillRect(ren, &(menu[1].rect));
            SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
            GC_DrawText(ren, font, 0, 0, 0, 0, 255, 255, 255, 0, menu[1].text,
                        menu[1].rect.x, menu[1].rect.y, shaded);
            // GC_DrawRect(ren,menu[1].rect.x,menu[1].rect.y,menu[1].rect.w,menu[1].rect.h);
            SDL_RenderDrawRect(ren, &(menu[1].rect));
            SDL_RenderDrawLine(ren, myevent.motion.x, myevent.motion.y,
                               menu[1].rect.x + menu[1].rect.w / 2,
                               menu[1].rect.y + menu[1].rect.h / 2);
            draw_menu(menu, ren, font);
            SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
            SDL_RenderFillRect(ren, &sub_v);
            draw_mesh(ren, tux);
            break;
          case 2: // choice==fiwin
            tmpx = myevent.motion.x - (menu[2].rect.x + menu[2].rect.w / 2);
            tmpy = myevent.motion.y - (menu[2].rect.y + menu[2].rect.h / 2);
            SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
            SDL_RenderFillRect(ren, &(menu[2].rect));
            SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
            GC_DrawText(ren, font, 0, 0, 0, 0, 255, 255, 255, 0, menu[2].text,
                        menu[2].rect.x, menu[2].rect.y, shaded);
            // GC_DrawRect(ren,menu[2].rect.x,menu[2].rect.y,menu[2].rect.w,menu[2].rect.h);
            SDL_RenderDrawRect(ren, &(menu[2].rect));
            SDL_RenderDrawLine(ren, myevent.motion.x, myevent.motion.y,
                               menu[2].rect.x + menu[2].rect.w / 2,
                               menu[2].rect.y + menu[2].rect.h / 2);
            draw_menu(menu, ren, font);
            SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
            SDL_RenderFillRect(ren, &sub_v);
            fi = atan2(tmpy, tmpx);
            draw_mesh(ren, tux);
            break;
          }
        }
        break;

      case SDL_MOUSEBUTTONDOWN:
        if (myevent.button.button == 1) {
          opt_menu(menu, 11, myevent.button.x, myevent.button.y, &choice);
          switch (choice) {
          case 0: // choice == finewin
            done = 1;
            break;
          case 7: // choice == rightwin
            SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
            SDL_RenderFillRect(ren, &sub_v);
            ssx = -sin(teta);
            ssy = cos(teta);
            csx -= sr * ssx;
            csy -= sr * ssy;
            draw_mesh(ren, tux);
            break;
          case 8: // choice == leftwin
            SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
            SDL_RenderFillRect(ren, &sub_v);
            ssx = -sin(teta);
            ssy = cos(teta);
            csx += sr * ssx;
            csy += sr * ssy;
            draw_mesh(ren, tux);
            break;
          case 9: // choice == upwin
            SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
            SDL_RenderFillRect(ren, &sub_v);
            ssx = -cos(fi) * cos(teta);
            ssy = -cos(fi) * sin(teta);
            ssz = sin(fi);
            csx += sr * ssx;
            csy += sr * ssy;
            csz += sr * ssz;
            draw_mesh(ren, tux);
            break;
          case 10: // choice == downwin
            SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
            SDL_RenderFillRect(ren, &sub_v);
            ssx = -cos(fi) * cos(teta);
            ssy = -cos(fi) * sin(teta);
            ssz = sin(fi);
            csx -= sr * ssx;
            csy -= sr * ssy;
            csz -= sr * ssz;
            draw_mesh(ren, tux);
            break;
          case 3: // choice == ziwin
            if (alpha - salpha > 0)
              alpha -= salpha;
            SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
            SDL_RenderFillRect(ren, &sub_v);
            draw_mesh(ren, tux);
            break;
          case 4: // choice == zowin
            if (alpha + salpha < 1.57)
              alpha += salpha;
            SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
            SDL_RenderFillRect(ren, &sub_v);
            draw_mesh(ren, tux);
            break;
          case 5: // choice == Dziwin
            if (D - Dstep > 0)
              D -= Dstep;
            SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
            SDL_RenderFillRect(ren, &sub_v);
            draw_mesh(ren, tux);
            break;
          case 6: // choice == Dzowin
            D += Dstep;
            SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
            SDL_RenderFillRect(ren, &sub_v);
            draw_mesh(ren, tux);
            break;
          }
        }
        break;
      case SDL_KEYDOWN:
        if (myevent.key.keysym.sym == SDLK_ESCAPE)
          done = 1;
        break;
      } /* fine switch */
      //   SDL_EventState(0xff, SDL_ENABLE);
      SDL_EventState(SDL_MOUSEMOTION, SDL_ENABLE);
    }
  } /* fine while */

  TTF_CloseFont(font);
  TTF_Quit();
  SDL_DestroyRenderer(ren);
  SDL_DestroyWindow(win);
  SDL_Quit();
  return (0);

} /* fine main */
