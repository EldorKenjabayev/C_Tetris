#ifndef TETRIS_FSM_H
#define TETRIS_FSM_H

#include <stdbool.h>
#include "tetris_types.h"

// FSM function declarations
void fsm_process_action(TetrisGame_t *game, UserAction_t action, bool hold);
void fsm_update_timer(TetrisGame_t *game);

// State handler functions
void handle_start_state(TetrisGame_t *game, UserAction_t action);
void handle_spawn_state(TetrisGame_t *game);
void handle_moving_state(TetrisGame_t *game, UserAction_t action, bool hold);
void handle_shifting_state(TetrisGame_t *game);
void handle_attaching_state(TetrisGame_t *game);
void handle_game_over_state(TetrisGame_t *game, UserAction_t action);
void handle_pause_state(TetrisGame_t *game, UserAction_t action);

// Game logic functions
int clear_completed_lines(TetrisGame_t *game);
void update_score(TetrisGame_t *game, int lines_cleared);
void update_level_and_speed(TetrisGame_t *game);
bool is_game_over(const TetrisGame_t *game);

#endif  // TETRIS_FSM_H
