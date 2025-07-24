#include "gui.h"
#include <string.h>
#include <unistd.h>

void init_gui(void) {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    curs_set(0);
    
    // Initialize colors if supported
    if (has_colors()) {
        start_color();
        init_pair(COLOR_FIELD, COLOR_WHITE, COLOR_BLACK);
        init_pair(COLOR_BORDER, COLOR_YELLOW, COLOR_BLACK);
        init_pair(COLOR_TEXT, COLOR_GREEN, COLOR_BLACK);
        init_pair(COLOR_PIECE, COLOR_CYAN, COLOR_BLACK);
    }
    
    clear();
    refresh();
}

void cleanup_gui(void) {
    endwin();
}

void draw_game(const GameInfo_t *info) {
    if (!info) return;
    
    clear();
    
    draw_border();
    draw_field(info);
    draw_info_panel(info);
    draw_next_piece(info);
    
    if (info->pause) {
        draw_pause();
    }
    
    refresh();
}

void draw_field(const GameInfo_t *info) {
    if (!info || !info->field) return;
    
    attron(COLOR_PAIR(COLOR_FIELD));
    
    for (int y = 0; y < BOARD_HEIGHT; y++) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            int screen_y = FIELD_START_Y + y;
            int screen_x = FIELD_START_X + x * 2;
            
            if (info->field[y][x]) {
                mvaddstr(screen_y, screen_x, "██");
            } else {
                mvaddstr(screen_y, screen_x, "  ");
            }
        }
    }
    
    attroff(COLOR_PAIR(COLOR_FIELD));
}

void draw_border(void) {
    attron(COLOR_PAIR(COLOR_BORDER));
    
    // Draw game field border
    int field_width = BOARD_WIDTH * 2;
    int field_height = BOARD_HEIGHT;
    
    // Top and bottom borders
    for (int x = 0; x <= field_width + 1; x++) {
        mvaddch(FIELD_START_Y - 1, FIELD_START_X + x - 1, '-');
        mvaddch(FIELD_START_Y + field_height, FIELD_START_X + x - 1, '-');
    }
    
    // Left and right borders
    for (int y = 0; y < field_height; y++) {
        mvaddch(FIELD_START_Y + y, FIELD_START_X - 1, '|');
        mvaddch(FIELD_START_Y + y, FIELD_START_X + field_width, '|');
    }
    
    // Corners
    mvaddch(FIELD_START_Y - 1, FIELD_START_X - 1, '+');
    mvaddch(FIELD_START_Y - 1, FIELD_START_X + field_width, '+');
    mvaddch(FIELD_START_Y + field_height, FIELD_START_X - 1, '+');
    mvaddch(FIELD_START_Y + field_height, FIELD_START_X + field_width, '+');
    
    attroff(COLOR_PAIR(COLOR_BORDER));
}

void draw_info_panel(const GameInfo_t *info) {
    if (!info) return;
    
    attron(COLOR_PAIR(COLOR_TEXT));
    
    mvprintw(FIELD_START_Y, INFO_PANEL_X, "TETRIS");
    mvprintw(FIELD_START_Y + 2, INFO_PANEL_X, "Score: %d", info->score);
    mvprintw(FIELD_START_Y + 3, INFO_PANEL_X, "High:  %d", info->high_score);
    mvprintw(FIELD_START_Y + 4, INFO_PANEL_X, "Level: %d", info->level);
    mvprintw(FIELD_START_Y + 5, INFO_PANEL_X, "Speed: %d", info->speed);
    
    mvprintw(FIELD_START_Y + 7, INFO_PANEL_X, "Next:");
    
    // Controls
    mvprintw(FIELD_START_Y + 12, INFO_PANEL_X, "Controls:");
    mvprintw(FIELD_START_Y + 13, INFO_PANEL_X, "A/D - Move");
    mvprintw(FIELD_START_Y + 14, INFO_PANEL_X, "S - Drop");
    mvprintw(FIELD_START_Y + 15, INFO_PANEL_X, "W - Rotate");
    mvprintw(FIELD_START_Y + 16, INFO_PANEL_X, "P - Pause");
    mvprintw(FIELD_START_Y + 17, INFO_PANEL_X, "Q - Quit");
    mvprintw(FIELD_START_Y + 18, INFO_PANEL_X, "R - Restart");
    
    attroff(COLOR_PAIR(COLOR_TEXT));
}

