CC = clang
CFLAGS = -lncurses -g -Wall
SOURCES = trackerplatz.c util.c ascii_art.c

all: trackerplatz

trackerplatz: $(SOURCES) acosw.S aco.c
	$(CC) $(CFLAGS) aco.c acosw.S $(SOURCES) -o trackerplatz
