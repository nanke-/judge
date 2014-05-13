cc = g++
sources = main.cpp mysqldao.cpp
CFLAGS = -c -lm  -I/opt/lampp/include -L/opt/lampp/lib/ -lmysqlclient -lz
object = main.o mysqldao.o

test: $(object) 
	$(cc) $^  -I/opt/lampp/include -L/opt/lampp/lib/ -lmysqlclient -o $@
main.o: $(sources)
	$(cc) $(CFLAGS) $^
mysqldao.o: mysqldao.cpp
	$(cc) $(CFLAGS) $^

clean:
	rm -rf test *.o
