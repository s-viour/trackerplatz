CC = clang
CFLAGS = -lncurses -lm -g -Wall -I./ -I./src
SOURCES = src/trackerplatz.c src/util.c src/ascii_art.c src/fx.c

all: trackerplatz trackerplatz_debug

trackerplatz: $(SOURCES) acosw.S aco.c
	$(CC) $(CFLAGS) aco.c acosw.S $(SOURCES) -o trackerplatz

# lol am i doing this right
# is this even common practice to make a debug executable that
# plays nice with valgrind?
trackerplatz_debug: $(SOURCES) acosw.S aco.c
	$(CC) $(CFLAGS) -D ACO_USE_VALGRIND aco.c acosw.S $(SOURCES) -o trackerplatz_debug
