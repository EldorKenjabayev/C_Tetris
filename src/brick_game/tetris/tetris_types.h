#ifndef TETRIS_TYPES_H
#define TETRIS_TYPES_H

#include <stdbool.h>

// Game constants
#define BOARD_WIDTH 10
#define BOARD_HEIGHT 20
#define BOARD_EXTRA_HEIGHT 4
#define TOTAL_HEIGHT (BOARD_HEIGHT + BOARD_EXTRA_HEIGHT)
#define PIECE_SIZE 4
#define PIECE_COUNT 7

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

// Piece types
typedef enum {
    PIECE_I, PIECE_O, PIECE_T, PIECE_S, PIECE_Z, PIECE_J, PIECE_L
} PieceType_t;

// Game states for FSM
typedef enum {
    STATE_START,
    STATE_SPAWN,
    STATE_MOVING,
    STATE_SHIFTING,
    STATE_ATTACHING,
    STATE_GAME_OVER,
    STATE_PAUSE
} GameState_t;

// Piece structure
typedef struct {
    PieceType_t type;
    int x, y;
    int rotation;
    int shape[PIECE_SIZE][PIECE_SIZE];
} Piece_t;

// Main game structure
typedef struct {
    GameState_t state;
    int board[TOTAL_HEIGHT][BOARD_WIDTH];
    Piece_t current_piece;
    Piece_t next_piece;
    int score;
    int high_score;
    int level;
    int speed;
    int lines_cleared;
    bool paused;
    bool game_over;
    int timer;
    int drop_timer;
} TetrisGame_t;

#endif  // TETRIS_TYPES_H
