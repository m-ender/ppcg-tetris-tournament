#include <stdio.h>
#include <stdlib.h>
#include <time.h>
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
    int score;
    tetromino_t current_T;
    tetromino_t next_T;
    char board[BOARD_H][BOARD_W];
} game_state_t;

static game_state_t game_state;

// Forward declarations
void initialize_state();
void print_state(FILE* stream, float ttt);
int get_level();
int next_tetromino();
int collision();
int gravity_tick();

int main(int argc, char** argv)
{
    srand(time(NULL));

    initialize_state();

    print_state(stdout, 1.0f);
    printf("Score: %d\n", game_state.score);

    char input;
    while (input = getchar())
    {
        switch (input)
        {
        case 'G':
            if (gravity_tick()) return 0;
            print_state(stdout, 1.0f);
            printf("Score: %d\n", game_state.score);
            break;
        }
    }

    return 0;
}

void render_tetromino(char character)
{
    int i;
    tetromino_t* tet = &game_state.current_T;
    for (i = 0; i < 4; ++i)
    {
        char* coord = &layouts[tet->type - 'A'][0][i][0];
        int x = tet->x + coord[0];
        int y = tet->y + coord[1];
        game_state.board[y][x] = character;
    }
}

void initialize_state()
{
    game_state.lines_cleared = 0;
    game_state.score = 0;

    game_state.current_T = prototypes[next_tetromino()];
    game_state.next_T = prototypes[next_tetromino()];

    int x, y;
    for (y = 0; y < BOARD_H; ++y)
        for (x = 0; x < BOARD_W; ++x)
            game_state.board[y][x] = '.';

    render_tetromino('*');
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

int next_tetromino()
{
    static int first_run = 1;
    static int bag[7];
    static int next;

    if (!next)
    {
        int i, j;
        int rands[7];
        for (i = 0; i < 7; ++i)
            rands[i] = rand();

        // Fill the bag with the indices from largest to smallest
        // random number
        for (i = 0; i < 7; ++i)
        {
            int max = -1;
            int j, max_j;
            for (j = 0; j < 7; ++j)
                if (rands[j] > max)
                {
                    max = rands[j];
                    max_j = j;
                }

            rands[max_j] = -1;
            bag[i] = max_j;
        }

        if (first_run)
        {
            first_run = 0;
            next = rand() % 7;
        }
    }

    int result = bag[next++];
    next %= 7;

    return result;
}

int collision()
{
    int i, j;
    tetromino_t* tet = &game_state.current_T;
    for (i = 0; i < 4; ++i)
    {
        char* coords = &layouts[tet->type - 'A'][tet->orientation][i][0];
        int x = tet->x + coords[0];
        int y = tet->y + coords[1];

        if (x < 0 || y < 0 || x >= BOARD_W || y >= BOARD_H ||
            game_state.board[y][x] == '#')
            return 1;
    }

    return 0;
}

int gravity_tick()
{
    int i;
    render_tetromino('.');

    tetromino_t* tet = &game_state.current_T;
    ++tet->y;

    if(collision())
    {
        --tet->y;

        render_tetromino('#');

        game_state.current_T = game_state.next_T;
        game_state.next_T = prototypes[next_tetromino()];

        int x, y, hit_top = 0;
        for (y = 0; y < 2; ++y)
            for (x = 0; x < BOARD_W; ++x)
                if (game_state.board[y][x] == '#')
                    hit_top == 1;

        if(hit_top || collision())
        {
            printf("Game Over! Final Score: %d\n", game_state.score);
            return 1;
        }
    }

    render_tetromino('*');
    return 0;
}