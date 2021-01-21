#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <ncurses.h>

#include "aco.h"
#include "ascii_art.h"
#include "fx.h"
#include "util.h"

#define NUM_BGS 5
#define NUM_ROUTINES 8
#define NUM_ART_ROUTINES 3

static int QUIT = 0;

void trackerplatz_init();

void ascii_art_simul(aco_t*, aco_t*, aco_t*);


int main(int argc, char* argv[]) {
	// initialize the screen, set the bg color to white on black,
	// hide the cursor, and set the getch() timeout to 10ms
	initscr();
	start_color();
	init_pair(1, COLOR_WHITE, COLOR_BLACK);
	init_pair(2, COLOR_RED, COLOR_BLACK);
	curs_set(0);
	timeout(10);

	// load all the necessary files into strings
	char* ticker_text = load_text_file("resources/ticker.txt");
	AsciiArt* art1 = load_ascii_art("resources/65warez_proudly_presents.txt");
	AsciiArt* art2 = load_ascii_art("resources/tracker_platz.txt");
	AsciiArt* art3 = load_ascii_art("resources/65daysofstatic.txt");
	Background* bgs[] = {
		load_background("resources/bg_joe.txt"),
		load_background("resources/bg_si.txt"),
		load_background("resources/bg_abstract.txt"),
		load_background("resources/bg_corner.txt"),
		load_background("resources/bg_t.txt")
	};

	// initialize aco, and create the main coroutine and the shared stack
	aco_thread_init(NULL);
	aco_t* main_co = aco_create(NULL, NULL, 0, NULL, NULL);
	aco_share_stack_t* sstk = aco_share_stack_new(0);
	
	// create the arguments to be supplied to the coroutines
	// since aco only lets us pass one argument, we bundle
	// them all into structures
	TickerArgs ticker1_args = {1, ticker_text};
	TickerArgs ticker2_args = {LINES - 2, ticker_text};
	FallingAsciiArgs falling_ascii_args1 = {1, 7, 1};
	FallingAsciiArgs falling_ascii_args2 = {COLS - 8, COLS - 2, 2};
	OrbitArgs orbit_args1 = {COLS / 2, LINES / 2, M_PI, 15, ' '};
	ChangeBackgroundArgs change_background_args = {bgs, 5};
	
	// array of the big ascii art coroutines
	// we store these separately from the main routines because
	// they don't get executed for the entire duration
	aco_t* ascii_art[] = {
		aco_create(main_co, sstk, 0, draw_ascii_art_co, art1),
		aco_create(main_co, sstk, 0, draw_ascii_art_co, art2),
		aco_create(main_co, sstk, 0, draw_ascii_art_co, art3)
	};


	// array of the main routines that run throughout the entire program
	// we store them in the array so we can iterate over them
	aco_t* routines[] = {
		aco_create(main_co, sstk, 0, ticker, &ticker1_args),
		aco_create(main_co, sstk, 0, ticker, &ticker2_args),
		aco_create(main_co, sstk, 0, falling_ascii, &falling_ascii_args1),
		aco_create(main_co, sstk, 0, falling_ascii, &falling_ascii_args1),
		aco_create(main_co, sstk, 0, falling_ascii, &falling_ascii_args2),
		aco_create(main_co, sstk, 0, falling_ascii, &falling_ascii_args2),
		aco_create(main_co, sstk, 0, change_backgrounds_co, &change_background_args),
		aco_create(main_co, sstk, 0, orbit, &orbit_args1),
	};

	// draw the bars and frames
	trackerplatz_init();
	
	
	// ascii_art_simul is responsible for drwaing the ascii art
	// while maintaining the scrolling text across the top and bottom
	for (int i = 0; i < NUM_ART_ROUTINES && !QUIT; ++i) {
		ascii_art_simul(ascii_art[i], routines[0], routines[1]);
		clear_main_screen();
	}
	

	// main loop of the program
	// as of right now, this just runs forever until quit
	draw_background(bgs[0]);
	while (!QUIT) {
		// for every coroutine in the main routines array
		// resume it
		for (int i = 0; i < NUM_ROUTINES; ++i) {
			aco_resume(routines[i]);
		}
		// after running each routine once, refresh and
		// sleep for 10ms while waiting to see if we can quit
		refresh();
		if (getch() == 'q') {
			QUIT = 1;
		}
	}
	

	// kill the ncurses window
	endwin();

	// deconstruct all the ascii art routines
	for (int i = 0; i < NUM_ART_ROUTINES; ++i) {
		aco_destroy(ascii_art[i]);
		ascii_art[i] = NULL;
	}

	// deconstruct all the main routines
	for (int i = 0; i < NUM_ROUTINES; ++i) {
		aco_destroy(routines[i]);
		routines[i] = NULL;
	}

	// and free everything else
    aco_share_stack_destroy(sstk);
    sstk = NULL;

    aco_destroy(main_co);
    main_co = NULL;

    for (int i = 0; i < NUM_BGS; ++i) {
    	free_background(bgs[i]);
    	bgs[i] = NULL;
    }
	free_ascii_art(art3);
	free_ascii_art(art2);
	free_ascii_art(art1);
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


// responsible for drawing the ascii art while also making
// sure the tickers keep running at a regular pace
void ascii_art_simul(aco_t* ascii_art, aco_t* ticker1, aco_t* ticker2) {
	while (!ascii_art->is_end && !QUIT) {
		// this is done by resuming the tickers 10 times
		// for every one resumption of the ascii art
		for (int i = 0; i < 10; ++i) {
			aco_resume(ticker1);
			aco_resume(ticker2);
			refresh();
			
			if (getch() == 'q') {
				QUIT = 1;
				return;
			}
		}
		aco_resume(ascii_art);
		refresh();
	}

	// after finishing drawing the art
	// tick for a little while longer to keep the art on the screen
	for (int i = 0; i < 300; ++i) {
		aco_resume(ticker1);
		aco_resume(ticker2);
		refresh();
		if (getch() == 'q') {
			QUIT = 1;
			return;
		}
	}
}