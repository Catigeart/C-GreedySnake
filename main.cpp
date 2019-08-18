#define _CRT_SEQURE_NO_WARNINGS 1
#include<stdio.h>
#include<time.h>
#include<windows.h>
#include<stdlib.h>
#include<conio.h>
#include<math.h>

#define UP 1
#define DOWN 2
#define LEFT 3
#define RIGHT 4

typedef struct snake
{
    int x;
    int y;
    struct snake *next;
}snake;

int sleepTime = 25;

int mapWide = 25;
int mapHeight = 25;
bool canCrossWall = true;
int stepTime = -1;
int score = 0, add = 10;
int bonusInterval = 10;
int sleepTimeLimited = 10;

int bonusCnt;
int commonFood[2];
int bonusFood[2];
snake *q, *head;
int direction;

void gotoxy(int x,int y);
void color(int c);
void createMap(int mapWide, int mapHeight);
void initSnake();
bool checkFood(int x, int y);
void createFood(bool isBonus);
void keyboardControl();
void snakeMove();
void gameOver();
bool crossWallEnd(int x, int y);
bool biteBodyEnd();
void crossWallMove();
void eatFood();
void welcome();
void setting();

int main()
{
    welcome();
    return 0;
}

void gotoxy(int x,int y)
{
    COORD c;
    c.X=x;
    c.Y=y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),c);
}

void color(int c)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), c);
}

void createMap(int mapWide, int mapHeight)
{
    system("cls");
    color(3);
    gotoxy(0, 0);
    for(int i = 0; i < (mapWide + 2) * 2; i += 2)
    {
        printf("■");
    }
    gotoxy(0, mapHeight + 1);
    for(int i = 0; i < (mapWide + 2) * 2; i += 2)
    {
        printf("■");
    }
    for(int i = 1; i < mapHeight + 1; i++)
    {
        gotoxy(0, i);
        printf("■");
        gotoxy((mapWide+1) * 2, i);
        printf("■");
    }
    for(int i = 2; i <= mapWide * 2; i += 2)
    {
        for(int j = 1; j <= mapHeight; j++)
        {
            gotoxy(i, j);
            color(3);
            printf("  ");
        }
    }
}

void initSnake()
{
    direction = RIGHT;
    snake *tail;
    tail = (snake*)malloc(sizeof(snake));
    tail -> x = 24;
    tail -> y = 5;
    tail -> next = NULL;
    for(int i = 1; i <= 4; i++)
    {
        head = (snake*)malloc(sizeof(snake));
        head -> x = tail -> x + 2;
        head -> y = tail -> y;
        head -> next = tail;
        tail = head;
    }
    while(tail != NULL)
    {
        gotoxy(tail -> x, tail -> y);
        color(14);
        printf("★");
        tail = tail -> next;
    }
}

//*****DEAL WITH FOOD BEGIN*****

void createFood(bool isBonus)
{
    bool ok = 0;
    int x, y;
    while(!ok)
    {
        x = rand()% (mapWide - 1) * 2 + 2;
        y = rand()% mapHeight + 1;
        ok = checkFood(x, y);
    }
    gotoxy(x, y);
    if(isBonus)
    {
        stepTime = abs(x - head -> x) / 2 + abs(y - head -> y) + 10;
        bonusFood[0] = x;
        bonusFood[1] = y;
        color(10);
        printf("◆");
    }
    else
    {
        commonFood[0] = x;
        commonFood[1] = y;
        color(12);
        printf("●");
    }
}

bool checkFood(int x, int y)
{
    if((x == commonFood[0] && y == commonFood[1]) || (x == bonusFood[0] && y == bonusFood[1]))
        return false;
    q = head;
    while(q -> next != NULL)
    {
        if(q -> x == x && q -> y == y)
        {
            return false;
        }
        q = q -> next;
    }
    return true;
}

//*****DEAL WITH FOOD END*****

void keyboardControl()
{
    while(true)
    {
        for(int i = 0; i < 10; i++)
        {
            if(GetAsyncKeyState(VK_UP) && direction != DOWN)
                direction = UP;
            else if(GetAsyncKeyState(VK_DOWN) && direction != UP)
                direction = DOWN;
            else if(GetAsyncKeyState(VK_LEFT) && direction != RIGHT)
                direction = LEFT;
            else if(GetAsyncKeyState(VK_RIGHT) && direction != LEFT)
                direction = RIGHT;
            if(GetAsyncKeyState(VK_SPACE))
                while(true)
                {
                    Sleep(sleepTime);
                    if(GetAsyncKeyState(VK_SPACE))
                        break;
                }
            Sleep(sleepTime);
        }
        snakeMove();
        if(canCrossWall)
            crossWallMove();
        else
            if(crossWallEnd(head -> x, head -> y))
                gameOver();
        if(biteBodyEnd())
            gameOver();
    }
}

