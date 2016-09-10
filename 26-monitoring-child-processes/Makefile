CC=cc

CFLAGS=-Wall -g

OUTPUT="bin/$(OUT)"
DIRECT=$(D)
FILE=$(FILE)

SOURCES=$(wildcard ../lib/*.c $(DIRECT)/$(FILE).c)
HEADERS=$(wildcard ../lib/*.h $(DIRECT)/$(FILE).h)


target:
	$(CC) $(CFLAGS) $(SOURCES) -o $(OUTPUT)
