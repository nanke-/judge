CC = g++
EXE = judge
SOURCES = compiler.cpp main.cpp checker.cpp executor.cpp logger.cpp
CFLAGS = -c -I/usr/include/libxml2 -lxml2
object = main.o compiler.o checker.o executor.o logger.o

judge: $(object)
	$(CC) $^ -lxml2 -o $@
main.o: $(SOURCES)
	$(CC) $(CFLAGS) $^
executor.o: executor.cpp
	$(CC) $(CFLAGS) $^
compiler.o: compiler.cpp
	$(CC) $(CFLAGS) $^
checker.o: checker.cpp
	$(CC) $(CFLAGS) $^
logger.o: logger.cpp
clean:
	rm -rf *.o *.gch judge 1001 stdout
