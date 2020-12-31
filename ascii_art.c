#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include "ascii_art.h"
#include "util.h"

AsciiArt* load_ascii_art(const char* filename) {
	char* raw = load_text_file(filename);
	Lines* lines = split_lines(raw);

	AsciiArt* art = malloc(sizeof(AsciiArt));
	art->raw = raw;
	art->lines = lines;

	return art;
}

void free_ascii_art(AsciiArt* art) {
	free(art->raw);
	art->raw = NULL;
	for (int i = 0; i < art->lines->count; ++i) {
		free(art->lines->lines[i]);
		art->lines->lines[i] = NULL;
	}
	free(art->lines);
	art->lines = NULL;
	free(art);
	art = NULL;
}


void draw_ascii_art(AsciiArt* art) {
	for (int i = 0; i < art->lines->count - 1; ++i) {
		mvprintw(10 + i, 20, art->lines->lines[i]);
		msleep(100);
		refresh();
	}
}
