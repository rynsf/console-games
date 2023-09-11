#include <stdio.h>
#include <stdlib.h>
#include <time.h>

const int maze_size_y = 20;
const int maze_size_x = 40;
const int arr_size_y = maze_size_y*2+3;
const int arr_size_x = maze_size_x*2+3;
const int Stack_size = arr_size_y*arr_size_x*2;

char* which_brick(int grid[arr_size_y][arr_size_x], char bricks[16][4], int posy, int posx)
{
    if(grid[posy][posx] == 0)
        return " ";
    return bricks[(8*grid[posy-1][posx]) + (4*grid[posy][posx+1]) + (2*grid[posy+1][posx]) + (grid[posy][posx-1])];
}

int show_maze(int grid[arr_size_y][arr_size_x], char bricks[16][4], int arr_size_y, int arr_size_x)
{
    for(int y = 1; y < arr_size_y-1; y++)
    {
        for(int x = 1; x < arr_size_x-1; x++)
        {
            printf("%s", which_brick(grid, bricks, y, x));
        }
        printf("\n");
    }

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
            decode_pair(&nexty, &nextx, arr_size_x, neighbour);//neighbour choosed
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

int main()
{
    srand(time(NULL));
    int grid[arr_size_y][arr_size_x];
    char bricks[16][4] = {" ", "─", "│", "┐", "─", "─", "┌", "┬", "│", "┘", "│", "┤", "└", "┴", "├", "┼"};
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

    recursive_backtrack(grid, arr_size_y, arr_size_x);
    show_maze(grid, bricks, arr_size_y, arr_size_x);
    return 0;
}
