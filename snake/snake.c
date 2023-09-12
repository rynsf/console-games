#include <stdio.h>
#include <curses.h>
#include <locale.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

typedef struct Snake{
    int y;
    int x;
    struct Snake *next;
} Snake;

Snake *snakeHead;
Snake *snakeTail;

typedef struct{
    int y;
    int x;
} Vect;

Vect vel;
Vect food;
int score;

int maxy, maxx;

int move_snake()
{
    char inpt = getch();
    if(inpt)
    {
        switch(inpt)
        {
            case 'w':
                vel.x = 0;
                vel.y = vel.y == 1 ? vel.y : -1;
                break;

            case 's':
                vel.x = 0;
                vel.y = vel.y == -1 ? vel.y : 1;
                break;

            case 'a':
                vel.x = vel.x == 2 ? vel.x : -2;
                vel.y = 0;
                break;

            case 'd':
                vel.x = vel.x == -2 ? vel.x : 2;
                vel.y = 0;
                break;
        }
    }
    return 0;
}

int checkEaten()
{
    if((food.x == snakeHead -> x && food.y == snakeHead -> y) || (food.x == snakeHead -> x+1 && food.y == snakeHead -> y))
    {
        return 1;
    }
    return 0;
}

int putFood()
{
    int num = rand() % (maxy - 1);
    food.y = num % 2 == 0 ? num : num + 1 ;
    num  = rand() % (maxx - 1);
    food.x = num % 2 == 0 ? num : num + 1 ;
    return 0;
}

int incSnake(int y, int x)
{
    Snake *newSnake = malloc(sizeof(Snake));
    snakeTail -> next = newSnake;
    newSnake -> y = y;
    newSnake -> x = x;
    newSnake -> next = NULL;
    snakeTail = newSnake;
    return 0;
}

int update_snake()
{
    Snake *nextSnake = snakeHead -> next;
    int lastx = snakeHead -> x;
    int lasty = snakeHead -> y;
    int currentx, currenty;
    while(nextSnake != NULL)
    {
        currenty = nextSnake -> y;
        currentx = nextSnake -> x;
        nextSnake -> y = lasty;
        nextSnake -> x = lastx;
        lastx = currentx;
        lasty = currenty;
        nextSnake = nextSnake -> next;
    }
    snakeHead -> x += vel.x;
    snakeHead -> y += vel.y;
    if(checkEaten())
    {
        putFood();
        score += 1;
        incSnake(0, 0);
    }
    return 0;
}

int lost()
{
    clear();
    timeout(-1);
    printw("You Lost. Your score was %d, \nPress space to continue...\n", score);
    while(getch() != ' ');
    return 0;
}

int islost()
{
    if(snakeHead -> x < 0 || snakeHead -> x > maxx-1 || snakeHead -> y < 0 || snakeHead -> y > maxy-1)
    {
        lost();
        return 1;
    }
    return 0;
}

int show_snake()
{
    clear();
    Snake *nextSnake =  snakeHead;
    while(nextSnake != NULL)
    {
        mvprintw(nextSnake -> y, nextSnake -> x, "██");
        nextSnake = nextSnake -> next;
    }
    mvprintw(food.y, food.x, "██");
    refresh();
    return 0;

}

int freeMem()
{
    Snake *head = snakeHead;
    Snake *temp;
    while(head != NULL)
    {
        temp = head;
        head = head -> next;
        free(temp);
    }
    return 0;
}

int main()
{
    srand(time(NULL));
    setlocale(LC_ALL, "");
    WINDOW *win = initscr(); cbreak(); noecho(); curs_set(0); timeout(0);
    getmaxyx(win, maxy, maxx);
    vel.x = 2;
    vel.y = 0;
    snakeHead = malloc(sizeof(Snake));
    snakeHead -> x = 10;
    snakeHead -> y = 10;
    snakeHead -> next = NULL;
    snakeTail = snakeHead;

    incSnake(10, 8);
    incSnake(10, 6);
    incSnake(10, 4);
    incSnake(10, 2);

    putFood();
    while(!islost())
    {
        show_snake();
        move_snake();
        update_snake();
        usleep(100000);
    }
    freeMem();
    endwin();
    return 0;
}
