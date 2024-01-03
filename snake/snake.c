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

Snake *snakeHead, *snakeTail, *snakeLast;

typedef struct{
    int y;
    int x;
} Vect;

Vect vel, food;

int score, maxy, maxx, minx, miny, ENDLESS;

int to_odd(int x, int round_to) 
{
    return x % 2 == 0 ? x + round_to : x;
}

int to_even(int x, int round_to) 
{
    return x % 2 == 0 ? x : x + round_to;
}

int move_snake()
{
    char inpt = getch();
    if(inpt)
    {
        if(inpt == '\033') // handling escape code for arrow keys
        {
            getch();
            inpt = getch();
        }
        switch(inpt)
        {
            case 'w':
            case 'k':
            case 'A':
                vel.x = 0;
                vel.y = vel.y == 1 ? vel.y : -1;
                break;

            case 's':
            case 'j':
            case 'B':
                vel.x = 0;
                vel.y = vel.y == -1 ? vel.y : 1;
                break;

            case 'a':
            case 'h':
            case 'D':
                vel.x = vel.x == 2 ? vel.x : -2;
                vel.y = 0;
                break;

            case 'd':
            case 'l':
            case 'C':
                vel.x = vel.x == -2 ? vel.x : 2;
                vel.y = 0;
                break;
        }
    }
    return 0;
}

int showScore()
{
    mvprintw(1, maxx - 7,"%8d", score);
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

int is_food_on_snake(Vect food) 
{
    Snake *nextSnake = snakeHead;
    while(nextSnake != NULL)
    {
        if(nextSnake -> y == food.y && nextSnake -> x == food.x)
        {
            return 1;
        }
        nextSnake = nextSnake -> next;
    }
    return 0;
}

int putFood()
{
    do 
    {
        int num = (rand() % (maxy - 4)) + 3;
        food.y = num;
        num  = (rand() % (maxx - 4)) + 1;
        food.x = to_odd(num, 1);
    } while (is_food_on_snake(food));
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
    snakeLast -> y = snakeTail -> y;
    snakeLast -> x = snakeTail -> x;
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
        snakeHead -> x = snakeHead -> x < minx ? maxx - 2 : snakeHead -> x;
        snakeHead -> x = snakeHead -> x > maxx-1 ? minx : snakeHead -> x;
        snakeHead -> y = snakeHead -> y < miny ? maxy - 1 : snakeHead -> y;
        snakeHead -> y = snakeHead -> y > maxy-1 ? miny : snakeHead -> y;
    }
    if(checkEaten())
    {
        putFood();
        score += 1;
        showScore();
        incSnake(snakeTail -> y, snakeTail -> x);
    }
    return 0;
}

int clean_snake(int wait)
{
    Snake *nextSnake =  snakeHead -> next;
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
    mvprintw(snakeLast -> y, snakeLast -> x, "  ");
    refresh();
    return 0;
}

int show_snake(int islost)
{
    attron(COLOR_PAIR(1));
    Snake *nextSnake = islost == 0 ? snakeHead : snakeHead -> next;
    while(nextSnake != NULL)
    {
        mvprintw(nextSnake -> y, nextSnake -> x, "██");
        nextSnake = nextSnake -> next;
    }
    if(islost)
    {
        mvprintw(snakeLast -> y, snakeLast -> x, "██");
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
        clean_snake(0);
        //mvprintw(food.y, food.x, "  ");
        refresh();
        usleep(500000);
        show_snake(1);
        usleep(500000);
    }
    clean_snake(200000);
    clear();
    timeout(-1);
    printw("You Lost. Your score was %d, \nPress space to EXIT...\n", score);
    while(getch() != ' ');
    return 0;
}

int islost()
{

    if(!ENDLESS && (snakeHead -> x < minx || snakeHead -> x > maxx-1 || snakeHead -> y < miny || snakeHead -> y > maxy-1))
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

int drawBox()
{
    int i;
    mvprintw(0, maxx - 10,"┌────────┐");
    mvprintw(1, maxx - 10,"│        │");
    mvprintw(2, 0, "┌");
    for(i = 1; i < maxx-10; i++)
        mvprintw(2, i, "─");
    mvprintw(2, i, "┴────────┤");
    for(i = 3; i < maxy-1; i++)
    {
        mvprintw(i, 0, "│");
        mvprintw(i, maxx-1, "│");
    }
    mvprintw(maxy-1, 0, "└");
    for(i = 1; i < maxx-1; i++)
        mvprintw(maxy-1, i, "─");
    mvprintw(maxy-1, maxx-1, "┘");
    return 0;
}

int main()
{
    srand(time(NULL));
    setlocale(LC_ALL, "");
    WINDOW *win = initscr(); start_color(); cbreak(); noecho(); curs_set(0); timeout(0);
    init_pair(1, COLOR_GREEN, COLOR_GREEN); // Could have done the stuff only with one color pair if there a printed "  "(spaces) in place of food
    init_pair(2, COLOR_RED, COLOR_RED);
    getmaxyx(win, maxy, maxx);
    maxx = to_odd(maxx, -1);
    vel.x = 2;
    vel.y = 0;
    snakeLast = malloc(sizeof(Snake));
    snakeHead = malloc(sizeof(Snake));
    snakeHead -> x = 15; //(int)(maxx/2);
    snakeHead -> y = 5; //(int)(maxy/2);
    snakeHead -> next = NULL;
    snakeTail = snakeHead;
    ENDLESS = 1; // Toggle wrap around world here
    for(int i = 2; i < 11 ; i +=2)
        incSnake(snakeHead -> y, snakeHead -> x - i);
    putFood();
    snakeLast -> y = snakeTail -> y;
    snakeLast -> x = snakeTail -> x;
    drawBox();
    miny = 3;
    minx = 1;
    maxy -= 1;
    maxx -= 2;
    showScore();
    int speed = 100000;
    while(!islost())
    {
        clean_snake(0);
        show_snake(0);
        move_snake();
        update_snake();
        usleep(speed);
    }
    freeMem();
    endwin();
    return 0;
}
