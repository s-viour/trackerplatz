#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include <math.h>
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
	int color = args->color;
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

			attron(COLOR_PAIR(color));
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
			attroff(COLOR_PAIR(color));
			aco_yield();
		}
		clear_rect(&clear);
	}
}

void draw_circle(char fill, Point* pos) {
	// 2x2 square
	mvaddch(pos->y, pos->x, fill);
	mvaddch(pos->y, pos->x + 1, fill);
	mvaddch(pos->y + 1, pos->x, fill);
	mvaddch(pos->y + 1, pos->x + 1, fill);
	mvaddch(pos->y + 2, pos->x, fill);
	mvaddch(pos->y + 2, pos->x + 1, fill);
	mvaddch(pos->y + 2, pos->x - 1, fill);
	mvaddch(pos->y + 2, pos->x + 2, fill);


	// right
	mvaddch(pos->y, pos->x + 2, fill);
	mvaddch(pos->y + 1, pos->x + 2, fill);
	
	// left
	mvaddch(pos->y, pos->x - 1, fill);
	mvaddch(pos->y + 1, pos->x - 1, fill);	
}

void orbit() {
	extern Point ORB_LOCATIONS[6];

	OrbitArgs* arg = aco_get_arg();
	double x_center = arg->x_center;
	double y_center = arg->y_center;

	double x1 = x_center;
	double y1 = y_center;
	double x2 = x_center;
	double y2 = y_center;

	int r = arg->radius;
	char fill = arg->fill;
	double t = arg->t_init;
	double rads = 2 * M_PI;
	Point p1 = {x1, y1};
	Point p2 = {x2, y2};

	while (true) {
		draw_line(&p1, &p2, ' ');
		draw_circle(' ', &p1);
		draw_circle(' ', &p2);
		x1 = r * cos(t) + x_center;
		y1 = r * sin(t) + y_center;
		x2 = -r * cos(t) + x_center;
		y2 = -r * sin(t) + y_center;
		p1.x = x1;
		p1.y = y1;
		p2.x = x2;
		p2.y = y2;
		draw_line(&p1, &p2, ' ');
		draw_circle(fill, &p1);
		draw_circle(fill, &p2);

		int pick1 = randint(0, 5);
		int pick2 = randint(0, 5);
		//draw_line(&p1, &ORB_LOCATIONS[pick1], ' ');
		//draw_line(&p2, &ORB_LOCATIONS[pick2], ' ');

		if (t >= rads) {
			t = 0;
		}
		t += 0.03;
		aco_yield();
	}
}