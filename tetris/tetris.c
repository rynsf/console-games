#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>
#include <locale.h>
#include <unistd.h>

int maxx, maxy;

int render() {
    clear();
    printw("This is Tetris\n");
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
