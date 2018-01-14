CC=cc

CFLAGS=-Wall -g -pthread

OUTPUT="bin/$(OUT)"
DIRECT=$(D)
FILE=$(FILE)

SOURCES=$(wildcard $(DIRECT)/$(FILE).c ../lib/*.c ../lib/ds/*.c)
HEADERS=$(wildcard $(DIRECT)/$(FILE).h ../lib/*.h ../lib/ds/*.h)


target:
	$(CC) $(CFLAGS) $(SOURCES) -o $(OUTPUT)
