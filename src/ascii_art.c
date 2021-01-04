#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include "ascii_art.h"
#include "util.h"
#include "aco.h"

// an AsciiArt structure consists of a Lines struct
// and a char* pointing to the raw data
// this function loads and constructs one from a file
AsciiArt* load_ascii_art(const char* filename) {
	char* raw = load_text_file(filename);
	Lines* lines = split_lines(raw);

	AsciiArt* art = malloc(sizeof(AsciiArt));
	art->raw = raw;
	art->lines = lines;

	return art;
}

// properly frees the lines of an AsciiArt
void free_ascii_art(AsciiArt* art) {
	free(art->lines->lines);
	art->lines->lines = NULL;

	free(art->lines);
	art->lines = NULL;

	free(art->raw);
	art->raw = NULL;

	free(art);
	art = NULL;
}

// this is unused as of now
void draw_ascii_art(AsciiArt* art) {
	for (int i = 0; i < art->lines->count - 1; ++i) {
		mvprintw(10 + i, 20, art->lines->lines[i]);
		msleep(100);
		refresh();
	}
} 

// coroutine version of drawing ascii art
void draw_ascii_art_co() {
	AsciiArt* art = aco_get_arg();
	for (int i = 0; i < art->lines->count - 1; ++i) {
		mvprintw(10 + i, 20, art->lines->lines[i]);
		aco_yield();
	}
	aco_exit();
}
