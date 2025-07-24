#include "tetris_pieces.h"
#include <stdlib.h>
#include <time.h>
#include <string.h>

// Tetris piece templates [piece_type][rotation][y][x]
const int piece_templates[PIECE_COUNT][4][PIECE_SIZE][PIECE_SIZE] = {
    // I piece
    {
        {{0,0,0,0}, {1,1,1,1}, {0,0,0,0}, {0,0,0,0}},
        {{0,0,1,0}, {0,0,1,0}, {0,0,1,0}, {0,0,1,0}},
        {{0,0,0,0}, {0,0,0,0}, {1,1,1,1}, {0,0,0,0}},
        {{0,1,0,0}, {0,1,0,0}, {0,1,0,0}, {0,1,0,0}}
    },
    // O piece
    {
        {{0,0,0,0}, {0,1,1,0}, {0,1,1,0}, {0,0,0,0}},
        {{0,0,0,0}, {0,1,1,0}, {0,1,1,0}, {0,0,0,0}},
        {{0,0,0,0}, {0,1,1,0}, {0,1,1,0}, {0,0,0,0}},
        {{0,0,0,0}, {0,1,1,0}, {0,1,1,0}, {0,0,0,0}}
    },
    // T piece
    {
        {{0,0,0,0}, {0,1,0,0}, {1,1,1,0}, {0,0,0,0}},
        {{0,0,0,0}, {0,1,0,0}, {0,1,1,0}, {0,1,0,0}},
        {{0,0,0,0}, {0,0,0,0}, {1,1,1,0}, {0,1,0,0}},
        {{0,0,0,0}, {0,1,0,0}, {1,1,0,0}, {0,1,0,0}}
    },
    // S piece
    {
        {{0,0,0,0}, {0,1,1,0}, {1,1,0,0}, {0,0,0,0}},
        {{0,0,0,0}, {0,1,0,0}, {0,1,1,0}, {0,0,1,0}},
        {{0,0,0,0}, {0,0,0,0}, {0,1,1,0}, {1,1,0,0}},
        {{0,0,0,0}, {1,0,0,0}, {1,1,0,0}, {0,1,0,0}}
    },
    // Z piece
    {
        {{0,0,0,0}, {1,1,0,0}, {0,1,1,0}, {0,0,0,0}},
        {{0,0,0,0}, {0,0,1,0}, {0,1,1,0}, {0,1,0,0}},
        {{0,0,0,0}, {0,0,0,0}, {1,1,0,0}, {0,1,1,0}},
        {{0,0,0,0}, {0,1,0,0}, {1,1,0,0}, {1,0,0,0}}
    },
    // J piece
    {
        {{0,0,0,0}, {1,0,0,0}, {1,1,1,0}, {0,0,0,0}},
        {{0,0,0,0}, {0,1,1,0}, {0,1,0,0}, {0,1,0,0}},
        {{0,0,0,0}, {0,0,0,0}, {1,1,1,0}, {0,0,1,0}},
        {{0,0,0,0}, {0,1,0,0}, {0,1,0,0}, {1,1,0,0}}
    },
    // L piece
    {
        {{0,0,0,0}, {0,0,1,0}, {1,1,1,0}, {0,0,0,0}},
        {{0,0,0,0}, {0,1,0,0}, {0,1,0,0}, {0,1,1,0}},
        {{0,0,0,0}, {0,0,0,0}, {1,1,1,0}, {1,0,0,0}},
        {{0,0,0,0}, {1,1,0,0}, {0,1,0,0}, {0,1,0,0}}
    }
};

void init_piece(Piece_t *piece, PieceType_t type) {
    if (!piece) return;
    
    piece->type = type;
    piece->x = BOARD_WIDTH / 2 - 2;  // Center horizontally
    piece->y = 0;  // Start at top
    piece->rotation = 0;
    
    // Copy the template
    for (int i = 0; i < PIECE_SIZE; i++) {
        for (int j = 0; j < PIECE_SIZE; j++) {
            piece->shape[i][j] = piece_templates[type][0][i][j];
        }
    }
}

void rotate_piece(Piece_t *piece) {
    if (!piece) return;
    
    int new_rotation = (piece->rotation + 1) % 4;
    
    // Copy the new rotation template
    for (int i = 0; i < PIECE_SIZE; i++) {
        for (int j = 0; j < PIECE_SIZE; j++) {
            piece->shape[i][j] = piece_templates[piece->type][new_rotation][i][j];
        }
    }
    
    piece->rotation = new_rotation;
}

void copy_piece(const Piece_t *src, Piece_t *dest) {
    if (!src || !dest) return;
    
    memcpy(dest, src, sizeof(Piece_t));
}

PieceType_t get_random_piece_type(void) {
    static bool seeded = false;
    if (!seeded) {
        srand(time(NULL));
        seeded = true;
    }
    
    return rand() % PIECE_COUNT;
}

bool is_valid_position(const TetrisGame_t *game, const Piece_t *piece) {
    if (!game || !piece) return false;
    
    for (int i = 0; i < PIECE_SIZE; i++) {
        for (int j = 0; j < PIECE_SIZE; j++) {
            if (piece->shape[i][j]) {
                int board_x = piece->x + j;
                int board_y = piece->y + i;
                
                // Check boundaries
                if (board_x < 0 || board_x >= BOARD_WIDTH || 
                    board_y >= TOTAL_HEIGHT) {
                    return false;
                }
                
                // Check collision with existing blocks (only if not in spawn area)
                if (board_y >= 0 && game->board[board_y][board_x]) {
                    return false;
                }
            }
        }
    }
    
    return true;
}

void place_piece(TetrisGame_t *game, const Piece_t *piece) {
    if (!game || !piece) return;
    
    for (int i = 0; i < PIECE_SIZE; i++) {
        for (int j = 0; j < PIECE_SIZE; j++) {
            if (piece->shape[i][j]) {
                int board_x = piece->x + j;
                int board_y = piece->y + i;
                
                if (board_x >= 0 && board_x < BOARD_WIDTH && 
                    board_y >= 0 && board_y < TOTAL_HEIGHT) {
                    game->board[board_y][board_x] = 1;
                }
            }
        }
    }
}