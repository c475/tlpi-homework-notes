CC=cc

CFLAGS=-Wall -g -static

OUTPUT="bin/$(OUT)"
DIRECT=$(D)
FILE=$(FILE)

SOURCES=$(wildcard $(DIRECT)/$(FILE).c)
HEADERS=$(wildcard $(DIRECT)/$(FILE).h)


target:
	$(CC) $(CFLAGS) $(SOURCES) -o $(OUTPUT)
