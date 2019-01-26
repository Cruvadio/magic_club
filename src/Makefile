CC=g++
CFLAGS=-g
PTHR=-lpthread

ALL=*.o

all: server
server: game.o server.o saferw.o
	$(CC) $(CFLAGS) -o $@ $^ $(PTHR)
%.o: %.cpp
	$(CC) $(CFLAGS) -c $^ $(PTHR)
clean:
	rm $(ALL)
