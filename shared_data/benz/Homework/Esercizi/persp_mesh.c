#include <stdio.h>
#include <math.h>
#include "../GCGraLib2/GCGraLib2.h"

#define MAXVERT (100)
#define MAXEDGE (200)
#define DEFAULT_PTSIZE  12

/* Dati per visualizzare un oggetto definito solo da vertici e lati*/
float   x[MAXVERT],y[MAXVERT],z[MAXVERT];       /* coordinate vertici */
int     edge[MAXEDGE][2];                       /* lista lati */
int     nvert,nedge;                            /* numero vertici e lati */
float   csx,csy,csz;                            /* centro oggetto */
float   D = 25,teta = 20,fi = 20,di;
int     xs[MAXVERT],ys[MAXVERT];                /* coordinate schermo */

int         xvmin,xvmax,yvmin,yvmax;            /* coordinate viewport */
float   salpha=0.05;                            /* zoomin e zoomout */
float   sr=0.25,ssx,ssy,ssz,ssmod,              /* modifica centro oggetto */
Dstep = 2.5,                                    /* passo di incremento D */
alpha,                                          /* apertura angolare piramide di vista */
s;
float     xwmin,xwmax,ywmin,ywmax;              /* coordinate window */
float     Sx, Sy;                               /* fattori di scala trasf. window-viewport */


float     tmpx,tmpy;                            /* variabili temporanee */
float     tmpx1,tmpy1;                          /* nuove variabili temporanee */
int       i, j;

typedef struct RECT RECT;
struct RECT{
    SDL_Rect rect;
    char     text[10];
};
RECT  menu[15];

int   done=0;


/* inizializzazione menu */
void init_menu(SDL_Renderer *ren, RECT menu[], float teta, float fi)
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

		menu[11].rect.x = xvmin;
    menu[11].rect.y = yvmin;
    menu[11].rect.w = xvmax;
    menu[11].rect.h = yvmax;
    strcpy(menu[11].text, "RENDER");
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

/* funzione che definisce l'oggetto mesh */
void define_object()
{

    /* Determino le coordinate dei vertici dell'oggetto */
    nvert=8;
    x[0]=-1.0; y[0]=-1.0; z[0]=-1.0;
    x[1]=1.0;  y[1]=-1.0; z[1]=-1.0;
    x[2]=1.0;  y[2]=1.0;  z[2]=-1.0;
    x[3]=-1.0; y[3]=1.0;  z[3]=-1.0;
    x[4]=-1.0; y[4]=-1.0; z[4]=1.0;
    x[5]=1.0;  y[5]=-1.0; z[5]=1.0;
    x[6]=1.0;  y[6]=1.0;  z[6]=1.0;
    x[7]=-1.0; y[7]=1.0;  z[7]=1.0;

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

    copy_trasl(8,&nvert,x,y,z,13,&nedge,edge,2.0,0.0,0.0);
    copy_trasl(8,&nvert,x,y,z,13,&nedge,edge,0.0,2.0,0.0);
    copy_trasl(8,&nvert,x,y,z,13,&nedge,edge,0.0,0.0,2.0);
}

/* trasformazione prospettica a tre punti di fuga (generico punto di vista) */
void trasf_prosp_gen(int *init,float x, float y, float z,
float *xe, float *ye, float *ze)
{
    static float steta,cteta,cfi,sfi;
    float xcom, ycom, zcom;

    if (*init)
    {
        *init=0;
        steta=sin(teta);
        cteta=cos(teta);
        sfi=sin(fi);
        cfi=cos(fi);
    }

    /* trasformazione in coordinate del sistema osservatore */
    // va modificata qui

    xcom = -steta*x -cteta*cfi*y -z*cteta*sfi;
    ycom = x*cteta -y*steta*cfi - z *steta*sfi;
    zcom = y*sfi - z*cfi + D;

    *xe=xcom;
    *ye=ycom;
    *ze=zcom;

    //*xe = -steta*x + y*cteta;
    //*ye = -cteta*cfi*x - y*steta*cfi + z*sfi;
    //*ze = -x*cteta*sfi - y *steta*sfi - z*cfi + D;


}

/* funzione che determina i fattori di scala */
void define_view(){
    int     k;
    float   r;
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

    /* raggio sfera contenente oggetto */
    r = sqrt(powf(xmax-csx,2)+powf(ymax-csy,2)+powf(zmax-csz,2));

    /* calcolo semilato window iniziale; il piano di proiezione viene assunto
    ad una distanza D dall'osservatore */
    s = r*D/sqrt(powf(D,2)-powf(r,2));

    /* in base al semilato window cosi' calcolato si determina l'apertura
    angolare iniziale */
    alpha=atan(s/D);
    /* printf("Apertura angolare: %f n",alpha);*/

}

