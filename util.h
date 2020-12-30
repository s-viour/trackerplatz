#ifndef UTIL_H
#define UTIL_H

typedef struct Point {
	int x;
	int y;
} Point;

typedef struct Rect {
	Point start;
	int width;
	int height;
} Rect;

typedef struct Lines {
	char** lines;
	int count;
} Lines;

void msleep(unsigned);

char* generate_string(int, char);

char* load_text_file(const char*);

Lines* split_lines(char*);

void clear_rect(const Rect*);

void clear_main_screen();

#endif