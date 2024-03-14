#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>
#include <locale.h>
#include <unistd.h>

#define BOARDX 10
#define BOARDY 20
#define NUM_OF_TETROS 7
#define MOVE_DOWN_THRESHOLD 30

int tetro[4][2];
int maxx, maxy, spawnx, spawny;
int board[BOARDY][BOARDX];
int moveDownAcc, tetroVelocity = 5;

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

int tetroSpawn() {
    int random_tetro = rand() % NUM_OF_TETROS;
    for(int n = 0; n < 4; n++) {
        tetro[n][0] = spawnx + tetrominos[random_tetro][n][0];
        tetro[n][1] = spawny + tetrominos[random_tetro][n][1];
    }
    return 0;
}

int tetroLeftWallCollison() {
    for(int n = 0; n < 4; n++) {
        if(tetro[n][0] == 0) {
            return 1;
        }
    }
    return 0;
}

int tetroRightWallCollison() {
    for(int n = 0; n < 4; n++) {
        if(tetro[n][0] == BOARDX - 1) {
            return 1;
        }
    }
    return 0;
}

int tetroFloorCollison() {
    for(int n = 0; n < 4; n++) {
        if(tetro[n][1] == BOARDY - 1) {
            return 1;
        }
    }
    return 0;
}

int tetroLockedTetroCollison() {
    for(int n = 0; n < 4; n++) {
        if(board[tetro[n][1]+1][tetro[n][0]] == 1) {
            return 1;
        }
    }
    return 0;
}

int tetroCollison() {
    if(tetroFloorCollison() || tetroLockedTetroCollison()) {
        return 1;
    }
    return 0;
}

int tetroMoveDown() {
    for(int n = 0; n < 4; n++) {
        tetro[n][1] += 1;
    }
    return 0;
}

int tetroMoveRight() {
    if(!tetroRightWallCollison()) {
        for(int n = 0; n < 4; n++) {
            tetro[n][0] += 1;
        }
    }
    return 0;
}

int tetroMoveLeft() {
    if(!tetroLeftWallCollison()) {
        for(int n = 0; n < 4; n++) {
            tetro[n][0] -= 1;
        }
    }
    return 0;
}

int tetroLock() {
    for(int n = 0; n < 4; n++) {
        board[tetro[n][1]][tetro[n][0]] = 1;
    }
    return 0;
}

int update() {
    moveDownAcc += tetroVelocity;
    if(moveDownAcc >= MOVE_DOWN_THRESHOLD && tetroCollison()) {
        tetroLock();
        tetroSpawn();
        moveDownAcc = 0;
    }
    if(moveDownAcc >= MOVE_DOWN_THRESHOLD && !tetroCollison()) {
        tetroMoveDown();
        moveDownAcc = 0;
    }
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
    tetroSpawn();
    int quit = 0;
    char input;

    while(!quit) {
        while((input = getch()) != -1) {
            switch(input) {
                case 'q':
                    quit = 1;
                    break;

                case 't':
                    tetroMoveRight();
                    break;

                case 'r':
                    tetroMoveLeft();
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
