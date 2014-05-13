CC = g++
RM = rm -rf
CFLAGS = -c -Wall -I. -D_DEBUG
client: main.o client.o logreader.o \
	consolesender.o socketsender.o
	$(CC) $^ -o $@
main.o: main.cpp
	$(CC) $(CFLAGS) $^
client.o: client.cpp
	$(CC) $(CFLAGS) $^
logreader.o: logreader.cpp
	$(CC) $(CFLAGS) $^
consolesender.o: consolesender.cpp
	$(CC) $(CFLAGS) $^
socketsender.o: socketsender.cpp
	$(CC) $(CFLAGS) $^
clean:
	$(RM) client *.o
