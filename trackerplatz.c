#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>

#include "aco.h"
#include "ascii_art.h"
#include "util.h"

void draw_frame(const Rect*);

void draw_bars(int);

void ticker();

void falling_ascii();

void trackerplatz_init();

typedef struct TickerArgs {
	int ypos;
	char* text;
} TickerArgs;

typedef struct FallingAsciiArgs {
	int xpos_start;
	int xpos_end;
} FallingAsciiArgs;

int main(int argc, char* argv[]) {
	// load all the necessary files into strings
	printf("TRACKERPLATZ INIT\n");
	char* ticker_text = load_text_file("resources/ticker.txt");
	AsciiArt* art1 = load_ascii_art("resources/65warez_proudly_presents.txt");
	AsciiArt* art2 = load_ascii_art("resources/tracker_platz.txt");
	AsciiArt* art3 = load_ascii_art("resources/65daysofstatic.txt");

	
	initscr();
	start_color();
	init_pair(1, COLOR_WHITE, COLOR_BLACK);
	curs_set(0);

	aco_thread_init(NULL);
	aco_t* main_co = aco_create(NULL, NULL, 0, NULL, NULL);
	aco_share_stack_t* sstk = aco_share_stack_new(0);

	

	trackerplatz_init();
	msleep(500);
	draw_ascii_art(art1);
	msleep(2000);
	
	clear_main_screen();
	msleep(100);
	draw_ascii_art(art2);
	msleep(2000);
	clear_main_screen();
	msleep(100);
	draw_ascii_art(art3);
	

	
	TickerArgs ticker1_args = {1, ticker_text};
	TickerArgs ticker2_args = {LINES - 2, ticker_text};
	FallingAsciiArgs falling_ascii_args1 = {1, 7};
	FallingAsciiArgs falling_ascii_args2 = {COLS - 8, COLS - 2};


	aco_t* ticker1 = aco_create(main_co, sstk, 0, ticker, &ticker1_args);
	aco_t* ticker2 = aco_create(main_co, sstk, 0, ticker, &ticker2_args);
	aco_t* falling_ascii1 = aco_create(main_co, sstk, 0, falling_ascii, &falling_ascii_args1);
	aco_t* falling_ascii2 = aco_create(main_co, sstk, 0, falling_ascii, &falling_ascii_args1);
	aco_t* falling_ascii3 = aco_create(main_co, sstk, 0, falling_ascii, &falling_ascii_args2);
	aco_t* falling_ascii4 = aco_create(main_co, sstk, 0, falling_ascii, &falling_ascii_args2);

	while (true) {
		aco_resume(ticker1);
		aco_resume(ticker2);
		aco_resume(falling_ascii1);
		aco_resume(falling_ascii2);
		aco_resume(falling_ascii3);
		aco_resume(falling_ascii4);
		refresh();
		msleep(6);
	}
	
	

	endwin();

	aco_destroy(ticker1);
	ticker1 = NULL;
	aco_destroy(ticker2);
	ticker2 = NULL;

    aco_share_stack_destroy(sstk);
    sstk = NULL;

    aco_destroy(main_co);
    main_co = NULL;

	free_ascii_art(art3);
	art3 = NULL;
	free_ascii_art(art2);
	art2 = NULL;
	free_ascii_art(art1);
	art1 = NULL;
	free(ticker_text);
	ticker_text = NULL;
	return 0;
}

void trackerplatz_init() {
	// draw the bars on the top and bottom
	// this happens immediately on the video, so no sleeps
	draw_bars(1);
	draw_bars(LINES - 2);

	Point frame1_p = {0, 3};
	Point frame2_p = {10, 3};
	Point frame3_p = {COLS - 9, 3};
	Rect frame1_r = {frame1_p, 9, LINES - 3};
	Rect frame2_r = {frame2_p, COLS - 20, LINES - 3};
	Rect frame3_r = {frame3_p, 9, LINES - 3};


	// draw all three frames
	// first left, then center, then right
	msleep(500);
	draw_frame(&frame1_r);
	msleep(500);
	draw_frame(&frame2_r);
	msleep(500);
	draw_frame(&frame3_r);
	msleep(500);
}

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

void draw_bars(int y) {
	char* line = generate_string(COLS, '-');
	mvprintw(y - 1, 0, line);
	mvprintw(y + 1, 0, line);
	free(line);
	refresh();
}

void ticker() {
	TickerArgs* arg = aco_get_arg();
	int ypos = arg->ypos;
	char* text = arg->text;

	long long state = 0;
	int textlen = strlen(text);

	while (true) {
		int start = state % textlen;

		char linebuffer[COLS + 1];
		strncpy(linebuffer, &text[start], COLS);


		mvprintw(ypos, 0, linebuffer);
		++state;
		aco_yield();
	}
}

void falling_ascii() {
	FallingAsciiArgs* args = aco_get_arg();
	int xpos_start = args->xpos_start;
	int xpos_end = args->xpos_end;
	float ypos_star;
	int xpos_star;
	float speed;

	while (true) {
		ypos_star = 6;
		xpos_star = rand() % (xpos_start - xpos_end + 1) + xpos_start;

		speed = (float)rand()/(float)(RAND_MAX/0.5) + 0.1;
		Point clear_start = {xpos_star, 4};
		Rect clear = {clear_start, 1, LINES - 8};

		while (ypos_star < LINES - 5) {
			clear_rect(&clear);

			mvprintw(ypos_star, xpos_star, "*");
			mvprintw(ypos_star - 1, xpos_star, "+");
			mvprintw(ypos_star - 2, xpos_star, "@");

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