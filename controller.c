#include <stdio.h>
#include "tetromino.h"

#define BOARD_W 10
#define BOARD_H 22

typedef struct {
    char type;
    int x;
    int y;
    int orientation;
} tetromino_t;

tetromino_t prototypes[7] = {
    {'I', 3, 0, 0},
    {'J', 3, 0, 0},
    {'L', 3, 0, 0},
    {'O', 3, 0, 0},
    {'S', 3, 0, 0},
    {'T', 3, 0, 0},
    {'Z', 3, 0, 0},
};

typedef struct {
    int lines_cleared;
    tetromino_t current_T;
    tetromino_t next_T;
    char board[BOARD_H][BOARD_W];
} game_state_t;

static game_state_t game_state;

// Forward declarations
void initialize_state();
void print_state(FILE* stream, float ttt);
int get_level();

int main(int argc, char** argv)
{
    initialize_state();

    print_state(stdout, 1.0f);
}

void initialize_state()
{
    game_state.lines_cleared = 0;
    // TODO: Initialize Random Generator and fill tetrominoes
    game_state.current_T = prototypes[0];
    game_state.next_T = prototypes[1];
    int x, y;
    for (y = 0; y < BOARD_H; ++y)
        for (x = 0; x < BOARD_H; ++x)
            game_state.board[y][x] = '.';
}

void print_state(FILE* stream, float ttt)
{
    fprintf(stream, "%f %d %d %c %c\n",
            ttt,
            get_level(),
            game_state.lines_cleared,
            game_state.current_T.type,
            game_state.next_T.type);

    int y;
    for (y = 0; y < BOARD_H; ++y)
        fprintf(stream, "%.10s\n", &game_state.board[y]);
}

int get_level()
{
    return game_state.lines_cleared / 10;
}