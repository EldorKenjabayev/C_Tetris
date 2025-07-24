#include <check.h>
#include <stdlib.h>
#include "tetris.h"

// Test initialization
START_TEST(test_init_game) {
    init_game();
    GameInfo_t info = updateCurrentState();
    
    ck_assert_int_eq(info.score, 0);
    ck_assert_int_eq(info.level, 1);
    ck_assert_int_ge(info.speed, 1);
    ck_assert_ptr_ne(info.field, NULL);
    ck_assert_ptr_ne(info.next, NULL);
    
    // Clean up
    if (info.field) free_field_memory(info.field, BOARD_HEIGHT);
    if (info.next) free_field_memory(info.next, PIECE_SIZE);
}
END_TEST

// Test game start
START_TEST(test_game_start) {
    init_game();
    
    // Start the game
    userInput(Start, false);
    GameInfo_t info = updateCurrentState();
    
    ck_assert_int_eq(info.score, 0);
    ck_assert_int_eq(info.level, 1);
    ck_assert_ptr_ne(info.next, NULL);
    
    // Clean up
    if (info.field) free_field_memory(info.field, BOARD_HEIGHT);
    if (info.next) free_field_memory(info.next, PIECE_SIZE);
}
END_TEST

// Test movement
START_TEST(test_piece_movement) {
    init_game();
    userInput(Start, false);
    
    // Try moving left
    userInput(Left, false);
    GameInfo_t info = updateCurrentState();
    
    // The game state should update
    ck_assert_ptr_ne(info.field, NULL);
    
    // Clean up
    if (info.field) free_field_memory(info.field, BOARD_HEIGHT);
    if (info.next) free_field_memory(info.next, PIECE_SIZE);
}
END_TEST

// Test pause functionality
START_TEST(test_pause) {
    init_game();
    userInput(Start, false);
    
    // Pause the game
    userInput(Pause, false);
    GameInfo_t info = updateCurrentState();
    
    ck_assert_int_eq(info.pause, 1);
    
    // Resume the game
    userInput(Pause, false);
    info = updateCurrentState();
    
    ck_assert_int_eq(info.pause, 0);
    
    // Clean up
    if (info.field) free_field_memory(info.field, BOARD_HEIGHT);
    if (info.next) free_field_memory(info.next, PIECE_SIZE);
}
END_TEST

// Test scoring system
START_TEST(test_scoring) {
    init_game();
    userInput(Start, false);
    
    GameInfo_t info = updateCurrentState();
    int initial_score = info.score;
    
    // Score should start at 0
    ck_assert_int_eq(initial_score, 0);
    
    // Clean up
    if (info.field) free_field_memory(info.field, BOARD_HEIGHT);
    if (info.next) free_field_memory(info.next, PIECE_SIZE);
}
END_TEST

// Test high score
START_TEST(test_high_score) {
    int test_score = 1000;
    save_high_score(test_score);
    
    int loaded_score = load_high_score();
    ck_assert_int_eq(loaded_score, test_score);
}
END_TEST

// Test memory allocation
START_TEST(test_memory_allocation) {
    init_game();
    GameInfo_t info = updateCurrentState();
    
    // Check that field is properly allocated
    ck_assert_ptr_ne(info.field, NULL);
    ck_assert_ptr_ne(info.next, NULL);
    
    // Check field dimensions
    for (int y = 0; y < BOARD_HEIGHT; y++) {
        ck_assert_ptr_ne(info.field[y], NULL);
    }
    
    for (int y = 0; y < PIECE_SIZE; y++) {
        ck_assert_ptr_ne(info.next[y], NULL);
    }
    
    // Clean up
    if (info.field) free_field_memory(info.field, BOARD_HEIGHT);
    if (info.next) free_field_memory(info.next, PIECE_SIZE);
}
END_TEST

// Test piece rotation
START_TEST(test_piece_rotation) {
    init_game();
    userInput(Start, false);
    
    // Try rotating piece
    userInput(Action, false);
    GameInfo_t info = updateCurrentState();
    
    // Game should still be running
    ck_assert_ptr_ne(info.field, NULL);
    
    // Clean up
    if (info.field) free_field_memory(info.field, BOARD_HEIGHT);
    if (info.next) free_field_memory(info.next, PIECE_SIZE);
}
END_TEST

// Test game termination
START_TEST(test_termination) {
    init_game();
    userInput(Start, false);
    
    // Terminate the game
    userInput(Terminate, false);
    GameInfo_t info = updateCurrentState();
    
    // Game should still provide info
    ck_assert_ptr_ne(info.field, NULL);
    
    // Clean up
    if (info.field) free_field_memory(info.field, BOARD_HEIGHT);
    if (info.next) free_field_memory(info.next, PIECE_SIZE);
}
END_TEST

Suite *tetris_suite(void) {
    Suite *s;
    TCase *tc_core;
    
    s = suite_create("Tetris");
    tc_core = tcase_create("Core");
    
    tcase_add_test(tc_core, test_init_game);
    tcase_add_test(tc_core, test_game_start);
    tcase_add_test(tc_core, test_piece_movement);
    tcase_add_test(tc_core, test_pause);
    tcase_add_test(tc_core, test_scoring);
    tcase_add_test(tc_core, test_high_score);
    tcase_add_test(tc_core, test_memory_allocation);
    tcase_add_test(tc_core, test_piece_rotation);
    tcase_add_test(tc_core, test_termination);
    
    suite_add_tcase(s, tc_core);
    
    return s;
}

int main(void) {
    int number_failed;
    Suite *s;
    SRunner *sr;
    
    s = tetris_suite();
    sr = srunner_create(s);
    
    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}