#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>
#include <locale.h>
#include <unistd.h>

#define BOARDX 10
#define BOARDY 20
#define NUM_OF_TETROS 7
#define NUM_OF_ORIENTATION 4
#define MOVE_DOWN_THRESHOLD 30
#define TETRO_I 0
#define TETRO_J 1
#define TETRO_L 2
#define TETRO_O 3
#define TETRO_S 4
#define TETRO_T 5
#define TETRO_Z 6

typedef struct {
    int piece[4][2];
    int type;
    int orientation;
} tetromino;

tetromino tetro;
int maxx, maxy, spawnx, spawny;
int board[BOARDY][BOARDX];
int moveDownAcc, tetroVelocity = 5;

int tetrominos[7][4][2] = {
    {{-1, 0}, {0, 0}, {1, 0}, {2, 0}},

    {{-1, 0},
    {-1, 1}, {0, 1}, {1, 1}},

                    {{1, 0},
    {-1, 1}, {0, 1}, {1, 1}},

    {{-1, 0}, {0, 0},
    {-1, 1}, {0, 1}},

              {{0, 0}, {1, 0},
    {-1, 1}, {0, 1}},

            {{0, 0},
    {-1, 1}, {0, 1}, {1, 1}},

    {{-1, 0}, {0, 0},
             {0, 1}, {1, 1}},
};

int tetroBoundingBox[7][4][3] = {
    {{0, 1, 4},
    {1, 0, 4},
    {0, 2, 4},
    {2, 0, 4}},

    {{0, 0, 3},
    {0, 0, 3},
    {0, 1, 3},
    {1, 0, 3}}, 

    {{0, 0, 3},
    {0, 0, 3},
    {0, 1, 3},
    {1, 0, 3}}, 

    {{0, 0, 2},
    {0, 0, 2},
    {0, 0, 2},
    {0, 0, 2}},

    {{0, 0, 3},
    {0, 0, 3},
    {0, 1, 3},
    {1, 0, 3}},

    {{0, 0, 3},
    {0, 0, 3},
    {0, 1, 3},
    {1, 0, 3}},

    {{0, 0, 3},
    {0, 0, 3},
    {0, 1, 3},
    {1, 0, 3}},
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
        mvprintw(tetro.piece[n][1], tetro.piece[n][0], "#");
    }

    //int minx = BOARDX, miny = BOARDY; // debug
    //int boxx, boxy, boxw;
    //for(int n = 0; n < 4; n++) {
    //    minx = minx > tetro.piece[n][0] ? tetro.piece[n][0] : minx;
    //    miny = miny > tetro.piece[n][1] ? tetro.piece[n][1] : miny;
    //}
    //boxx = minx - tetroBoundingBox[tetro.type][tetro.orientation][0];
    //boxy = miny - tetroBoundingBox[tetro.type][tetro.orientation][1];
    //boxw = tetroBoundingBox[tetro.type][tetro.orientation][2];
    //for(int y = 0; y < boxw; y++) {
    //    for(int x = 0; x < boxw; x++) {
    //        mvprintw(boxy+y, boxx+x, "o");
    //    }
    //}
    //for(int n = 0; n < 4; n++) {
    //    mvprintw(tetro.piece[n][1], tetro.piece[n][0], "#");
    //} // end debug
    
    refresh();
    return 0;
}

int tetroSpawn() {
    int random_tetro = rand() % NUM_OF_TETROS;
    for(int n = 0; n < 4; n++) {
        tetro.piece[n][0] = spawnx + tetrominos[random_tetro][n][0];
        tetro.piece[n][1] = spawny + tetrominos[random_tetro][n][1];
    }
    tetro.type = random_tetro;
    tetro.orientation = 0;
    return 0;
}

int tetroLeftWallCollison() {
    for(int n = 0; n < 4; n++) {
        if(tetro.piece[n][0] == 0) {
            return 1;
        }
    }
    return 0;
}

int tetroLeftLockedTetroCollison() {
    for(int n = 0; n < 4; n++) {
        if(board[tetro.piece[n][1]][tetro.piece[n][0]-1] == 1) {
            return 1;
        }
    }
    return 0;
}

int tetroLeftCollison() {
    if(tetroLeftWallCollison() || tetroLeftLockedTetroCollison()) {
        return 1;
    }
    return 0;
}

int tetroRightWallCollison() {
    for(int n = 0; n < 4; n++) {
        if(tetro.piece[n][0] == BOARDX - 1) {
            return 1;
        }
    }
    return 0;
}

int tetroRightLockedTetroCollison() {
    for(int n = 0; n < 4; n++) {
        if(board[tetro.piece[n][1]][tetro.piece[n][0]+1] == 1) {
            return 1;
        }
    }
    return 0;
}

