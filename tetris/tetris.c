#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>
#include <locale.h>
#include <unistd.h>

#define BOARDX 10
#define BOARDY 20
#define NUM_OF_TETROS 7

int tetrominos[7][4][2] = {
    {{-1, 0}, {0, 0}, {1, 0}, {2, 0}},

    {{-1, 0}, {0, 0},
    {-1, 1}, {0, 1}},

    {{-1, 0}, {0, 0}, {1, 0},
              {0, 1}},

    {{-1, 0}, {0, 0}, {1, 0},
    {-1, 1}},
              
    {{-1, 0}, {0, 0}, {1, 0},
                     {1, 1}},

              {{0, 0}, {1, 0},
    {-1, 1}, {0, 1}},


    {{-1, 0}, {0, 0},
             {0, 1}, {1, 1}},
};

int tetro[4][2];

int maxx, maxy, spawnx, spawny;

int board[BOARDY][BOARDX];

int render() {
    clear();
    for(int y = 0; y < BOARDY; y++) {
        for(int x = 0; x < BOARDX; x++) {
            if(board[y][x] == 1) {
                mvprintw(y, x, "#");
            } else {
                mvprintw(y, x, ".");
            }
        }
    }
    for(int n = 0; n < 4; n++) {
        mvprintw(tetro[n][1], tetro[n][0], "#");
    }
    refresh();
    return 0;
}

int spawn_tetro() {
    int random_tetro = rand() % NUM_OF_TETROS;
    for(int n = 0; n < 4; n++) {
        tetro[n][0] = spawnx + tetrominos[random_tetro][n][0];
        tetro[n][1] = spawny + tetrominos[random_tetro][n][1];
    }
    return 0;
}

int update() {
    return 0;
}

int main() {
    setlocale(LC_ALL, "");
    WINDOW *win = initscr();
    cbreak();
    noecho();
    curs_set(0);
    timeout(0);
    getmaxyx(win, maxy, maxx);
    spawnx = BOARDX / 2;
    spawny = 0;
    spawn_tetro();
    int quit = 0;
    char input;

    while(!quit) {
        while((input = getch()) != -1) {
            switch(input) {
                case 'q':
                    quit = 1;
                    break;

                default: {};
            }
        }
        update();
        render();
        usleep(33333);
    }
    endwin();
    return 0;
} 
