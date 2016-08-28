CC=cc

CFLAGS=-Wall -g -lrt

OUTPUT="bin/$(OUT)"
DIRECT=$(D)
FILE=$(FILE)

SOURCES=$(wildcard $(DIRECT)/$(FILE).c ../lib/*.c)
HEADERS=$(wildcard $(DIRECT)/$(FILE).h ../lib/*.h)


target:
	$(CC) $(CFLAGS) $(SOURCES) timers/itimerspec_from_str.c -o $(OUTPUT)
