#ifndef TETRIS_H
#define TETRIS_H

#include <stdbool.h>
#include "tetris_types.h"

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
