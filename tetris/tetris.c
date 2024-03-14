#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>
#include <locale.h>
#include <unistd.h>

#define BOARDX 10
#define BOARDY 20

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

int tetro[4];

int maxx, maxy;

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
    refresh();
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
