#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include "aco.h"
#include "util.h"

#include "fx.h"

// responsible for drawing the scrolling text at
// the top and bottom of the screen
// i'm not really sure why i called it ticker
void ticker() {
	TickerArgs* arg = aco_get_arg();
	int ypos = arg->ypos;
	char* text = arg->text;
	int textlen = strlen(text);
	int start = 0;
	int end = 0;
	int idx = 0;
	long long state = 0;

	while (true) {
		start = state % textlen;
		end = start + COLS;

		idx = 0;
		for (int i = start; i < end; ++i) {
			mvaddch(ypos, idx, text[i % textlen]);
			++idx;
		}

		
		++state;
		aco_yield();
	}
}

// function that handles drawing the falling asterisks and slashes
// on the left and right frames
// TODO: add color to this, so it can be red like in the video
void falling_ascii() {
	FallingAsciiArgs* args = aco_get_arg();
	int xpos_start = args->xpos_start;
	int xpos_end = args->xpos_end;
	float ypos_star;
	int xpos_star;
	float speed;

	// outer loop, runs forever, clears the screen before looping again
	while (true) {
		// start at a y of 6 so we can draw something 3 characters tall
		ypos_star = 6;
		// generate a random xpos within the frame
		xpos_star = rand() % (xpos_start - xpos_end + 1) + xpos_start;

		// generate a random float for the speed
		// the ypos will be equal to the floor(initial_ypos + speed)
		speed = (float)rand()/(float)(RAND_MAX/0.5) + 0.1;
		Point clear_start = {xpos_star, 4};
		Rect clear = {clear_start, 1, LINES - 8};

		// inner loop, makes the ascii fall until reaching the bottom, then exits
		while (ypos_star < LINES - 5) {
			clear_rect(&clear);

			mvprintw(ypos_star, xpos_star, "*");
			mvprintw(ypos_star - 1, xpos_star, "+");
			mvprintw(ypos_star - 2, xpos_star, "@");

			// this loop draws the slashes above the main falling object
			for (int i = ypos_star; i > ypos_star / 2; --i) {
				if (rand() % 5 == 0) {
					mvprintw(i, xpos_star, "/");
				}
			}

			ypos_star += speed;
			aco_yield();
		}
		clear_rect(&clear);
	}
}