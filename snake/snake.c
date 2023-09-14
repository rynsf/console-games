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

Snake *snakeHead, *snakeTail;

typedef struct{
    int y;
    int x;
} Vect;

Vect vel, food;

int score, maxy, maxx, ENDLESS;

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
    if((food.x == snakeHead -> x && food.y == snakeHead -> y) ||
       (food.x == snakeHead -> x+1 && food.y == snakeHead -> y))
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
    if(ENDLESS)
    {
        snakeHead -> x = snakeHead -> x < 0 ? maxx - 2 : snakeHead -> x;
        snakeHead -> x = snakeHead -> x > maxx-1 ? 0 : snakeHead -> x;
        snakeHead -> y = snakeHead -> y < 0 ? maxy - 1 : snakeHead -> y;
        snakeHead -> y = snakeHead -> y > maxy-1 ? 0 : snakeHead -> y;
    }
    if(checkEaten())
    {
        putFood();
        score += 1;
        incSnake(snakeTail -> y, snakeTail -> x);
    }
    return 0;
}

int clean_snake(int wait)
{
    Snake *nextSnake =  snakeHead;
    while(nextSnake != NULL)
    {
        mvprintw(nextSnake -> y, nextSnake -> x, "  ");
        nextSnake = nextSnake -> next;
        if(wait)
        {
            refresh();
            usleep(wait);
        }
    }
    refresh();
    return 0;
}

int show_snake()
{
    clear();
    attron(COLOR_PAIR(1));
    Snake *nextSnake =  snakeHead;
    while(nextSnake != NULL)
    {
        mvprintw(nextSnake -> y, nextSnake -> x, "██");
        nextSnake = nextSnake -> next;
    }
    attroff(COLOR_PAIR(1));
    attron(COLOR_PAIR(2));
    mvprintw(food.y, food.x, "██");
    attroff(COLOR_PAIR(2));
    refresh();
    return 0;

}

int lost()
{
    for(int n = 0; n < 3; n++)
    {
        clear();
        refresh();
        usleep(500000);
        show_snake();
        usleep(500000);
    }
    clear();
    timeout(-1);
    printw("You Lost. Your score was %d, \nPress space to EXIT...\n", score);
    while(getch() != ' ');
    return 0;
}

int islost()
{

    if(!ENDLESS && (snakeHead -> x < 0 || snakeHead -> x > maxx-1 || snakeHead -> y < 0 || snakeHead -> y > maxy-1))
    {
        lost();
        return 1;
    }
    Snake *snakeBody = snakeHead -> next;
    while(snakeBody != NULL)
    {
        if(snakeBody -> x == snakeHead -> x && snakeBody -> y == snakeHead -> y)
        {
            lost();
            return 1;
        }
        snakeBody = snakeBody -> next;
    }
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
    WINDOW *win = initscr(); start_color(); cbreak(); noecho(); curs_set(0); timeout(0);
    init_pair(1, COLOR_GREEN, COLOR_GREEN);
    init_pair(2, COLOR_RED, COLOR_RED);
    getmaxyx(win, maxy, maxx);
    maxx = maxx % 2 == 0 ? maxx : maxx - 1;
    vel.x = 2;
    vel.y = 0;
    snakeHead = malloc(sizeof(Snake));
    snakeHead -> x = 22;
    snakeHead -> y = 6;
    snakeHead -> next = NULL;
    snakeTail = snakeHead;
    ENDLESS = 1;
    for(int i = 20; i > 2; i -=2)
        incSnake(6, i);
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
