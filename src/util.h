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

void draw_frame(const Rect*);

void draw_bars(int);

void fill_main_screen(char c);

void draw_line(Point*, Point*, char);

int randint(int, int);

#endif