void draw_next_piece(const GameInfo_t *info) {
    if (!info || !info->next) return;
    
    attron(COLOR_PAIR(COLOR_PIECE));
    
    for (int y = 0; y < PIECE_SIZE; y++) {
        for (int x = 0; x < PIECE_SIZE; x++) {
            if (info->next[y][x]) {
                int screen_y = NEXT_PIECE_Y + y;
                int screen_x = NEXT_PIECE_X + x * 2;
                mvaddstr(screen_y, screen_x, "██");
            }
        }
    }
    
    attroff(COLOR_PAIR(COLOR_PIECE));
}

void draw_game_over(void) {
    attron(COLOR_PAIR(COLOR_TEXT) | A_BOLD);
    
    int center_y = LINES / 2;
    int center_x = COLS / 2;
    
    mvprintw(center_y - 2, center_x - 5, "GAME OVER");
    mvprintw(center_y, center_x - 8, "Press R to restart");
    mvprintw(center_y + 1, center_x - 7, "Press Q to quit");
    
    attroff(COLOR_PAIR(COLOR_TEXT) | A_BOLD);
    refresh();
}

void draw_pause(void) {
    attron(COLOR_PAIR(COLOR_TEXT) | A_BOLD);
    
    int center_y = LINES / 2;
    int center_x = COLS / 2;
    
    mvprintw(center_y, center_x - 3, "PAUSED");
    mvprintw(center_y + 1, center_x - 8, "Press P to continue");
    
    attroff(COLOR_PAIR(COLOR_TEXT) | A_BOLD);
}

UserAction_t get_user_input(void) {
    int ch = getch();
    
    switch (ch) {
        case 'r':
        case 'R':
            return Start;
        case 'p':
        case 'P':
            return Pause;
        case 'q':
        case 'Q':
        case 27:  // ESC
            return Terminate;
        case 'a':
        case 'A':
        case KEY_LEFT:
            return Left;
        case 'd':
        case 'D':
        case KEY_RIGHT:
            return Right;
        case 's':
        case 'S':
        case KEY_DOWN:
            return Down;
        case 'w':
        case 'W':
        case KEY_UP:
        case ' ':  // Space for rotation
            return Action;
        default:
            return -1;  // No valid input
    }
}

void show_instructions(void) {
    clear();
    attron(COLOR_PAIR(COLOR_TEXT));
    
    mvprintw(2, 2, "TETRIS - Instructions");
    mvprintw(4, 2, "Arrange falling pieces to form complete horizontal lines.");
    mvprintw(5, 2, "Complete lines will disappear and award points.");
    mvprintw(6, 2, "Game ends when pieces reach the top.");
    
    mvprintw(8, 2, "Controls:");
    mvprintw(9, 4, "A/← - Move left");
    mvprintw(10, 4, "D/→ - Move right");
    mvprintw(11, 4, "S/↓ - Soft drop");
    mvprintw(12, 4, "W/↑/Space - Rotate piece");
    mvprintw(13, 4, "P - Pause/Resume");
    mvprintw(14, 4, "R - Restart game");
    mvprintw(15, 4, "Q/ESC - Quit");
    
    mvprintw(17, 2, "Scoring:");
    mvprintw(18, 4, "1 line  = 100 points");
    mvprintw(19, 4, "2 lines = 300 points");
    mvprintw(20, 4, "3 lines = 700 points");
    mvprintw(21, 4, "4 lines = 1500 points");
    
    mvprintw(23, 2, "Press R to start playing!");
    
    attroff(COLOR_PAIR(COLOR_TEXT));
    refresh();
}