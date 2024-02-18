#include <stdio.h>
#include <ncurses.h>
#include <locale.h>
#include <unistd.h>

#define LASER_VELOCITY 1
#define ACC_THRESH 30
#define NUM_ALIEN_X 10
#define NUM_ALIEN_Y 5

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
int pauseval = 2; // bad variable name?

character laser;
character alien[NUM_ALIEN_Y][NUM_ALIEN_X];

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
        return 0;
    }
    for(int y = 0; y < NUM_ALIEN_Y; ++y) {
        for(int x = 0; x < NUM_ALIEN_X; ++x) {
            if(laser.x >= alien[y][x].x &&
                laser.y >= alien[y][x].y &&
                laser.x <= alien[y][x].x + alien[y][x].w &&
                laser.y <= alien[y][x].y + alien[y][x].h) {
                alien[y][x].x = -1;
                laserDestroy();
                return 0;
            }
        }
    }
    return 0;
}

int alienDead(character a) {
    return a.x == -1;
}

int alienMoveDown() {
    for(int y = 0; y < NUM_ALIEN_Y; ++y) {
        for(int x = 0; x < NUM_ALIEN_X; ++x) {
            alien[y][x].y += 2; // deal with magic nums TODO
        }
    }
    return 0;
}

char* glyphs[7][6] = {
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
    },
    {
        &"     ▄▄▄▄▄▄\n",
        &"▄██████████████▄\n",
        &"████▄▄████▄▄████\n",
        &"   ▄▀▀▀▄▄▀▀▀▄\n",
        &"    ▀      ▀\n",
        NULL,
    },
    {
        &"     ▄▄▄▄▄▄\n",
        &"▄██████████████▄\n",
        &"████▄▄████▄▄████\n",
        &"  ▄▄▀▀▀▄▄▀▀▀▄▄\n",
        &"▀▀            ▀▀\n",
        NULL,
    },
    {
        &"   ▄▄▄▄\n",
        &" ▄██████▄\n",
        &"███▄██▄███\n",
        &"  ▄▀▄▄▀▄\n",
        &" ▀ ▀  ▀ ▀\n",
        NULL,
    },
    {
        &"   ▄▄▄▄\n",
        &" ▄██████▄\n",
        &"███▄██▄███\n",
        &"  ▄▀  ▀▄\n",
        &"   ▀  ▀\n",
        NULL,
    }
};

int maxx, maxy;

int update() {
    int moveDown = false;
    laserCollison();
    //check collison with aliens

    //move aliens.
    accumulator += pauseval;
    if(accumulator >= ACC_THRESH) {
        for(int y = 0; y < NUM_ALIEN_Y; ++y) {
            for(int x = 0; x < NUM_ALIEN_X; ++x) {
                if((alien[y][x].x >= maxx - alien[y][x].w - 2 
                    && alienvelx == 2)
                    || (alien[y][x].x <= 2
                    && alienvelx == -2)
                    && !alienDead(alien[y][x])) {
                    moveDown = true;
                }
            }
        }

        if(moveDown) {
            alienvelx = alienvelx == 2 ? -2 : 2;
            alienMoveDown();
        } else {
            for(int y = 0; y < NUM_ALIEN_Y; ++y) {
                for(int x = 0; x < NUM_ALIEN_X; ++x) {
                    if(!alienDead(alien[y][x])) {
                        alien[y][x].x += alienvelx;
                    }
                }
            }
        }

        for(int y = 0; y < NUM_ALIEN_Y; ++y) {
            for(int x = 0; x < NUM_ALIEN_X; ++x) {
                alien[y][x].glyph += alien[y][x].glyph % 2 ? 1 : -1;
            }
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
    for(int y = 0; y < NUM_ALIEN_Y; ++y) {
        for(int x = 0; x < NUM_ALIEN_X; ++x) {
            if(!alienDead(alien[y][x])) {
                renderglyph(alien[y][x]);
            }
        }
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
    int lineGlyph[5] = {5, 1, 1, 3, 3};
    craft.x = maxx / 2;
    craft.y = maxy - 4;
    laser.x = -1; 
    //create alien
    for(int y = 0; y < NUM_ALIEN_Y; ++y) {
        for(int x = 0; x < NUM_ALIEN_X; ++x) {
            if(lineGlyph[y] == 5) {
                alien[y][x].x = 2 + 19 * x + 3;
                alien[y][x].y = 2 + 6 * y;
                alien[y][x].w = 10;
                alien[y][x].h = 5;
            } else {
                alien[y][x].x = 2 + 19 * x;
                alien[y][x].y = 2 + 6 * y;
                alien[y][x].w = 16;
                alien[y][x].h = 5;
            }
            alien[y][x].glyph = lineGlyph[y];
        }
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
    init();
    char input;

    if(maxx < 250 || maxy < 50) {
        endwin();
        printf("Terminal too small :(\n");
        return 0;
    }

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
