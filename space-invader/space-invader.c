#include <stdio.h>
#include <ncurses.h>
#include <locale.h>
#include <unistd.h>

#define LASER_VELOCITY 1
#define ACC_THRESH 30

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

int alienvelx = 2;
int accumulator = 0;
int pauseval = 10; // bad variable name

character laser;
character alien[10][10]; // what is the size here?

int craftMoveRight() {
    craft.x += 1;
    return 0;
}

int craftMoveLeft() {
    craft.x -= 1;
    return 0;
}

int noLaser() {
    return laser.x == -1;
}

int laserShoot() {
    if(noLaser()) {
        laser.x = craft.x + 7;
        laser.y = craft.y + 1;
    }
    return 0;
}

int laserDestroy() {
    laser.x = -1;
    return 0;
}

int laserCollison() {
    if (laser.y < 0) {
        laserDestroy();
    }
    return 0;
}

int alienDead(character a) {
    return a.x == -1;
}

int alienMoveDown() {
    alien[0][0].y += 2; // deal with magic nums TODO
}

char* glyphs[3][6] = {
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
    },
    {
        &"   ▀▄      ▄▀\n",        
        &"  ▄██▀████▀██▄\n",
        &"████████████████\n",
        &"█  █▀▀▀▀▀▀▀▀█  █\n",
        &"    ▀▀    ▀▀\n",
        NULL,
    }
};

int maxx, maxy;

int update() {
    laserCollison();
    //check collison with aliens

    //move aliens.
    accumulator += pauseval;
    if(accumulator >= ACC_THRESH) {
        if(!alienDead(alien[0][0])) {
            if(alien[0][0].x >= maxx - alien[0][0].w - 2 && alienvelx == 2) {
                alienvelx = -2;
                alienMoveDown();
            } else if(alien[0][0].x <= 2 && alienvelx == -2) {
                alienvelx = 2;
                alienMoveDown();
            } else {
                alien[0][0].x += alienvelx;
            }
            alien[0][0].glyph += alien[0][0].glyph % 2 ? 1 : -1;
        }
        accumulator = 0;
    }
        
    if(!noLaser()) {
        laser.y -= LASER_VELOCITY;
    }
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
    if(!alienDead(alien[0][0])) {
        renderglyph(alien[0][0]);
    }
    if(!noLaser()) {
        mvprintw(laser.y, laser.x, "█");
    }
    return 0;
}

int islost() {
    return 0;
}

int init() {
    craft.x = maxx / 2;
    craft.y = maxy - 4;
    laser.x = -1; 
    //create alien
    alien[0][0].x = 80;
    alien[0][0].y = 2;
    alien[0][0].w = 16;
    alien[0][0].h = 5;
    alien[0][0].glyph = 1;
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
                case 'r':
                    craftMoveLeft();
                    break;

                case 'd':
                case 'l':
                case 't':
                    craftMoveRight();
                    break;

                case ' ':
                    laserShoot();
                
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
