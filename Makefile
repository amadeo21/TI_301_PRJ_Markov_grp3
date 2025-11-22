CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -O2

all: part1 part2 part3 markov

part1: main.o adjlist.o
	$(CC) $(CFLAGS) -o part1 main.o adjlist.o

part2: main_part2.o adjlist.o tarjan.o hasse.o
	$(CC) $(CFLAGS) -o part2 main_part2.o adjlist.o tarjan.o hasse.o

part3: main_part3.o adjlist.o tarjan.o hasse.o matrix.o
	$(CC) $(CFLAGS) -o part3 main_part3.o adjlist.o tarjan.o hasse.o matrix.o

markov: main_global.o
	$(CC) $(CFLAGS) -o markov main_global.o

main.o: main.c adjlist.h
	$(CC) $(CFLAGS) -c main.c

main_part2.o: main_part2.c adjlist.h tarjan.h hasse.h
	$(CC) $(CFLAGS) -c main_part2.c

main_part3.o: main_part3.c adjlist.h tarjan.h hasse.h matrix.h
	$(CC) $(CFLAGS) -c main_part3.c

main_global.o: main_global.c
	$(CC) $(CFLAGS) -c main_global.c

adjlist.o: adjlist.c adjlist.h
	$(CC) $(CFLAGS) -c adjlist.c

tarjan.o: tarjan.c tarjan.h adjlist.h
	$(CC) $(CFLAGS) -c tarjan.c

hasse.o: hasse.c hasse.h tarjan.h adjlist.h
	$(CC) $(CFLAGS) -c hasse.c

matrix.o: matrix.c matrix.h
	$(CC) $(CFLAGS) -c matrix.c

clean:
	rm -f *.o part1 part2 part3 markov
