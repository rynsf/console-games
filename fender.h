#include <ncurses.h>

#define NUM_GLYF 2

typedef struct {
    char chr;
    int box[4];
    int bitmap[6][6];
} glyf;

int renderFont(int, int, char*);
