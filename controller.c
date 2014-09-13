
#include "tetromino.h"

#define BOARD_W 10
#define BOARD_H 22

typedef struct {
    char type;
    int x;
    int y;
    char** layout;
} tetromino_t;

typedef struct {
    int lines_cleared;
    tetromino_t current_T;
    tetromino_t next_T;
    char board[BOARD_H][BOARD_W];
} game_state_t;

static game_state_t game_state;

int main(int argc, char** argv)
{
    int i,j,k;
    for (k = 0; k < 4; ++k)
    {
        for (i = 0; i < 4; ++i)
        {
            for (j = 0; j < 4; ++j)
                putchar(layouts['T'-'A'][k][i][j]);

            putchar('\n');
        }
        putchar('\n');
    }
}