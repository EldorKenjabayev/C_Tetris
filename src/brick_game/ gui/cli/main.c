#include "gui.h"
#include "tetris.h"
#include <unistd.h>
#include <signal.h>

static volatile bool running = true;

void signal_handler(int sig) {
    (void)sig;
    running = false;
}

void game_loop(void) {
    bool game_started = false;
    bool hold_key = false;
    UserAction_t last_action = -1;
    int hold_counter = 0;
    
    while (running) {
        UserAction_t action = get_user_input();
        
        // Handle key holding for movement
        if (action == last_action && (action == Left || action == Right || action == Down)) {
            hold_counter++;
            if (hold_counter > 5) {  // Start holding after 5 frames
                hold_key = true;
            }
        } else {
            hold_counter = 0;
            hold_key = false;
        }
        
        if (action != -1) {
            last_action = action;
            userInput(action, hold_key);
            
            if (action == Terminate) {
                break;
            }
            
            if (action == Start) {
                game_started = true;
            }
        }
        
        GameInfo_t info = updateCurrentState();
        
        if (!game_started) {
            show_instructions();
        } else {
            draw_game(&info);
        }
        
        // Free allocated memory
        if (info.field) {
            free_field_memory(info.field, BOARD_HEIGHT);
        }
        if (info.next) {
            free_field_memory(info.next, PIECE_SIZE);
        }
        
        // Control game speed (60 FPS)
        usleep(16667);  // ~16.67ms = 60 FPS
    }
}

int main(void) {
    // Set up signal handler for graceful exit
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    // Initialize game and GUI
    init_game();
    init_gui();
    
    // Main game loop
    game_loop();
    
    // Cleanup
    cleanup_game();
    cleanup_gui();
    
    return 0;
}