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
LDLIBGL = -L/usr/X11R6/lib -lX11 -lSDL2 -lSDL2_ttf -lSDL2_image -lGL -lGLU -lm

# ---- Files --- 
SRC0= persp_sdl.c $(GCGDIR)$(GCGLib).c $(GCGDIR)$(GCGLib).h
OBJ0= persp_sdl.o $(GCGDIR)$(GCGLib).o

SRC1= persp_cube_image_sdl.c $(GCGDIR)$(GCGLib).c $(GCGDIR)$(GCGLib).h
OBJ1= persp_cube_image_sdl.o $(GCGDIR)$(GCGLib).o

SRC2= persp_cube_sdl.c $(GCGDIR)$(GCGLib).c $(GCGDIR)$(GCGLib).h
OBJ2= persp_cube_sdl.o $(GCGDIR)$(GCGLib).o

SRC3= persp_cube_color_sdl.c $(GCGDIR)$(GCGLib).c $(GCGDIR)$(GCGLib).h
OBJ3= persp_cube_color_sdl.o $(GCGDIR)$(GCGLib).o

BIN= persp_sdl persp_cube_image_sdl persp_cube_sdl persp_cube_color_sdl

all:$(BIN) 

persp_sdl:$(OBJ0)
	$(CC) $(OBJ0) $(LDLIB) -o $@

persp_cube_image_sdl:$(OBJ1)
	$(CC) $(OBJ1) $(LDLIB) -o $@

persp_cube_sdl:$(OBJ2)
	$(CC) $(OBJ2) $(LDLIB) -o $@

persp_cube_color_sdl:$(OBJ3)
	$(CC) $(OBJ3) $(LDLIB) -o $@

$(GCGDIR)$(GCGLib).o: $(GCGDIR)$(GCGLib).c $(GCGDIR)$(GCGLib).h
	$(CC) $(CFLAGS) $(GCGDIR)$(GCGLib).c -o$@

persp_sdl.o:$(SRC0)
	$(CC) $(CDEB) $(CFLAGS) persp_sdl.c

persp_cube_image_sdl.o:$(SRC1)
	$(CC) $(CDEB) $(CFLAGS) persp_cube_image_sdl.c

persp_cube_sdl.o:$(SRC2)
	$(CC) $(CDEB) $(CFLAGS) persp_cube_sdl.c

persp_cube_color_sdl.o:$(SRC3)
	$(CC) $(CDEB) $(CFLAGS) persp_cube_color_sdl.c

.c.o:
	$(CC) $(CFLAGS) *.c

clean:
	rm -f *.o *.*~ *~ $(GCGLib).o $(BIN)
