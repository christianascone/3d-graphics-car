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

# ---- Files --- 
SRC0= raster_draw_color.c $(GCGDIR)$(GCGLib).c $(GCGDIR)$(GCGLib).h
OBJ0= raster_draw_color.o $(GCGDIR)$(GCGLib).o

SRC1= raster_draw_image.c $(GCGDIR)$(GCGLib).c $(GCGDIR)$(GCGLib).h
OBJ1= raster_draw_image.o $(GCGDIR)$(GCGLib).o

SRC2= rgb2gray.c $(GCGDIR)$(GCGLib).c $(GCGDIR)$(GCGLib).h
OBJ2= rgb2gray.o $(GCGDIR)$(GCGLib).o

SRC3= line_clip.c $(GCGDIR)$(GCGLib).c $(GCGDIR)$(GCGLib).h
OBJ3= line_clip.o $(GCGDIR)$(GCGLib).o

SRC4= scan_conv.c $(GCGDIR)$(GCGLib).c $(GCGDIR)$(GCGLib).h
OBJ4= scan_conv.o $(GCGDIR)$(GCGLib).o

BIN= raster_draw_color raster_draw_image rgb2gray line_clip scan_conv

all:$(BIN) 

raster_draw_color:$(OBJ0)
	$(CC) $(OBJ0) $(LDLIB) -o $@

raster_draw_image:$(OBJ1)
	$(CC) $(OBJ1) $(LDLIB) -o $@

rgb2gray:$(OBJ2)
	$(CC) $(OBJ2) $(LDLIB) -o $@

line_clip:$(OBJ3)
	$(CC) $(OBJ3) $(LDLIB) -o $@

scan_conv:$(OBJ4)
	$(CC) $(OBJ4) $(LDLIB) -o $@

$(GCGDIR)$(GCGLib).o: $(GCGDIR)$(GCGLib).c $(GCGDIR)$(GCGLib).h
	$(CC) $(CFLAGS) $(GCGDIR)$(GCGLib).c -o $@

raster_draw_color.o:$(SRC0)
	$(CC) $(CDEB) $(CFLAGS) raster_draw_color.c

raster_draw_image.o:$(SRC1)
	$(CC) $(CDEB) $(CFLAGS) raster_draw_image.c

rgb2gray.o:$(SRC2)
	$(CC) $(CDEB) $(CFLAGS) rgb2gray.c

line_clip.o:$(SRC3)
	$(CC) $(CDEB) $(CFLAGS) line_clip.c

scan_conv.o:$(SRC4)
	$(CC) $(CDEB) $(CFLAGS) scan_conv.c

.c.o:
	$(CC) $(CFLAGS) *.c

clean:
	rm -f *.o *.*~ *~ $(GCGDIR)*.o $(BIN)
