# Project: progettoCar1

CPP  = g++
CC   = gcc
BIN  = progettoCar1

OBJ  = main.o
LINKOBJ  = main.o
LIBS = -L/usr/X11R6 -lGL -lGLU -lSDL2 -lm
INCS = -I. -I/usr/X11R6/include
CXXINCS=#
CXXFLAGS = $(CXXINCS)
CFLAGS = $(INCS)
RM = rm -f

all: $(BIN)


clean:
	${RM} $(OBJ) $(BIN)

$(BIN): $(OBJ)
	$(CPP) $(LINKOBJ) -o $(BIN) $(LIBS)

main.o: main.cpp
	$(CPP) -c main.cpp -o main.o $(CXXFLAGS)
