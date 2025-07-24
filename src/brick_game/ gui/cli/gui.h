#ifndef GUI_H
#define GUI_H

#include <ncurses.h>
#include "tetris.h"

#define FIELD_START_Y 2
#define FIELD_START_X 2
#define INFO_PANEL_X (FIELD_START_X + BOARD_WIDTH * 2 + 4)
#define NEXT_PIECE_Y (FIELD_START_Y + 8)
#define NEXT_PIECE_X (INFO_PANEL_X + 2)

// Color pairs
#define COLOR_FIELD 1
#define COLOR_BORDER 2
#define COLOR_TEXT 3
#define COLOR_PIECE 4

// Function prototypes
void init_gui(void);
void cleanup_gui(void);
void draw_game(const GameInfo_t *info);
void draw_field(const GameInfo_t *info);
void draw_border(void);
void draw_info_panel(const GameInfo_t *info);
void draw_next_piece(const GameInfo_t *info);
void draw_game_over(void);
void draw_pause(void);
UserAction_t get_user_input(void);
void show_instructions(void);

#endif