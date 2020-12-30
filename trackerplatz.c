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

void trackerplatz_init();

typedef struct TickerArgs {
	int ypos;
	char* text;
} TickerArgs;


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
	aco_t* ticker1 = aco_create(main_co, sstk, 0, ticker, &ticker1_args);
	aco_t* ticker2 = aco_create(main_co, sstk, 0, ticker, &ticker2_args);

	while (true) {
		aco_resume(ticker1);
		aco_resume(ticker2);
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
		refresh();
		msleep(7);
		++state;
		aco_yield();
	}
}