int tetroRightCollison() {
    if(tetroRightWallCollison() || tetroRightLockedTetroCollison()) {
        return 1;
    }
    return 0;
}

int tetroFloorCollison() {
    for(int n = 0; n < 4; n++) {
        if(tetro.piece[n][1] == BOARDY - 1) {
            return 1;
        }
    }
    return 0;
}

int tetroLockedTetroCollison() {
    for(int n = 0; n < 4; n++) {
        if(board[tetro.piece[n][1]+1][tetro.piece[n][0]] == 1) {
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
        tetro.piece[n][1] += 1;
    }
    return 0;
}

int tetroMoveRight() {
    if(!tetroRightCollison()) {
        for(int n = 0; n < 4; n++) {
            tetro.piece[n][0] += 1;
        }
    }
    return 0;
}

int tetroMoveLeft() {
    if(!tetroLeftCollison()) {
        for(int n = 0; n < 4; n++) {
            tetro.piece[n][0] -= 1;
        }
    }
    return 0;
}

int tetroLock() {
    for(int n = 0; n < 4; n++) {
        board[tetro.piece[n][1]][tetro.piece[n][0]] = 1;
    }
    return 0;
}

int tetroRotate() {
    int minx = BOARDX, miny = BOARDY;
    int boxx, boxy, boxw;
    for(int n = 0; n < 4; n++) {
        minx = minx > tetro.piece[n][0] ? tetro.piece[n][0] : minx;
        miny = miny > tetro.piece[n][1] ? tetro.piece[n][1] : miny;
    }

    boxx = minx - tetroBoundingBox[tetro.type][tetro.orientation][0];
    boxy = miny - tetroBoundingBox[tetro.type][tetro.orientation][1];
    boxw = tetroBoundingBox[tetro.type][tetro.orientation][2];

    int boundingBox[boxw][boxw];
    int boundingBoxRotated[boxw][boxw];
    int tetroTemp[4][2];

    for(int n = 0; n < boxw; n++) {
        for(int m = 0; m < boxw; m++) {
            boundingBox[n][m] = 0;
            boundingBoxRotated[n][m] = 0;
        }
    }

    for(int n = 0; n < 4; n++) {
        tetroTemp[n][0] = tetro.piece[n][0] - boxx;
        tetroTemp[n][1] = tetro.piece[n][1] - boxy;
    }

    for(int n = 0; n < 4; n++) {
        boundingBox[tetroTemp[n][1]][tetroTemp[n][0]] = 1;
    }

    for(int n = 0; n < boxw; n++) {
        for(int m = 0; m < boxw; m++) {
            boundingBoxRotated[n][m] = boundingBox[m][boxw-1-n];
        }
    }

    int num = 0;
    for(int n = 0; n < boxw; n++) {
        for(int m = 0; m < boxw; m++) {
            if(boundingBoxRotated[n][m] == 1) {
                tetroTemp[num][0] = m;
                tetroTemp[num][1] = n;
                num += 1;
            }
        }
    }

    for(int n = 0; n < 4; n++) {
        tetroTemp[n][0] = boxx + tetroTemp[n][0];
        tetroTemp[n][1] = boxy + tetroTemp[n][1];
    }

    for(int n = 0; n < 4; n++) {
        if(tetroTemp[n][0] < 0 ||
            tetroTemp[n][0] >= BOARDX ||
            tetroTemp[n][1] < 0 ||
            tetroTemp[n][1] >= BOARDY ||
            board[tetroTemp[n][1]][tetroTemp[n][0]] == 1) {
            return 0;
        }
    }

    for(int n = 0; n < 4; n++) {
        tetro.piece[n][0] = tetroTemp[n][0];
        tetro.piece[n][1] = tetroTemp[n][1];
    }
    tetro.orientation = (tetro.orientation + 1) % NUM_OF_ORIENTATION;

    return 0;
}

int boardShiftDown(int line) {
    for(int y = line; y >= 0; y--) {
        for(int x = 0; x < BOARDX; x++) {
            board[y][x] = board[y-1][x];
        }
    }
    for(int x = 0; x < BOARDX; x++) {
        board[0][x] = 0;
    }
    return 0;
}

int tetroLineClear() {
    int isFilled = 1, numLinesFilled = 0;
    int lineFilled[4] = {0};
    for(int y = 0; y < BOARDY; y++) {
        isFilled = 1;
        for(int x = 0; x < BOARDX; x++) {
            if(board[y][x] == 0) {
                isFilled = 0;
                break;
            }
        }
        if(isFilled) {
            lineFilled[numLinesFilled++] = y;
        }
    }

    //clean the lines
    // TODO tetris
    for(int n = 0; n < numLinesFilled; n++) {
        boardShiftDown(lineFilled[n]);
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
    tetroLineClear();
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

                case 'f':
                    tetroRotate();
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
