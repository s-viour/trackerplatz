#ifndef ASCII_ART_H
#define ASCII_ART_H

#include "util.h"

typedef struct AsciiArt {
	char* raw;
	Lines* lines;
} AsciiArt;

typedef struct Background {
	char* cropped;
} Background;

typedef struct ChangeBackgroundArgs {
	Background** list;
	int count;
} ChangeBackgroundArgs;


AsciiArt* load_ascii_art(const char*);

void free_ascii_art(AsciiArt*);

void draw_ascii_art(AsciiArt*);

void draw_ascii_art_co();

Background* load_background(const char*);

void draw_background(Background*);

void free_background(Background*);

void change_backgrounds_co();

#endif