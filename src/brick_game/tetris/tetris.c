#include "tetris.h"
#include "tetris_fsm.h"
#include "tetris_pieces.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Global game state
static TetrisGame_t g_game = {0};
static bool g_initialized = false;

// Function declarations
void prepare_game_info(GameInfo_t *info);
void allocate_field_memory(int ***field, int height, int width);

// Main API implementation
void userInput(UserAction_t action, bool hold) {
    if (!g_initialized) {
        init_game();
    }
    fsm_process_action(&g_game, action, hold);
}

GameInfo_t updateCurrentState(void) {
    if (!g_initialized) {
        init_game();
    }
    
    fsm_update_timer(&g_game);
    
    GameInfo_t info = {0};
    prepare_game_info(&info);
    
    return info;
}

void init_game(void) {
    if (g_initialized) return;
    
    memset(&g_game, 0, sizeof(TetrisGame_t));
    g_game.state = STATE_START;
    g_game.high_score = load_high_score();
    g_game.speed = 48;
    g_game.level = 1;
    
    g_initialized = true;
}

void cleanup_game(void) {
    if (g_game.score > 0) {
        save_high_score(g_game.high_score);
    }
    g_initialized = false;
}

void prepare_game_info(GameInfo_t *info) {
    if (!info) return;
    
    allocate_field_memory(&info->field, BOARD_HEIGHT, BOARD_WIDTH);
    allocate_field_memory(&info->next, PIECE_SIZE, PIECE_SIZE);
    
    // Copy visible game field (excluding spawn area)
    for (int y = 0; y < BOARD_HEIGHT; y++) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            info->field[y][x] = g_game.board[y + BOARD_EXTRA_HEIGHT][x];
        }
    }
    
    // Draw current piece on field if it's visible
    if (g_game.state == STATE_MOVING || g_game.state == STATE_SHIFTING) {
        for (int i = 0; i < PIECE_SIZE; i++) {
            for (int j = 0; j < PIECE_SIZE; j++) {
                if (g_game.current_piece.shape[i][j]) {
                    int board_x = g_game.current_piece.x + j;
                    int board_y = g_game.current_piece.y + i - BOARD_EXTRA_HEIGHT;
                    
                    if (board_x >= 0 && board_x < BOARD_WIDTH && 
                        board_y >= 0 && board_y < BOARD_HEIGHT) {
                        info->field[board_y][board_x] = 1;
                    }
                }
            }
        }
    }
    
    // Copy next piece
    for (int i = 0; i < PIECE_SIZE; i++) {
        for (int j = 0; j < PIECE_SIZE; j++) {
            info->next[i][j] = g_game.next_piece.shape[i][j];
        }
    }
    
    info->score = g_game.score;
    info->high_score = g_game.high_score;
    info->level = g_game.level;
    info->speed = g_game.speed;
    info->pause = g_game.paused ? 1 : 0;
}

void allocate_field_memory(int ***field, int height, int width) {
    if (!field) return;
    
    *field = malloc(height * sizeof(int*));
    if (*field) {
        for (int i = 0; i < height; i++) {
            (*field)[i] = malloc(width * sizeof(int));
            if ((*field)[i]) {
                memset((*field)[i], 0, width * sizeof(int));
            }
        }
    }
}

void free_field_memory(int **field, int height) {
    if (!field) return;
    
    for (int i = 0; i < height; i++) {
        free(field[i]);
    }
    free(field);
}

void save_high_score(int score) {
    FILE *file = fopen("high_score.dat", "w");
    if (file) {
        fprintf(file, "%d", score);
        fclose(file);
    }
}

int load_high_score(void) {
    FILE *file = fopen("high_score.dat", "r");
    int score = 0;
    
    if (file) {
        fscanf(file, "%d", &score);
        fclose(file);
    }
    
    return score;
}