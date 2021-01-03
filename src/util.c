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

// retrived from https://stackoverflow.com/questions/9210528/split-string-with-delimiters-in-c
// copying code, i know ;p
// returns a pointer to a Lines structure
// that contains a char** of all the lines in a string
Lines* split_lines(char* a_str)
{
	char a_delim = '\n';
    char** result    = 0;
    size_t count     = 0;
    char* tmp        = a_str;
    char* last_comma = 0;
    char delim[2];
    delim[0] = a_delim;
    delim[1] = 0;

    /* Count how many elements will be extracted. */
    while (*tmp)
    {
        if (a_delim == *tmp)
        {
            count++;
            last_comma = tmp;
        }
        tmp++;
    }

    /* Add space for trailing token. */
    count += last_comma < (a_str + strlen(a_str) - 1);

    /* Add space for terminating null string so caller
       knows where the list of returned strings ends. */
    count++;

    result = malloc(sizeof(char*) * count);

    if (result)
    {
        size_t idx  = 0;
        char* token = strtok(a_str, delim);

        while (token)
        {
            *(result + idx++) = strdup(token);
            token = strtok(0, delim);
        }
        *(result + idx) = 0;
    }

    Lines* s_lines = malloc(sizeof(Lines));
    s_lines->lines = result;
    s_lines->count = count;
    return s_lines;
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
