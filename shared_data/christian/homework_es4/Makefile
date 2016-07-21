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
SRC1= persp_zbuffer.c $(GCGDIR)$(GCGLib).c $(GCGDIR)$(GCGLib).h
OBJ1= persp_zbuffer.o $(GCGDIR)$(GCGLib).o

SRC2= scan_conv.c $(GCGDIR)$(GCGLib).c $(GCGDIR)$(GCGLib).h
OBJ2= scan_conv.o $(GCGDIR)$(GCGLib).o

BIN= persp_zbuffer scan_conv

all:$(BIN) 


persp_zbuffer:$(OBJ1)
	$(CC) $(OBJ1) $(LDLIB) -o $@

scan_conv:$(OBJ2)
	$(CC) $(OBJ2) $(LDLIB) -o $@

$(GCGDIR)$(GCGLib).o: $(GCGDIR)$(GCGLib).c $(GCGDIR)$(GCGLib).h
	$(CC) $(CFLAGS) $(GCGDIR)$(GCGLib).c -o$@


persp_zbuffer.o:$(SRC1)
	$(CC) $(CDEB) $(CFLAGS) persp_zbuffer.c

scan_conv.o:$(SRC1)
	$(CC) $(CDEB) $(CFLAGS) scan_conv.c

.c.o:
	$(CC) $(CFLAGS) *.c

clean:
	rm -f *.o *.*~ *~ $(GCGLib).o $(BIN)
