#include "util.h"

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>

void msleep(unsigned msec) {
	usleep(msec * 1000);
}

// generates a string of length size by repeating
// the supplied character
char* generate_string(int size, const char repeat) {
	char* str = (char*)malloc(size * sizeof(char) + 1);
	for (int i = 0; i < size; ++i) {
		str[i] = repeat;
	}
	str[size] = '\0';

	return str;
}

// opens a text file and returns a pointer
// to the text content
// if this fails, it'll just exit the program lol
char* load_text_file(const char* filename) {
	FILE* fp = NULL;

	fp = fopen(filename, "r");
	if (fp == NULL) {
		perror("error occurred in load_text_file: ");
		exit(1);
	}

	fseek(fp, 0, SEEK_END);
	size_t fsize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	char* str = (char*)malloc(fsize + 1);
	fread(str, 1, fsize, fp);
	fclose(fp);
	str[fsize] = '\0';

	return str;
}

// splits a string into lines and
// returns a Lines struct containing pointers to each line
Lines* split_lines(char* a_str)
{
	char* tmp = a_str;
    int count = 1;
    while (*tmp) {
        if (*tmp == '\n') {
            count += 1;
        }
        ++tmp;
    }

    char** lines = malloc(count * sizeof(char*));
    int i = 0;
    char* tok_str = strtok(a_str, "\n");
    while (tok_str != NULL) {
        lines[i] = tok_str;
        ++i;
        tok_str = strtok(NULL, "\n");
    }

    Lines* l = malloc(sizeof(Lines));
    l->lines = lines;
    l->count = count;
    return l;
}

// clears a space on the screen by printing whitespace
void clear_rect(const Rect* r) {
    int i, j;

    for (i = r->start.x; i < r->start.x + r->width; ++i) {
        for (j = r->start.y; j < r->start.y + r->height; ++j) {
            mvaddch(j, i, ' ');
        }
    }
    refresh();
}

void clear_main_screen() {
    Point main_point = {11, 4};
    Rect main_rect = {main_point, COLS - 22, LINES - 8};
    clear_rect(&main_rect);
}

// function responsible for drawing the boxes made of asterisks
void draw_frame(const Rect* r) {
    char* top_frame = generate_string(r->width, '*');
    char* mid_frame = (char*)malloc(r->width * sizeof(char) + 1);
    int i;

    for (i = 1; i < r->width - 1; ++i) {
        mid_frame[i] = ' ';
    }
    mid_frame[0] = '*';
    mid_frame[r->width - 1] = '*';
    mid_frame[r->width] = '\0';

    mvprintw(r->start.y, r->start.x, top_frame);
    for (i = r->start.y + 1; i < r->height - 1; ++i) {
        mvprintw(i, r->start.x, mid_frame);
    }
    mvprintw(i, r->start.x, top_frame);

    free(top_frame);
    free(mid_frame);
    refresh();
}

// function responsible for drawing the bars
// made of dash-characters at the top and bottom of the screen
void draw_bars(int y) {
    char* line = generate_string(COLS, '-');
    mvprintw(y - 1, 0, line);
    mvprintw(y + 1, 0, line);
    free(line);
    refresh();
}
