CC = clang
CFLAGS = -O2 -lncurses -lm -g -Wall -I./ -I./src
SOURCES = src/trackerplatz.c src/util.c src/ascii_art.c src/fx.c

all: trackerplatz

trackerplatz: $(SOURCES) acosw.S aco.c
	$(CC) $(CFLAGS) aco.c acosw.S $(SOURCES) -o trackerplatz
