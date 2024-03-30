#include "fender.h"

const char* pixels[] = {" ", "▀", "▄", "█"};

const glyf Font[] = {
    {
        'A',
        {0, 0, 4, 5},
        {
            {0,1,1,0,0,0},
            {1,0,0,1,0,0},
            {1,0,0,1,0,0},
            {1,1,1,1,0,0},
            {1,0,0,1,0,0},
            {0,0,0,0,0,0},
        }
    },

    {
        'B',
        {0, 0, 4, 5},
        {
            {1,1,1,0,0,0},
            {1,0,0,1,0,0},
            {1,1,1,0,0,0},
            {1,0,0,1,0,0},
            {1,1,1,0,0,0},
            {0,0,0,0,0,0},
        }
    }, 
};

glyf cMap(char chr) {
    for(int n = 0; n < NUM_GLYF; n++) {
        if(chr == Font[n].chr) {
            return Font[n];
        }
    }
    return Font[0]; // may be have a wild card glyph
}

int getBitFromMap(int y, int x, glyf g) { 
    if(y < g.box[1] + g.box[3] &&
        x < g.box[0] + g.box[2]) {
        return g.bitmap[y][x];
    }
    return 0;
}

int renderFont(int posx, int posy, char* str) {
    for(int c = 0; str[c] != '\0'; c++) { // classic NULL terminated string...
        glyf g = cMap(str[c]);
        for(int y = g.box[1]; y < g.box[1]+g.box[3]; y += 2) {
            for(int x = g.box[0]; x < g.box[0]+g.box[2]; x++ ) {
                int pixel = getBitFromMap(y, x, g)
                            + getBitFromMap(y+1, x, g) * 2;  
                mvprintw(posy+y/2, posx+x, "%s", pixels[pixel]);
            }
        }
        posx += g.box[2] + 1;
    }
    return 0;
}
