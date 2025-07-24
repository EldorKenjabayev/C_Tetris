#ifndef TETRIS_H
#define TETRIS_H

#include <stdbool.h>

#define BOARD_WIDTH 10
#define BOARD_HEIGHT 20
#define BOARD_EXTRA_HEIGHT 4
#define TOTAL_HEIGHT (BOARD_HEIGHT + BOARD_EXTRA_HEIGHT)
#define PIECE_SIZE 4

// User actions enum as specified in requirements
typedef enum {
    Start,
    Pause,
    Terminate,
    Left,
    Right,
    Up,
    Down,
    Action
} UserAction_t;

// Game info structure as specified in requirements
typedef struct {
    int **field;
    int **next;
    int score;
    int high_score;
    int level;
    int speed;
    int pause;
} GameInfo_t;

// Main API functions as specified in requirements
void userInput(UserAction_t action, bool hold);
GameInfo_t updateCurrentState(void);

// Additional helper functions
void init_game(void);
void cleanup_game(void);
void save_high_score(int score);
int load_high_score(void);
void free_field_memory(int **field, int height);

#endif  // TETRIS_H