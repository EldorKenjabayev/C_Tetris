#include "tetris_fsm.h"
#include "tetris_pieces.h"
#include <string.h>
#include <stdbool.h>

void fsm_process_action(TetrisGame_t *game, UserAction_t action, bool hold) {
    if (!game) return;
    
    switch (game->state) {
        case STATE_START:
            handle_start_state(game, action);
            break;
        case STATE_SPAWN:
            handle_spawn_state(game);
            break;
        case STATE_MOVING:
            handle_moving_state(game, action, hold);
            break;
        case STATE_SHIFTING:
            handle_shifting_state(game);
            break;
        case STATE_ATTACHING:
            handle_attaching_state(game);
            break;
        case STATE_GAME_OVER:
            handle_game_over_state(game, action);
            break;
        case STATE_PAUSE:
            handle_pause_state(game, action);
            break;
    }
}

void fsm_update_timer(TetrisGame_t *game) {
    if (!game || game->paused) return;
    
    if (game->state == STATE_MOVING) {
        game->timer++;
        if (game->timer >= game->speed) {
            game->timer = 0;
            game->state = STATE_SHIFTING;
        }
    }
}

void handle_start_state(TetrisGame_t *game, UserAction_t action) {
    if (action == Start) {
        // Initialize new game
        memset(game->board, 0, sizeof(game->board));
        game->score = 0;
        game->level = 1;
        game->speed = 48;  // Initial speed (frames)
        game->lines_cleared = 0;
        game->timer = 0;
        game->drop_timer = 0;
        game->paused = false;
        game->game_over = false;
        
        // Generate first piece
        init_piece(&game->next_piece, get_random_piece_type());
        game->state = STATE_SPAWN;
    }
    else if (action == Terminate) {
        game->game_over = true;
    }
}

void handle_spawn_state(TetrisGame_t *game) {
    // Move next piece to current and generate new next piece
    copy_piece(&game->next_piece, &game->current_piece);
    init_piece(&game->next_piece, get_random_piece_type());
    
    // Check if spawn position is valid
    if (!is_valid_position(game, &game->current_piece)) {
        game->state = STATE_GAME_OVER;
        game->game_over = true;
    } else {
        game->state = STATE_MOVING;
        game->timer = 0;
    }
}

void handle_moving_state(TetrisGame_t *game, UserAction_t action, bool hold) {
    Piece_t temp_piece = game->current_piece;
    
    switch (action) {
        case Left:
            temp_piece.x--;
            if (is_valid_position(game, &temp_piece)) {
                game->current_piece.x--;
            }
            break;
            
        case Right:
            temp_piece.x++;
            if (is_valid_position(game, &temp_piece)) {
                game->current_piece.x++;
            }
            break;
            
        case Down:
            if (hold) {
                // Drop piece to bottom
                while (is_valid_position(game, &temp_piece)) {
                    game->current_piece.y++;
                    temp_piece.y++;
                }
                game->current_piece.y--;
                game->state = STATE_ATTACHING;
            } else {
                // Soft drop
                temp_piece.y++;
                if (is_valid_position(game, &temp_piece)) {
                    game->current_piece.y++;
                } else {
                    game->state = STATE_ATTACHING;
                }
            }
            break;
            
        case Action:
            rotate_piece(&temp_piece);
            if (is_valid_position(game, &temp_piece)) {
                copy_piece(&temp_piece, &game->current_piece);
            }
            break;
            
        case Pause:
            game->state = STATE_PAUSE;
            game->paused = true;
            break;
            
        case Terminate:
            game->state = STATE_GAME_OVER;
            game->game_over = true;
            break;
            
        default:
            break;
    }
}

void handle_shifting_state(TetrisGame_t *game) {
    Piece_t temp_piece = game->current_piece;
    temp_piece.y++;
    
    if (is_valid_position(game, &temp_piece)) {
        game->current_piece.y++;
        game->state = STATE_MOVING;
    } else {
        game->state = STATE_ATTACHING;
    }
}

void handle_attaching_state(TetrisGame_t *game) {
    // Place the piece on the board
    place_piece(game, &game->current_piece);
    
    // Clear completed lines
    int lines_cleared = clear_completed_lines(game);
    
    if (lines_cleared > 0) {
        update_score(game, lines_cleared);
        update_level_and_speed(game);
    }
    
    // Check game over condition
    if (is_game_over(game)) {
        game->state = STATE_GAME_OVER;
        game->game_over = true;
    } else {
        game->state = STATE_SPAWN;
    }
}

void handle_game_over_state(TetrisGame_t *game, UserAction_t action) {
    if (action == Start) {
        game->state = STATE_START;
    } else if (action == Terminate) {
        // Keep in game over state
    }
}

void handle_pause_state(TetrisGame_t *game, UserAction_t action) {
    if (action == Pause || action == Start) {
        game->state = STATE_MOVING;
        game->paused = false;
    } else if (action == Terminate) {
        game->state = STATE_GAME_OVER;
        game->game_over = true;
        game->paused = false;
    }
}

int clear_completed_lines(TetrisGame_t *game) {
    int lines_cleared = 0;
    
    // Check from bottom to top
    for (int y = TOTAL_HEIGHT - 1; y >= BOARD_EXTRA_HEIGHT; y--) {
        bool line_complete = true;
        
        // Check if line is complete
        for (int x = 0; x < BOARD_WIDTH; x++) {
            if (!game->board[y][x]) {
                line_complete = false;
                break;
            }
        }
        
        if (line_complete) {
            // Move all lines above down by one
            for (int move_y = y; move_y > 0; move_y--) {
                for (int x = 0; x < BOARD_WIDTH; x++) {
                    game->board[move_y][x] = game->board[move_y - 1][x];
                }
            }
            
            // Clear top line
            for (int x = 0; x < BOARD_WIDTH; x++) {
                game->board[0][x] = 0;
            }
            
            lines_cleared++;
            y++; // Check the same line again
        }
    }
    
    return lines_cleared;
}

void update_score(TetrisGame_t *game, int lines_cleared) {
    int points = 0;
    
    switch (lines_cleared) {
        case 1: points = 100; break;
        case 2: points = 300; break;
        case 3: points = 700; break;
        case 4: points = 1500; break;
        default: points = 0; break;
    }
    
    game->score += points;
    game->lines_cleared += lines_cleared;
    
    // Update high score
    if (game->score > game->high_score) {
        game->high_score = game->score;
    }
}

void update_level_and_speed(TetrisGame_t *game) {
    int new_level = (game->score / 600) + 1;
    
    if (new_level > 10) {
        new_level = 10;
    }
    
    if (new_level != game->level) {
        game->level = new_level;
        // Increase speed (decrease timer threshold)
        game->speed = 48 - (game->level - 1) * 4;
        if (game->speed < 1) game->speed = 1;
    }
}

bool is_game_over(const TetrisGame_t *game) {
    // Check if any blocks are in the spawn area (top 4 rows)
    for (int y = 0; y < BOARD_EXTRA_HEIGHT; y++) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            if (game->board[y][x]) {
                return true;
            }
        }
    }
    return false;
}