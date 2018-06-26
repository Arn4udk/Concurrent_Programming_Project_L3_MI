CC=gcc
CFLAGS=-std=gnu99 -Wall -g
LDFLAGS=-lrt -pthread

all : passenger lift

shared.o : shared.c shared.h
	$(CC) $(CFLAGS) -c $<

passenger.o: passenger.c shared.h
	$(CC) $(CFLAGS) -c $< -o $@

lift.o: lift.c shared.h
	$(CC) $(CFLAGS) -c $< -o $@

passenger: passenger.o shared.o
	$(CC) $(CFLAGS) passenger.o shared.o -o $@ $(LDFLAGS)

lift: lift.o shared.o
	$(CC) $(CFLAGS) lift.o shared.o -o $@ $(LDFLAGS)
	
clean:
	rm -rf lift passenger *.o *.dSYM