void draw_mesh(SDL_Renderer *ren)
{
    int t,u,k,init;
    float xe,ye,ze;    /* coord. dell'osservatore */

    /* si ricalcola il semilato della window in base a D e ad alpha */
    s=D*tan(alpha);
    xwmin = -s ;
    xwmax = s;
    ywmin = -s;
    ywmax = s;

    /* fattori di scala per trasf. Window-Viewport */
    Sx = ((float)(xvmax) - (float)(xvmin))/(xwmax - xwmin);
    Sy = ((float)(yvmax) -(float)(yvmin))/(ywmax - ywmin);

    /* il piano di proiezione viene definito a passare per l'origine */
    di=D;
    init=1;
    for (k=0;k<nvert;k++)
    {
        trasf_prosp_gen(&init,x[k]-csx,y[k]-csy,z[k]-csz,&xe,&ye,&ze);
        //gluLookAt(&init,x[k]-csx,y[k]-csy,z[k]-csz,&xe,&ye,&ze);
        /* proiezione e trasformazione in coordinate schermo */
        xs[k] = (int)(Sx * ((di * xe)/ze - xwmin) + xvmin + 0.5);
        ys[k] = (int)(Sy * (ywmin - (di * ye)/ze) + yvmax + 0.5);
    }

    /* disegno mesh */
    for (k = 0;k<nedge;k++)
    {
        t=edge[k][0];
        u=edge[k][1];
        SDL_RenderDrawLine(ren, xs[t],ys[t],xs[u],ys[u]);
    }
    //printf("n [D,teta,fi]= %f %f %f",D,teta,fi);

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

    if (SDL_Init (SDL_INIT_VIDEO) < 0){
        fprintf (stderr, "Couldn't init video: %sn", SDL_GetError ());
        return (1);
    }

    /* Initialize the TTF library */
    if (TTF_Init () < 0){
        fprintf (stderr, "Couldn't initialize TTF: %sn", SDL_GetError ());
        SDL_Quit ();
        return (2);
    }

    font = TTF_OpenFont ("FreeSans.ttf", DEFAULT_PTSIZE);
    if (font == NULL)
    {
        fprintf (stderr, "Couldn't load fontn");
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

    win = SDL_CreateWindow("View Cube Model", 0, 0, v.w, v.h, SDL_WINDOW_RESIZABLE);
    if(win==NULL){
        fprintf(stderr,"SDL_CreateWindow Error: %sn",SDL_GetError());
        SDL_Quit();
        return 1;
    }

    ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (ren == NULL){
        SDL_DestroyWindow(win);
        fprintf(stderr,"SDL_CreateRenderer Error: %sn",SDL_GetError());
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
    define_view();  /*calcolo dei parametri di vista */
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
            switch(myevent.type){
                case SDL_MOUSEMOTION:
                if(myevent.motion.state==1){
										// it sets the variable choice
                    opt_menu(menu,11,myevent.motion.x,myevent.motion.y,&choice);
                    switch(choice){
                        case 1: //choice==tetawin
                        tmpx = myevent.motion.x-(menu[1].rect.x+menu[1].rect.w/2);
                        tmpy = myevent.motion.y-(menu[1].rect.y+menu[1].rect.h/2);
                        teta = atan2(tmpy,tmpx);
                        SDL_SetRenderDrawColor(ren,255,255,255,255);
                        SDL_RenderFillRect(ren,&(menu[1].rect));
                        SDL_SetRenderDrawColor(ren,0,0,0,255);
                        GC_DrawText(ren, font, 0, 0, 0, 0, 255, 255, 255, 0, menu[1].text,
                        menu[1].rect.x, menu[1].rect.y, shaded);

												SDL_RenderDrawRect(ren,&(menu[1].rect));
                        SDL_RenderDrawLine(ren, myevent.motion.x,myevent.motion.y,
                        menu[1].rect.x+menu[1].rect.w/2,menu[1].rect.y+menu[1].rect.h/2);

                        SDL_SetRenderDrawColor(ren,255,255,255,255);
                        SDL_RenderFillRect(ren,&sub_v);
                        SDL_SetRenderDrawColor(ren,0,0,0,255);
                        draw_mesh(ren);

                        break;
                        case 2: //choice==fiwin
                        tmpx = myevent.motion.x-(menu[2].rect.x+menu[2].rect.w/2);
                        tmpy = myevent.motion.y-(menu[2].rect.y+menu[2].rect.h/2);

                        SDL_SetRenderDrawColor(ren,255,255,255,255);
                        SDL_RenderFillRect(ren,&(menu[2].rect));
                        SDL_SetRenderDrawColor(ren,0,0,0,255);
                        GC_DrawText(ren, font, 0, 0, 0, 0, 255, 255, 255, 0, menu[2].text,menu[2].rect.x, menu[2].rect.y, shaded);

												SDL_RenderDrawRect(ren,&(menu[2].rect));
                        SDL_RenderDrawLine (ren, myevent.motion.x,myevent.motion.y, menu[2].rect.x+menu[2].rect.w/2,menu[2].rect.y+menu[2].rect.h/2);

                        SDL_SetRenderDrawColor(ren,255,255,255,255);
                        SDL_RenderFillRect(ren,&sub_v);
                        SDL_SetRenderDrawColor(ren,0,0,0,255);
                        fi = atan2(tmpy,tmpx);
                        draw_mesh(ren);
                        break;
												case 11:

                        tmpx = myevent.motion.x-(menu[11].rect.x+menu[11].rect.w/2);
                        tmpy = myevent.motion.y-(menu[11].rect.y+menu[11].rect.h/2);

                        teta = atan2(1,tmpx);
                        fi = atan2(tmpy,1);
                        SDL_SetRenderDrawColor(ren,255,255,255,255);
                        SDL_RenderFillRect(ren,&(menu[1].rect));
                        SDL_SetRenderDrawColor(ren,0,0,0,255);
                        GC_DrawText(ren, font, 0, 0, 0, 0, 255, 255, 255, 0, menu[1].text,
                        menu[1].rect.x, menu[1].rect.y, shaded);

												SDL_RenderDrawRect(ren,&(menu[1].rect));
                        SDL_RenderDrawLine(ren, myevent.motion.x,myevent.motion.y,
                        menu[1].rect.x+menu[1].rect.w/2,menu[1].rect.y+menu[1].rect.h/2);

                        SDL_SetRenderDrawColor(ren,255,255,255,255);
                        SDL_RenderFillRect(ren,&sub_v);
                        SDL_SetRenderDrawColor(ren,0,0,0,255);
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
                        SDL_SetRenderDrawColor(ren,0,0,0,255);
                        draw_mesh(ren);
                        break;
                        case 8: //choice == leftwin
                        SDL_SetRenderDrawColor(ren,255,255,255,255);
                        SDL_RenderFillRect(ren,&sub_v);
                        ssx=-sin(teta);
                        ssy=cos(teta);
                        csx+=sr*ssx;
                        csy+=sr*ssy;
                        SDL_SetRenderDrawColor(ren,0,0,0,255);
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
                        SDL_SetRenderDrawColor(ren,0,0,0,255);
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
                        SDL_SetRenderDrawColor(ren,0,0,0,255);
                        draw_mesh(ren);
                        break;
                        case 3: //choice == ziwin
                        if (alpha-salpha >0)
                        alpha-=salpha;
                        SDL_SetRenderDrawColor(ren,255,255,255,255);
                        SDL_RenderFillRect(ren,&sub_v);
                        SDL_SetRenderDrawColor(ren,0,0,0,255);
                        draw_mesh(ren);
                        break;
                        case 4: //choice == zowin
                        if (alpha+salpha <1.57)
                        alpha+=salpha;
                        SDL_SetRenderDrawColor(ren,255,255,255,255);
                        SDL_RenderFillRect(ren,&sub_v);
                        SDL_SetRenderDrawColor(ren,0,0,0,255);
                        draw_mesh(ren);
                        break;
                        case 5: //choice == Dziwin
                        if (D-Dstep>0)
                        D-=Dstep;
                        SDL_SetRenderDrawColor(ren,255,255,255,255);
                        SDL_RenderFillRect(ren,&sub_v);
                        SDL_SetRenderDrawColor(ren,0,0,0,255);
                        draw_mesh(ren);
                        break;
                        case 6: //choice == Dzowin
                        D+=Dstep;
                        SDL_SetRenderDrawColor(ren,255,255,255,255);
                        SDL_RenderFillRect(ren,&sub_v);
                        SDL_SetRenderDrawColor(ren,0,0,0,255);
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
