#include <stdio.h>
#include <ncurses.h>
#include <locale.h>
#include <unistd.h>

typedef struct {
    int x, y, w, h;
    int glyph;
} character;

character craft = {
    .x = 4,
    .y = 3,
    .w = 15,
    .h = 4,
    .glyph = 0
};

int craftMoveRight() {
    craft.x += 1;
    return 0;
}

int craftMoveLeft() {
    craft.x -= 1;
    return 0;
}

character laser;
character alien[10][10]; // what is the size here?

char* glyphs[2][6] = {
    {
        &"       ▄\n",
        &"      ███\n",
        &"▄█████████████▄\n",
        &"███████████████\n",
        NULL,
    },

    {
        &"   ▀▄      ▄▀\n",
        &"█  ██▀████▀██  █\n",
        &"████████████████\n",
        &" ▀█▀▀▀▀▀▀▀▀▀▀█▀\n",
        &" ▀            ▀\n",
        NULL,
    }
};

int maxx, maxy;

int update() {
    return 0;
}

int renderglyph(character glyph) {
    for(int n = 0; glyphs[glyph.glyph][n] != NULL; ++n) {
        mvprintw(glyph.y+n, glyph.x, "%s", glyphs[glyph.glyph][n]);
    }
    return 0;
}
    
int render() {
    clear();
    renderglyph(craft);
    return 0;
}

int islost() {
    return 0;
}

int init() {
    craft.x = maxx / 2;
    craft.y = maxy - 4;
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
    init();
    char input;

    while(!islost()) {
        input = getch();
        while(input != -1) {
            // handle input here
            switch(input) {
                case 'a':
                case 'h':
                    craftMoveLeft();
                    break;

                case 'd':
                case 'l':
                    craftMoveRight();
                    break;
                
                default: {} 
            }
            input = getch();
        }
        update();
        render();
        usleep(33333);
    }
    endwin();
    return 0;
}
