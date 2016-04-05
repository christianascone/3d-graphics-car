# -- makefile
#    if you  encounter the message: "Missing separator..."
#    make sure that all indented lines are indented by using a TAB
#    rather than blanks

CC = gcc
CFLAGS = -c -Wall
CDEB= -DDEBUG
GCGDIR=../GCGraLib2/
GCGLib=GCGraLib2

# Library linking 
LDLIB = -L/usr/X11R6/lib -lX11 -lSDL2 -lSDL2_ttf -lSDL2_image -lm

# ---- Files --- 
SRC14= persp_cube_sdl.c $(GCGDIR)$(GCGLib).c $(GCGDIR)$(GCGLib).h
OBJ14= persp_cube_sdl.o $(GCGDIR)$(GCGLib).o

BIN= persp_cube_sdl

all:$(BIN) 

persp_cube_sdl:$(OBJ14)
	$(CC) $(OBJ14) $(LDLIB) -o $@

$(GCGDIR)$(GCGLib).o: $(GCGDIR)$(GCGLib).c $(GCGDIR)$(GCGLib).h
	$(CC) $(CFLAGS) $(GCGDIR)$(GCGLib).c -o $@

persp_cube_sdl.o:$(SRC14)
	$(CC) $(CDEB) $(CFLAGS) persp_cube_sdl.c

.c.o:
	$(CC) $(CFLAGS) *.c

clean:
	rm -f *.o *.*~ *~ $(GCGDIR)*.o $(BIN)
