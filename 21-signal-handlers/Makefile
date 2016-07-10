CC=cc

CFLAGS=-Wall -g

OUTPUT="bin/$(OUT)"
DIRECT=$(D)
FILE=$(FILE)
FLAGS=

SOURCES=$(wildcard $(DIRECT)/$(FILE).c ../lib/*.c)
HEADERS=$(wildcard $(DIRECT)/$(FILE).h ../lib/*.h)


target:
	$(CC) $(CFLAGS) $(SOURCES) -o $(OUTPUT) $(FLAGS)
