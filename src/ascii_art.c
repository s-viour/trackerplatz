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

Background* load_background(const char* filename) {
	int width = COLS - 22;
	int height = LINES - 8;
	char* raw = load_text_file(filename);
	char* cropped = malloc(width * height * sizeof(char) + 1);
	cropped[width * height * sizeof(char)] = '\0';


	int idx = 0;
	int ridx = 0;
	for (int j = 0; j < height; ++j) {
		for (int i = 0; i < width; ++i) {
			cropped[idx] = raw[ridx];
			++idx;
			++ridx;
		}
		ridx += 201 - width;
	}

	Background* bg = malloc(sizeof(Background));
	bg->cropped = cropped;
	free(raw);
	raw = NULL;

	return bg;
}

void free_background(Background* bg) {
	free(bg->cropped);
	bg->cropped = NULL;

	free(bg);
	bg = NULL;
}

void draw_background(Background* bg) {
	int idx = 0;
	
	for (int y = 4; y < LINES - 4; ++y) {
		for (int x = 11; x < COLS - 11; ++x) {
			mvaddch(y, x, bg->cropped[idx]);
			++idx;
		}
	}
}

void change_backgrounds_co() {
	ChangeBackgroundArgs* args = aco_get_arg();
	int tick = 0;
	int idx = 0;
	while (true) {
		if (tick != 1000) {
			++tick;
		} else {
			tick = 0;
			idx = rand() % args->count;
		}
		draw_background(args->list[idx]);
		aco_yield();
	}
}