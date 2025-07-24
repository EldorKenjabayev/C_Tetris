#ifndef TETRIS_PIECES_H
#define TETRIS_PIECES_H

#include <stdbool.h>
#include "tetris_types.h"

// Piece templates array - extern declaration
extern const int piece_templates[PIECE_COUNT][4][PIECE_SIZE][PIECE_SIZE];

// Function declarations
void init_piece(Piece_t *piece, PieceType_t type);
void rotate_piece(Piece_t *piece);
void copy_piece(const Piece_t *src, Piece_t *dest);
PieceType_t get_random_piece_type(void);
bool is_valid_position(const TetrisGame_t *game, const Piece_t *piece);
void place_piece(TetrisGame_t *game, const Piece_t *piece);

#endif  // TETRIS_PIECES_H
