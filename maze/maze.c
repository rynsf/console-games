#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <locale.h>
#include <time.h>

const int maze_size_y = 10;
const int maze_size_x = 20;
const int arr_size_y = maze_size_y*2+3;
const int arr_size_x = maze_size_x*2+3;
const int Stack_size = arr_size_y*arr_size_x*2;

int which_brick(int grid[arr_size_y][arr_size_x], char bricks[19][4], int posy, int posx)
{
    if(grid[posy][posx] == 0)
        return 0;
    return (8*grid[posy-1][posx]) + (4*grid[posy][posx+1]) + (2*grid[posy+1][posx]) + (grid[posy][posx-1]);
}

int finalize_maze(int grid[arr_size_y][arr_size_x], char bricks[19][4], int arr_size_y, int arr_size_x)
{
    int grid2[arr_size_y][arr_size_x];
    for(int y = 0; y<arr_size_y; y++)
        for(int x = 0; x<arr_size_x; x++)
            grid2[y][x] = grid[y][x];

    for(int y = 1; y < arr_size_y-1; y++)
        for(int x = 1; x < arr_size_x-1; x++)
            grid[y][x] = which_brick(grid2, bricks, y, x);

    return 0;
}

int encode_one(int currenty, int currentx, int arr_size_x)
{
    return (currenty*arr_size_x)+currentx;
}

int decode_pair(int* currenty, int* currentx, int arr_size_x, int value)
{
    *currenty = value / arr_size_x;
    *currentx = value % arr_size_x;
    return 0;
}

int pop(int Stack[Stack_size], int* top)
{
    int poped = Stack[*top];
    *top -= 1;
    return poped;
}

int push(int Stack[Stack_size], int* top, int value)
{
    *top += 1;
    Stack[*top] = value;
    return 0;
}

int check_neighbour(int grid[arr_size_y][arr_size_x], int currenty, int currentx, int arr_size_x)
{
    int neighbour = 0;
    int arr_neighbour[4];
    if(grid[currenty - 2][currentx] == 3)
        arr_neighbour[neighbour++] = encode_one(currenty - 2, currentx, arr_size_x);
    if(grid[currenty][currentx + 2] == 3)
        arr_neighbour[neighbour++] = encode_one(currenty, currentx + 2, arr_size_x);
    if(grid[currenty + 2][currentx] == 3)
        arr_neighbour[neighbour++] = encode_one(currenty + 2, currentx, arr_size_x);
    if(grid[currenty][currentx-2] == 3)
        arr_neighbour[neighbour++] = encode_one(currenty, currentx - 2, arr_size_x);

    if(neighbour > 0)
    {
        return arr_neighbour[rand()%neighbour];
    }
    return 0;
}

int recursive_backtrack(int grid[arr_size_y][arr_size_x], int arr_size_y, int arr_size_x)
{
    int Stack[Stack_size];
    int top = -1;
    int currentx = 4;
    int currenty = 4;
    int nextx = 0;
    int nexty = 0;
    int neighbour = 0;
    grid[currenty][currentx] = 0;
    push(Stack, &top, encode_one(currenty, currentx, arr_size_x));
    while(top >= 0)
    {
        decode_pair(&currenty, &currentx, arr_size_x, pop(Stack, &top));
        if((neighbour = check_neighbour(grid, currenty, currentx, arr_size_x)))
        {
            push(Stack, &top, encode_one(currenty, currentx, arr_size_x));//pused to stack
            decode_pair(&nexty, &nextx, arr_size_x, neighbour);//neighbour chooosed
            if(currenty == nexty)// Wall removed
                grid[currenty][currentx + ((nextx - currentx)/2)] = 4;
            if(currentx == nextx)
                grid[currenty + ((nexty - currenty)/2)][currentx] = 4;
            grid[nexty][nextx] = 0;//marked visited
            push(Stack, &top, encode_one(nexty, nextx, arr_size_x));//pushed to stack
        }
    }

    for(int y = 0; y < arr_size_y; y++)
        for(int x = 0; x < arr_size_x; x++)
            if(grid[y][x] != 1)
                grid[y][x] = 0;
    return 0;
}

int won()
{
    clear();
    printw("YOU WON!!!\n");
    printw("Press any key to quit...\n");
    getch();
    endwin();
    exit(0);
}

int Move(int grid[arr_size_y][arr_size_x], int posy, int posx, int posy1, int posx1)
{
    if(grid[posy][posx] == 0)
    {
        grid[posy1][posx1] = 17;
        grid[posy][posx] = 16;
        mvprintw(posy1-1, posx1-1, ".");
        mvprintw(posy-1, posx-1, "*");
        return 0;
    }

    if(grid[posy][posx] == 17)
    {
        grid[posy1][posx1] = 0;
        grid[posy][posx] = 16;
        mvprintw(posy1-1, posx1-1, " ");
        mvprintw(posy-1, posx-1, "*");
        return 0;
    }

    if(grid[posy][posx] == 18)
    {
        won();
        return 0;
    }
    return 1;
}

int Input(int grid[arr_size_y][arr_size_x], int* posy, int* posx)
{
    char inpt;
    re_inpt:
    inpt = getch();

    switch(inpt) {
        case 'w':
            if(!Move(grid, *posy-1, *posx, *posy, *posx))
                *posy -= 1;
            break;
        case 'a':
            if(!Move(grid, *posy, *posx-1, *posy, *posx))
                *posx -= 1;
            break;
        case 's':
            if(!Move(grid, *posy+1, *posx, *posy, *posx))
                *posy += 1;
            break;
        case 'd':
            if(!Move(grid, *posy, *posx+1, *posy, *posx))
                *posx += 1;
            break;
        default:
            goto re_inpt;
    }
    return 0;
}

int print_maze(char bricks[19][4], int grid[arr_size_y][arr_size_x])
{
    clear();
    for(int y = 1; y<arr_size_y-1; y++)
    {
        for(int x = 1; x<arr_size_x-1; x++)
            printw("%s", bricks[grid[y][x]]);
        printw("\n");
    }
    return 0;
}

int main()
{
    setlocale(LC_ALL, "");
    initscr(); cbreak(); noecho(); curs_set(0);
    srand(time(NULL));
    int grid[arr_size_y][arr_size_x];
    char bricks[19][4] = {" ", "─", "│", "┐", "─", "─", "┌", "┬", "│", "┘", "│", "┤", "└", "┴", "├", "┼", "*", ".", "o"};
    for(int y = 0; y < arr_size_y; y++)
    {
        for(int x = 0; x < arr_size_x; x++)
        {
            if(y == 0 || x == 0 || y == arr_size_y-1 || x == arr_size_x - 1 )
                grid[y][x] = 2;
            else if( y % 2 == 0 && x % 2 == 0)
                grid[y][x] = 3;
            else
                grid[y][x] = 1;
        }
    }
    int posx = 2;
    int posy = 2;
    recursive_backtrack(grid, arr_size_y, arr_size_x);
    finalize_maze(grid, bricks, arr_size_y, arr_size_x);
    grid[arr_size_y-3][arr_size_x-3] = 18;
    grid[posy][posx] = 16;
    print_maze(bricks, grid);
    while(1)
    {
        Input(grid, &posy, &posx);
    }
    endwin();
    return 0;
}
