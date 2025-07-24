#include "tetris.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Internal game state structures
typedef enum {
    STATE_START,
    STATE_SPAWN,
    STATE_MOVING,
    STATE_SHIFTING,
    STATE_ATTACHING,
    STATE_GAME_OVER,
    STATE_PAUSE
} GameState_t;

typedef enum {
    PIECE_I, PIECE_O, PIECE_T, PIECE_S, PIECE_Z, PIECE_J, PIECE_L, PIECE_COUNT
} PieceType_t;

typedef struct {
    PieceType_t type;
    int x, y;
    int rotation;
    int shape[PIECE_SIZE][PIECE_SIZE];
} Piece_t;

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
} TetrisGame_t;

// Global game state
static TetrisGame_t g_game = {0};
static bool g_initialized = false;

// Piece templates [piece_type][rotation][y][x]
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

// Function declarations
void init_piece(Piece_t *piece, PieceType_t type);
void rotate_piece(Piece_t *piece);
void copy_piece(const Piece_t *src, Piece_t *dest);
PieceType_t get_random_piece_type(void);
bool is_valid_position(const TetrisGame_t *game, const Piece_t *piece);
void place_piece(TetrisGame_t *game, const Piece_t *piece);
void fsm_process_action(TetrisGame_t *game, UserAction_t action, bool hold);
void fsm_update_timer(TetrisGame_t *game);
int clear_completed_lines(TetrisGame_t *game);
void update_score(TetrisGame_t *game, int lines_cleared);
void update_level_and_speed(TetrisGame_t *game);
bool is_game_over(const TetrisGame_t *game);
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

// Piece functions
void init_piece(Piece_t *piece, PieceType_t type) {
    if (!piece) return;
    
    piece->type = type;
    piece->x = BOARD_WIDTH / 2 - 2;
    piece->y = 0;
    piece->rotation = 0;
    
    for (int i = 0; i < PIECE_SIZE; i++) {
        for (int j = 0; j < PIECE_SIZE; j++) {
            piece->shape[i][j] = piece_templates[type][0][i][j];
        }
    }
}

void rotate_piece(Piece_t *piece) {
    if (!piece) return;
    
    int new_rotation = (piece->rotation + 1) % 4;
    
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
                
                if (board_x < 0 || board_x >= BOARD_WIDTH || 
                    board_y >= TOTAL_HEIGHT) {
                    return false;
                }
                
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

// FSM implementation
void fsm_process_action(TetrisGame_t *game, UserAction_t action, bool hold) {
    if (!game) return;
    
    switch (game->state) {
        case STATE_START:
            if (action == Start) {
                memset(game->board, 0, sizeof(game->board));
                game->score = 0;
                game->level = 1;
                game->speed = 48;
                game->lines_cleared = 0;
                game->timer = 0;
                game->paused = false;
                game->game_over = false;
                
                init_piece(&game->next_piece, get_random_piece_type());
                game->state = STATE_SPAWN;
            }
            break;
            
        case STATE_SPAWN:
            copy_piece(&game->next_piece, &game->current_piece);
            init_piece(&game->next_piece, get_random_piece_type());
            
            if (!is_valid_position(game, &game->current_piece)) {
                game->state = STATE_GAME_OVER;
                game->game_over = true;
            } else {
                game->state = STATE_MOVING;
                game->timer = 0;
            }
            break;
            
        case STATE_MOVING:
            {
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
                            while (is_valid_position(game, &temp_piece)) {
                                game->current_piece.y++;
                                temp_piece.y++;
                            }
                            game->current_piece.y--;
                            game->state = STATE_ATTACHING;
                        } else {
                            temp_piece.y++;
                            if (is_valid_position(game, &temp_piece)) {
                                game->current_piece.y++;
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
            break;
            
        case STATE_SHIFTING:
            {
                Piece_t temp_piece = game->current_piece;
                temp_piece.y++;
                
                if (is_valid_position(game, &temp_piece)) {
                    game->current_piece.y++;
                    game->state = STATE_MOVING;
                } else {
                    game->state = STATE_ATTACHING;
                }
            }
            break;
            
        case STATE_ATTACHING:
            place_piece(game, &game->current_piece);
            
            int lines_cleared = clear_completed_lines(game);
            if (lines_cleared > 0) {
                update_score(game, lines_cleared);
                update_level_and_speed(game);
            }
            
            if (is_game_over(game)) {
                game->state = STATE_GAME_OVER;
                game->game_over = true;
            } else {
                game->state = STATE_SPAWN;
            }
            break;
            
        case STATE_PAUSE:
            if (action == Pause || action == Start) {
                game->state = STATE_MOVING;
                game->paused = false;
            } else if (action == Terminate) {
                game->state = STATE_GAME_OVER;
                game->game_over = true;
                game->paused = false;
            }
            break;
            
        case STATE_GAME_OVER:
            if (action == Start) {
                game->state = STATE_START;
            }
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

int clear_completed_lines(TetrisGame_t *game) {
    int lines_cleared = 0;
    
    for (int y = TOTAL_HEIGHT - 1; y >= BOARD_EXTRA_HEIGHT; y--) {
        bool line_complete = true;
        
        for (int x = 0; x < BOARD_WIDTH; x++) {
            if (!game->board[y][x]) {
                line_complete = false;
                break;
            }
        }
        
        if (line_complete) {
            for (int move_y = y; move_y > 0; move_y--) {
                for (int x = 0; x < BOARD_WIDTH; x++) {
                    game->board[move_y][x] = game->board[move_y - 1][x];
                }
            }
            
            for (int x = 0; x < BOARD_WIDTH; x++) {
                game->board[0][x] = 0;
            }
            
            lines_cleared++;
            y++;
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
        game->speed = 48 - (game->level - 1) * 4;
        if (game->speed < 1) game->speed = 1;
    }
}

bool is_game_over(const TetrisGame_t *game) {
    for (int y = 0; y < BOARD_EXTRA_HEIGHT; y++) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            if (game->board[y][x]) {
                return true;
            }
        }
    }
    return false;
}

void prepare_game_info(GameInfo_t *info) {
    if (!info) return;
    
    allocate_field_memory(&info->field, BOARD_HEIGHT, BOARD_WIDTH);
    allocate_field_memory(&info->next, PIECE_SIZE, PIECE_SIZE);
    
    for (int y = 0; y < BOARD_HEIGHT; y++) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            info->field[y][x] = g_game.board[y + BOARD_EXTRA_HEIGHT][x];
        }
    }
    
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