#ifndef FX_H
#define FX_H

typedef struct TickerArgs {
	int ypos;
	char* text;
} TickerArgs;

typedef struct FallingAsciiArgs {
	int xpos_start;
	int xpos_end;
} FallingAsciiArgs;

void ticker();

void falling_ascii();

#endif