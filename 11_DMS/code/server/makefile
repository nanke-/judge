CC = g++
RM = rm -rf
CFLAGS = -c -Wall -I. -D_DEBUG
server: main.o filedao.o oracledao.o logbuffer.o logthread.o \
	storethread.o clientthread.o serversocket.o server.o
	$(CC) $^ -lpthread -o $@
main.o: main.cpp
	$(CC) $(CFLAGS) $^
filedao.o: filedao.cpp
	$(CC) $(CFLAGS) $^
oracledao.o: oracledao.cpp
	$(CC) $(CFLAGS) $^
logbuffer.o: logbuffer.cpp
	$(CC) $(CFLAGS) $^
storethread.o: storethread.cpp
	$(CC) $(CFLAGS) $^
clientthread.o: clientthread.cpp
	$(CC) $(CFLAGS) $^
serversocket.o: serversocket.cpp
	$(CC) $(CFLAGS) $^
server.o: server.cpp
	$(CC) $(CFLAGS) $^

clean:
	$(RM) server *.o *.gch
