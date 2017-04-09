CC=cc

CFLAGS=-Wall -g -pthread -lcrypt

OUTPUT="bin/$(OUT)"
DIRECT=$(D)
FILE=$(FILE)

SOURCES=$(wildcard $(DIRECT)/$(FILE).c ../lib/*.c)
HEADERS=$(wildcard $(DIRECT)/$(FILE).h ../lib/*.h)


target:
	$(CC) $(CFLAGS) $(SOURCES) -o $(OUTPUT)
