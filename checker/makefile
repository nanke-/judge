CC=g++
EXE=checker
SOURCES=checker.h checker.cpp main.cpp
CFLAGS=-lm -I/usr/include/libxml2 -lxml2 -g

checker: main.cpp
	$(CC) $(SOURCES) $(CFLAGS) -o $(EXE)

clean:
	checker