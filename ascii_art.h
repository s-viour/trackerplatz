#ifndef ASCII_ART_H
#define ASCII_ART_H

#include "util.h"

typedef struct AsciiArt {
	char* raw;
	Lines* lines;
} AsciiArt;


AsciiArt* load_ascii_art(const char*);

void free_ascii_art(AsciiArt*);

void draw_ascii_art(AsciiArt*);

#endif