#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "tetromino.h"

#define BOARD_W 10
#define BOARD_H 22

// Portable high-res timer
#ifdef WIN32   // Windows system specific
#include <windows.h>
#else          // Unix based system specific
#include <sys/time.h>
#endif

double startTimeInMicroSec;
#ifdef WIN32
    LARGE_INTEGER frequency; // ticks per second
    LARGE_INTEGER startCount;
#else
    struct timeval startCount;
#endif

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
void initialize_timer();
double get_elapsed_time();
void initialize_state();
void print_state(FILE* stream, float ttt);
int get_level();
int next_tetromino();
int collision();
int gravity_tick();
int move_down();
void move_left();
void move_right();
void rotate();

int main(int argc, char** argv)
{
    srand(time(NULL));

    initialize_state();

    initialize_timer();

    print_state(stdout, 1.0f);
    printf("Score: %d\n", game_state.score);

    char input;

    double dTime = 0;
    while (1)
    {
        dTime += get_elapsed_time();
        if (dTime > 1)
        {
            printf("Tick.\n");
            dTime -= 1;
        }
    }

    while (input = getchar())
    {
        switch (input)
        {
        case 'D':
            if (move_down()) return 0;
            break;
        case 'G':
            if (gravity_tick()) return 0;
            break;
        case 'L':
            move_left();
            break;
        case 'R':
            move_right();
            break;
        case 'U':
            rotate();
            break;
        }
        print_state(stdout, 1.0f);
        printf("Score: %d\n", game_state.score);
    }

    return 0;
}

void initialize_timer()
{
#ifdef WIN32
    QueryPerformanceFrequency(&frequency);
    startCount.QuadPart = 0;
    QueryPerformanceCounter(&startCount);
    startTimeInMicroSec = startCount.QuadPart * (1000000.0 / frequency.QuadPart);
#else
    startCount.tv_sec = startCount.tv_usec = 0;
    gettimeofday(&startCount, NULL);
    startTimeInMicroSec = (startCount.tv_sec * 1000000.0) + startCount.tv_usec;
#endif
}

double get_elapsed_time()
{
    double endTimeInMicroSec;
#ifdef WIN32
    LARGE_INTEGER endCount;
    QueryPerformanceCounter(&endCount);

    endTimeInMicroSec = endCount.QuadPart * (1000000.0 / frequency.QuadPart);
#else
    struct timeval endCount;
    gettimeofday(&endCount, NULL);

    startTimeInMicroSec = (startCount.tv_sec * 1000000.0) + startCount.tv_usec;
    endTimeInMicroSec = (endCount.tv_sec * 1000000.0) + endCount.tv_usec;
#endif

    double elapsed = (endTimeInMicroSec - startTimeInMicroSec) * 0.000001;
    startCount = endCount;
    startTimeInMicroSec = endTimeInMicroSec;
    return elapsed;
}

void render_tetromino(char character)
{
    int i;
    tetromino_t* tet = &game_state.current_T;
    for (i = 0; i < 4; ++i)
    {
        char* coord = &layouts[tet->type - 'A'][tet->orientation][i][0];
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

void clear_lines()
{
    int x, y_from, y_to;

    int lines_cleared = 0;

    for(y_to = y_from = BOARD_H - 1; y_to >= 0; --y_to, --y_from)
    {
        int cleared = 1;
        for (x = 0; x < BOARD_W; ++x)
            if (game_state.board[y_from][x] != '#')
            {
                cleared = 0;
                break;
            }

        if (cleared)
        {
            ++lines_cleared;
            ++y_to;
            continue;
        }

        if (y_from != y_to)
            for (x = 0; x < BOARD_W; ++x)
                game_state.board[y_to][x] = y_from < 0 ? '.' : game_state.board[y_from][x];
    }

    int base_score;
    switch (lines_cleared)
    {
    case 0: base_score = 0;    break;
    case 1: base_score = 40;   break;
    case 2: base_score = 100;  break;
    case 3: base_score = 300;  break;
    case 4: base_score = 1200; break;
    default:
        printf("Cleared more than 4 lines. This should not have happened!\n");
        exit(1);
    }

    game_state.score += base_score * (get_level() + 1);

    game_state.lines_cleared += lines_cleared;
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

        clear_lines();
    }

    render_tetromino('*');
    return 0;
}

int move_down()
{
    ++game_state.score;
    return gravity_tick();
}

void move_left()
{
    int i;
    render_tetromino('.');

    tetromino_t* tet = &game_state.current_T;

    --tet->x;
    if(collision())
        ++tet->x;

    render_tetromino('*');
}

void move_right()
{
    int i;
    render_tetromino('.');

    tetromino_t* tet = &game_state.current_T;

    ++tet->x;
    if(collision())
        --tet->x;

    render_tetromino('*');
}

void rotate()
{
    int i;
    render_tetromino('.');

    tetromino_t* tet = &game_state.current_T;

    ++tet->orientation;
    tet->orientation %= 4;
    if(collision())
    {
        tet->orientation += 3;
        tet->orientation %= 4;
    }

    render_tetromino('*');
}
