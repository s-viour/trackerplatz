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


AsciiArt* load_ascii_art(const char*);

void free_ascii_art(AsciiArt*);

void draw_ascii_art(AsciiArt*);

void draw_ascii_art_co();

Background* load_background(const char*);

void draw_background(Background*);

void free_background(Background*);

#endif