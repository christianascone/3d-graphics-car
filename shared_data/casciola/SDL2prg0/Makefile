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
SRC9= provasdl.c $(GCGDIR)$(GCGLib).c $(GCGDIR)$(GCGLib).h
OBJ9= provasdl.o $(GCGDIR)$(GCGLib).o

SRC10= provatext.c $(GCGDIR)$(GCGLib).c $(GCGDIR)$(GCGLib).h
OBJ10= provatext.o $(GCGDIR)$(GCGLib).o

SRC11= provasdl_GC.c $(GCGDIR)$(GCGLib).c $(GCGDIR)$(GCGLib).h
OBJ11= provasdl_GC.o $(GCGDIR)$(GCGLib).o

SRC12= provasdl_GCsurf1.c $(GCGDIR)$(GCGLib).c $(GCGDIR)$(GCGLib).h
OBJ12= provasdl_GCsurf1.o $(GCGDIR)$(GCGLib).o

SRC13= provasdl_GCsurf2.c $(GCGDIR)$(GCGLib).c $(GCGDIR)$(GCGLib).h
OBJ13= provasdl_GCsurf2.o $(GCGDIR)$(GCGLib).o

SRC0= draw_data.c $(GCGDIR)$(GCGLib).c $(GCGDIR)$(GCGLib).h
OBJ0= draw_data.o $(GCGDIR)$(GCGLib).o

SRC2= polygon2.c $(GCGDIR)$(GCGLib).c $(GCGDIR)$(GCGLib).h
OBJ2= polygon2.o $(GCGDIR)$(GCGLib).o

SRC3= polygonf2ren.c $(GCGDIR)$(GCGLib).c $(GCGDIR)$(GCGLib).h
OBJ3= polygonf2ren.o $(GCGDIR)$(GCGLib).o

SRC4= inter_polygon2ren.c $(GCGDIR)$(GCGLib).c $(GCGDIR)$(GCGLib).h
OBJ4= inter_polygon2ren.o $(GCGDIR)$(GCGLib).o

SRC5= mouse_polygon2ren.c $(GCGDIR)$(GCGLib).c $(GCGDIR)$(GCGLib).h
OBJ5= mouse_polygon2ren.o $(GCGDIR)$(GCGLib).o

SRC6= ball2.c $(GCGDIR)$(GCGLib).c $(GCGDIR)$(GCGLib).h
OBJ6= ball2.o $(GCGDIR)$(GCGLib).o

SRC7= ball_time.c $(GCGDIR)$(GCGLib).c $(GCGDIR)$(GCGLib).h
OBJ7= ball_time.o $(GCGDIR)$(GCGLib).o

SRC8= orologio.c $(GCGDIR)$(GCGLib).c $(GCGDIR)$(GCGLib).h
OBJ8= orologio.o $(GCGDIR)$(GCGLib).o

SRC14= ball.c $(GCGDIR)$(GCGLib).c $(GCGDIR)$(GCGLib).h
OBJ14= ball.o $(GCGDIR)$(GCGLib).o

BIN= provasdl provatext provasdl_GC provasdl_GCsurf1 provasdl_GCsurf2 draw_data polygon2 polygonf2ren inter_polygon2ren mouse_polygon2ren ball2 ball_time orologio ball

all:$(BIN) 

provasdl:$(OBJ9)
	$(CC) $(OBJ9) $(LDLIB) -o $@

provatext:$(OBJ10)
	$(CC) $(OBJ10) $(LDLIB) -o $@

provasdl_GC:$(OBJ11)
	$(CC) $(OBJ11) $(LDLIB) -o $@

provasdl_GCsurf1:$(OBJ12)
	$(CC) $(OBJ12) $(LDLIB) -o $@

provasdl_GCsurf2:$(OBJ13)
	$(CC) $(OBJ13) $(LDLIB) -o $@

draw_data:$(OBJ0)
	$(CC) $(OBJ0) $(LDLIB) -o $@

polygon2:$(OBJ2)
	$(CC) $(OBJ2) $(LDLIB) -o $@

polygonf2ren:$(OBJ3)
	$(CC) $(OBJ3) $(LDLIB) -o $@

inter_polygon2ren:$(OBJ4)
	$(CC) $(OBJ4) $(LDLIB) -o $@

mouse_polygon2ren:$(OBJ5)
	$(CC) $(OBJ5) $(LDLIB) -o $@

ball2:$(OBJ6)
	$(CC) $(OBJ6) $(LDLIB) -o $@

ball_time:$(OBJ7)
	$(CC) $(OBJ7) $(LDLIB) -o $@

orologio:$(OBJ8)
	$(CC) $(OBJ8) $(LDLIB) -o $@

ball:$(OBJ14)
	$(CC) $(OBJ14) $(LDLIB) -o $@

$(GCGDIR)$(GCGLib).o: $(GCGDIR)$(GCGLib).c $(GCGDIR)$(GCGLib).h
	$(CC) $(CFLAGS) $(GCGDIR)$(GCGLib).c -o $@

provasdl.o:$(SRC9)
	$(CC) $(CDEB) $(CFLAGS) provasdl.c

provatext.o:$(SRC10)
	$(CC) $(CDEB) $(CFLAGS) provatext.c

provasdl_GC.o:$(SRC11)
	$(CC) $(CDEB) $(CFLAGS) provasdl_GC.c

provasdl_GCsurf1.o:$(SRC12)
	$(CC) $(CDEB) $(CFLAGS) provasdl_GCsurf1.c

provasdl_GCsurf2.o:$(SRC13)
	$(CC) $(CDEB) $(CFLAGS) provasdl_GCsurf2.c

draw_data.o:$(SRC0)
	$(CC) $(CDEB) $(CFLAGS) draw_data.c

polygon2.o:$(SRC2)
	$(CC) $(CDEB) $(CFLAGS) polygon2.c

polygonf2ren.o:$(SRC3)
	$(CC) $(CDEB) $(CFLAGS) polygonf2ren.c

inter_polygon2ren.o:$(SRC4)
	$(CC) $(CDEB) $(CFLAGS) inter_polygon2ren.c

mouse_polygon2ren.o:$(SRC5)
	$(CC) $(CDEB) $(CFLAGS) mouse_polygon2ren.c

ball2.o:$(SRC6)
	$(CC) $(CDEB) $(CFLAGS) ball2.c

ball_time.o:$(SRC7)
	$(CC) $(CDEB) $(CFLAGS) ball_time.c

orologio.o:$(SRC8)
	$(CC) $(CDEB) $(CFLAGS) orologio.c

ball.o:$(SRC14)
	$(CC) $(CDEB) $(CFLAGS) ball.c

.c.o:
	$(CC) $(CFLAGS) *.c

clean:
	rm -f *.o *.*~ *~ $(GCGDIR)*.o $(BIN)
