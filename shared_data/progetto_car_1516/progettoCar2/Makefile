# Project: progettoCar2

CPP  = g++
CC   = gcc
BIN  = progettoCar2

OBJ  = main.o car.o
LINKOBJ  = main.o car.o
LIBS = -L/usr/X11R6 -lGL -lGLU -lSDL2 -lm
INCS = -I. -I/usr/X11R6/include
CXXINCS=#
CXXFLAGS = $(CXXINCS)  
CFLAGS = $(INCS)  
CDEB= #-DDEBUG
RM = rm -f

all: $(BIN)


clean:
	${RM} $(OBJ) $(BIN)

$(BIN): $(OBJ)
	$(CPP) $(LINKOBJ) -o $(BIN) $(LIBS)

main.o: main.cpp
	$(CPP) $(CDEB) -c main.cpp -o main.o $(CXXFLAGS)

car.o: car.cpp
	$(CPP) $(CDEB) -c car.cpp -o car.o $(CXXFLAGS)
