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

typedef struct OrbitArgs {
	int x_center;
	int y_center;
	float t_init;
	int radius;
	char fill;
} OrbitArgs;

void ticker();

void falling_ascii();

void draw_circle(char, Point*);

void orbit();

#endif