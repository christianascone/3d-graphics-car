# -- makefile
#    if you  encounter the message: "Missing separator..."
#    make sure that all indented lines are indented by using a TAB
#    rather than blanks

CC = gcc
CFLAGS = -c -Wall
CDEB= #-DDEBUG
GCGDIR=../GCGraLib2/
GCGLib=GCGraLib2

# Library linking 
LDLIB = -L/usr/X11R6/lib -lX11 -lSDL2 -lSDL2_ttf -lSDL2_image -lm
LDLIBGL = -lSDL2 -lSDL2_ttf -lSDL2_image -lGL -lGLU -lm
LDLIBGL2 = -lSDL2 -lGL -lGLU -lm
#LDLIBGL = -L/usr/X11R6/lib -lX11 -lGL -lGLU -lGLEW -lSDL2 -lSDL2_ttf -lSDL2_image -lm

# ---- Files --- 
SRC00= persp_sdl_gl.c $(GCGDIR)$(GCGLib).c $(GCGDIR)$(GCGLib).h
OBJ00= persp_sdl_gl.o $(GCGDIR)$(GCGLib).o

SRC01= persp_cube_sdl_gl.c $(GCGDIR)$(GCGLib).c $(GCGDIR)$(GCGLib).h
OBJ01= persp_cube_sdl_gl.o $(GCGDIR)$(GCGLib).o

SRC02= persp_cube_sdl_gl3D.c $(GCGDIR)$(GCGLib).c $(GCGDIR)$(GCGLib).h
OBJ02= persp_cube_sdl_gl3D.o $(GCGDIR)$(GCGLib).o

SRC03= bezier3d.c
OBJ03= bezier3d.o

BIN= persp_sdl_gl persp_cube_sdl_gl persp_cube_sdl_gl3D bezier3d

all:$(BIN) 

persp_sdl_gl:$(OBJ00)
	$(CC) $(OBJ00) $(LDLIBGL) -o $@

persp_cube_sdl_gl:$(OBJ01)
	$(CC) $(OBJ01) $(LDLIBGL) -o $@

persp_cube_sdl_gl3D:$(OBJ02)
	$(CC) $(OBJ02) $(LDLIBGL) -o $@

bezier3d:$(OBJ03)
	$(CC) $(OBJ03) $(LDLIBGL2) -o $@

$(GCGDIR)$(GCGLib).o: $(GCGDIR)$(GCGLib).c $(GCGDIR)$(GCGLib).h
	$(CC) $(CFLAGS) $(GCGDIR)$(GCGLib).c -o$@

persp_sdl_gl.o:$(SRC00)
	$(CC) $(CDEB) $(CFLAGS) persp_sdl_gl.c

persp_cube_sdl_gl.o:$(SRC01)
	$(CC) $(CDEB) $(CFLAGS) persp_cube_sdl_gl.c

persp_cube_sdl_gl3D.o:$(SRC02)
	$(CC) $(CDEB) $(CFLAGS) persp_cube_sdl_gl3D.c

bezier3d.o:$(SRC03)
	$(CC) $(CDEB) $(CFLAGS) bezier3d.c

.c.o:
	$(CC) $(CFLAGS) *.c

clean:
	rm -f *.o *.*~ *~ $(BIN)
