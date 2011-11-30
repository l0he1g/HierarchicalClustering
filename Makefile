#makefile for hierarchical clustering
CC=g++
CFLAGS=-c -pipe -O2
SRC=$(wildcard *.cpp)
OBJ=$(SRC:.cpp=.o)
EXE=test

$(EXE):$(OBJ)
	$(CC) $(OBJ) -o $@

test.o:Controller.h
Cluster.o:Cluster.h string_util.h Word.h
Controller.o:Controller.h Cluster.h Config.h DimArray.h
DimArray.o:DimArray.h Word.h

%.o:%.cpp
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf $(OBJ) $(EXE)