void snakeMove()
{
    snake *nexthead;
    nexthead = (snake*)malloc(sizeof(snake));
    if(direction == UP)
    {
        nexthead -> x = head -> x;
        nexthead -> y = head -> y - 1;
    }
    else if(direction == DOWN)
    {
        nexthead -> x = head -> x;
        nexthead -> y = head -> y + 1;
    }
    else if(direction == LEFT)
    {
        nexthead -> x = head -> x - 2;
        nexthead -> y = head -> y;
    }
    else if(direction == RIGHT)
    {
        nexthead -> x = head -> x + 2;
        nexthead -> y = head -> y;
    }
    nexthead -> next = head;
    head = nexthead;
    q = head;
    if(nexthead -> x == commonFood[0] && nexthead -> y == commonFood[1])
    {
        eatFood();
        score = score + add;
        if(sleepTime >= sleepTimeLimited)
            sleepTime--;
        createFood(false);
        gotoxy(2 * mapWide + 10, mapHeight / 3);
        color(15);
        printf("Score = %d",score);
        bonusCnt++;
    }
    else if(nexthead -> x == bonusFood[0] && nexthead -> y == bonusFood[1])
    {
        eatFood();
        score = score + 2 * add;
        stepTime = 0;
        gotoxy(2 * mapWide + 10, mapHeight / 3);
        color(15);
        printf("Score = %d",score);
        gotoxy(2 * mapWide + 10, mapHeight / 3 * 2);
        printf("                                       ");
    }
    else
    {
        while(q -> next -> next != NULL)
        {
            gotoxy(q -> x,q -> y);
            color(14);
            printf("★");
            q = q -> next;
        }
        gotoxy(q -> next -> x,q -> next -> y);
        printf("  ");
        free(q -> next);
        q -> next = NULL;
    }
    if(bonusCnt >= bonusInterval && bonusFood[0] == 0 && bonusFood[1] == 0)
    {
        createFood(true);
        bonusCnt = 0;
    }
    if(stepTime > 0)
    {
        color(15);
        gotoxy(2 * mapWide + 10, mapHeight / 3 * 2);
        printf("                                       ");
        gotoxy(2 * mapWide + 10, mapHeight / 3 * 2);
        printf("Bonus limited = %d",stepTime);
        stepTime--;
    }
    else if(stepTime == 0)
    {
        gotoxy(bonusFood[0], bonusFood[1]);
        printf("  ");
        bonusFood[0] = 0;
        bonusFood[1] = 0;
        stepTime = -1;
        gotoxy(2 * mapWide + 10, mapHeight / 3 * 2);
        printf("                                       ");
    }
}

void eatFood()
{
    while(q != NULL)
        {
            gotoxy(q -> x,q -> y);
            color(14);
            printf("★");
            q = q -> next;
        }
}

bool crossWallEnd(int x, int y)
{
    if(x == 0 || x == (mapWide + 1) * 2 || y == 0 || y == mapHeight + 1)
        return true;
    else
        return false;
}

bool biteBodyEnd()
{
    snake *body;
    body = head -> next;
    while(body != NULL)
    {
        if(body -> x == head -> x && body -> y == head -> y)
            return true;
        body = body -> next;
    }
    return false;
}

void crossWallMove()
{
    int situation = 0;
    if(head -> x == 0)
        situation = 1;
    else if(head -> x == (mapWide + 1) * 2)
        situation = 2;
    else if(head -> y == 0)
        situation = 3;
    else if(head -> y == mapHeight + 1)
        situation = 4;
    if(situation == 0)
        return;
    else
    {
        gotoxy(head -> x, head -> y);
        color(3);
        printf("■");
        if(situation == 1)
            head -> x = mapWide * 2;
        else if(situation == 2)
            head -> x = 2;
        else if(situation == 3)
            head -> y = mapHeight;
        else if(situation == 4)
            head -> y = 1;
        gotoxy(head -> x, head -> y);
        color(14);
        printf("★");
        return;
    }
}

void gameOver()
{
    system("cls");
    color(15);
    printf("Game over!你的最终得分是 %d\n",score);
    printf("请按任意键继续...\n");
    getchar();
    welcome();
}

void welcome()
{
    printf("欢迎来到贪吃蛇，请通过输入对应数字选择选项：\n");
    printf("1、开始游戏 2、退出游戏\n");
    switch(getch())
    {
    case '1':
        createMap(mapWide, mapHeight);
        initSnake();
        createFood(false);
        gotoxy(2 * mapWide + 10, mapHeight / 3);
        color(15);
        printf("Score = %d",score);
        keyboardControl();
        break;
    case '2':
        exit(0);
    default:
        printf("警告：请输入合法的字符！\n");
        welcome();
    }
}



