CC=g++
CFLAGS=-g
PTHR=-lpthread

ALL=*.o

all: server
server: game.cpp server.cpp saferw.cpp player.cpp
	$(CC) $(CFLAGS) -o $@ $^ $(PTHR)
#%.o: %.cpp
#	$(CC) $(CFLAGS) -c $^ $(PTHR)
clean:
	rm $(ALL)
