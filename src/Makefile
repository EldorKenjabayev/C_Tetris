CC = gcc
CFLAGS = -Wall -Werror -Wextra -std=c11 -g
LDFLAGS = -lncurses -lm
TEST_LDFLAGS = -lcheck -lm -lpthread -lsubunit

# Directories
BUILD_DIR = build
SRC_DIR = .
BRICK_GAME_DIR = $(SRC_DIR)/brick_game/tetris
GUI_DIR = $(SRC_DIR)/gui/cli
TEST_DIR = $(SRC_DIR)/tests

# Create build subdirectories
BRICK_GAME_BUILD = $(BUILD_DIR)/brick_game/tetris
GUI_BUILD = $(BUILD_DIR)/gui/cli
TEST_BUILD = $(BUILD_DIR)/tests

# Source files
BRICK_GAME_SOURCES = $(wildcard $(BRICK_GAME_DIR)/*.c)
GUI_SOURCES = $(wildcard $(GUI_DIR)/*.c)
TEST_SOURCES = $(wildcard $(TEST_DIR)/*.c)

# Object files
BRICK_GAME_OBJECTS = $(BRICK_GAME_SOURCES:$(BRICK_GAME_DIR)/%.c=$(BRICK_GAME_BUILD)/%.o)
GUI_OBJECTS = $(GUI_SOURCES:$(GUI_DIR)/%.c=$(GUI_BUILD)/%.o)
TEST_OBJECTS = $(TEST_SOURCES:$(TEST_DIR)/%.c=$(TEST_BUILD)/%.o)

# Targets
TARGET = tetris
TEST_TARGET = $(BUILD_DIR)/test_tetris
LIBRARY = $(BUILD_DIR)/libtetris.a

# Install directory
INSTALL_DIR = /usr/local/bin

.PHONY: all clean test gcov_report install uninstall dist dvi

all: $(TARGET)

# Create build directories
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)
	mkdir -p $(BRICK_GAME_BUILD)
	mkdir -p $(GUI_BUILD)
	mkdir -p $(TEST_BUILD)

# Main target
$(TARGET): $(BUILD_DIR) $(LIBRARY) $(GUI_OBJECTS)
	$(CC) $(GUI_OBJECTS) -L$(BUILD_DIR) -ltetris $(LDFLAGS) -o $@

# Static library
$(LIBRARY): $(BRICK_GAME_OBJECTS)
	ar rcs $@ $^

# Compile brick_game objects
$(BRICK_GAME_BUILD)/%.o: $(BRICK_GAME_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(BRICK_GAME_DIR) -c $< -o $@

# Compile GUI objects
$(GUI_BUILD)/%.o: $(GUI_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(BRICK_GAME_DIR) -I$(GUI_DIR) -c $< -o $@

# Compile test objects
$(TEST_BUILD)/%.o: $(TEST_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(BRICK_GAME_DIR) -c $< -o $@

# Test target
test: $(TEST_TARGET)
	./$(TEST_TARGET)

$(TEST_TARGET): $(BUILD_DIR) $(BRICK_GAME_OBJECTS) $(TEST_OBJECTS)
	$(CC) $(BRICK_GAME_OBJECTS) $(TEST_OBJECTS) $(TEST_LDFLAGS) -o $@

# Coverage report
gcov_report: CFLAGS += --coverage
gcov_report: LDFLAGS += --coverage
gcov_report: TEST_LDFLAGS += --coverage
gcov_report: clean test
	@mkdir -p report
	gcov $(BRICK_GAME_SOURCES)
	lcov -t "tetris" -o tetris.info -c -d . --no-external
	genhtml -o report tetris.info
	@echo "Coverage report generated in report/ directory"

# Install
install: $(TARGET)
	cp $(TARGET) $(INSTALL_DIR)/

# Uninstall
uninstall:
	rm -f $(INSTALL_DIR)/$(TARGET)

# Create documentation
dvi:
	@echo "Creating documentation..."
	@echo "Documentation would be generated here"

# Create distribution
dist: clean
	tar --exclude='./build' --exclude='./tetris' --exclude='*.tar.gz' -czf tetris.tar.gz .

# Clean
clean:
	rm -rf $(BUILD_DIR) $(TARGET) *.gcno *.gcda *.gcov *.info report/

# Help
help:
	@echo "Available targets:"
	@echo "  all        - Build the project"
	@echo "  test       - Run tests"
	@echo "  gcov_report- Generate coverage report"
	@echo "  install    - Install to system"
	@echo "  uninstall  - Remove from system"
	@echo "  clean      - Clean build files"
	@echo "  dist       - Create distribution archive"
	@echo "  help       - Show this help"