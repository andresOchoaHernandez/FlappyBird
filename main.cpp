/*
    AUTHOR:Andres Ochoa Hernandez a computer science student in University of Verona, Italy

    FLAPPY BIRD GAME

    Default window's corners (absolute)coordinates [y x] and size:

            * * * * * * * * * * * * * * * * * * *
            *(0,0)                        (0,79)*
            *                                   *
            *                                   *            
            *                                   *
            *(23,0)                      (23,79)*
            * * * * * * * * * * * * * * * * * * *
             
            witdh: 80 chars
            heigth: 24 chars

    Game window's corners (relative to starter [1 1] game window's coordinates) coordinates [y x] and size:

            * * * * * * * * * * * * * * * * * * *
            *(0,0)                        (0,77)*
            *                                   *
            *                                   *            
            *                                   *
            *(21,0)                      (21,77)*
            * * * * * * * * * * * * * * * * * * *
             
            witdh: 78 chars
            heigth: 22 chars

    TODO: Finish implementing-> bool checkIfTouches(BIRD bird,SCENARIO scenario) function that makes the game restart when the bird touches an obstacle
*/

#include <curses.h>
#include <iostream>
#include <vector>
#include <unistd.h>
#include <time.h>
#include <sys/signal.h>

#define BORDER_WIN_HEIGTH 24
#define BORDER_WIN_WITDH 80
#define BORDER_WIN_X 0
#define BORDER_WIN_Y 0

#define GAME_WIN_HEIGTH (BORDER_WIN_HEIGTH -2)
#define GAME_WIN_WITDH (BORDER_WIN_WITDH-2)
#define GAME_WIN_X (BORDER_WIN_X +1)
#define GAME_WIN_Y (BORDER_WIN_Y+1)

/*MIN OFFSET=1,MAX OFFSET=74*/

#define OBSTACLE_VERTICAL_DIST 8

/*Actually is obstacle max heigth +2. This value in fact represents the #of rows in the "cilinder" of the obstacle*/
#define OBSTACLE_MAX_HEIGTH 10

using namespace std;

struct COORDINATE
{
    int pos[2];
}
typedef COORDINATE;

struct OBSTACLE
{
    int length;
    vector<COORDINATE> positions;
}
typedef OBSTACLE;

struct OBSTACLECOUPLE
{
    OBSTACLE obstacles[2];
}
typedef OBSTACLECOUPLE;

struct SCENARIO
{
    int length;
    vector<OBSTACLECOUPLE> obstacleCouples;
}
typedef SCENARIO;

struct BIRD
{
    COORDINATE points[2];
}
typedef BIRD;

bool checkIfTouches(BIRD bird,SCENARIO scenario);
BIRD initBird(void);
OBSTACLECOUPLE generateObstacleCouple(int offset);
OBSTACLE generateUpperObstacle(int offset,int obstacleHeigth);
OBSTACLE generateRelativeBottomObstacle(OBSTACLE obstacle);
SCENARIO generateScenario(void);
void drawScenario(WINDOW *gameWin,SCENARIO *scenario);
void drawObastacle(WINDOW *gameWin,OBSTACLE *obstacle);
void drawObstacleCouple(WINDOW *gameWin,OBSTACLECOUPLE *obstacleCouple);
void thick(WINDOW *gameWin,SCENARIO *scenario,int module,BIRD *bird,bool up);
void updateScenario(SCENARIO *scenario,int module);
void drawBird(WINDOW *gameWin,BIRD *bird);
void updateDownBird(BIRD *bird);
void updateUpBird(BIRD *bird);

int main(int argc,char *argv[])
{
    initscr();
    cbreak();
    curs_set(0);

    WINDOW *borderWin=newwin(BORDER_WIN_HEIGTH,BORDER_WIN_WITDH,BORDER_WIN_Y,BORDER_WIN_X);
    WINDOW *gameWin=newwin(GAME_WIN_HEIGTH,GAME_WIN_WITDH,GAME_WIN_Y,GAME_WIN_X);
    WINDOW *inputWin=newwin(1,1,BORDER_WIN_HEIGTH+1,1);

    box(borderWin,0,0);

    refresh();
    wrefresh(borderWin);

    nodelay(inputWin,true);

    begin:

    wclear(gameWin);

    srand(time(NULL));

    SCENARIO scenario=generateScenario();
    BIRD bird=initBird();

    int moduleX=scenario.obstacleCouples[scenario.length-1].obstacles[1].positions[scenario.obstacleCouples[scenario.length-1].obstacles[1].length-1].pos[1];

    int times=0;
  
    drawBird(gameWin,&bird);
    drawScenario(gameWin,&scenario);

    do{
        mvaddch(1,1,' ');
        mvwprintw(gameWin,8,3,"PRESS SPACE BAR");
        mvwprintw(gameWin,10,7,"TO PLAY ");
        wrefresh(gameWin);

    }while (mvgetch(1,1)!=32);

    while(1)
    {
         switch (wgetch(inputWin))
        {
        case 32:
            thick(gameWin,&scenario,moduleX,&bird,true);
            break;
        case 'e':
            kill(getpid(),SIGINT);
            break;     
        default:
            times=(times+1)%2;
            if(times%2==1)
                thick(gameWin,&scenario,moduleX,&bird,false);
            break;
        }
        if(checkIfTouches(bird,scenario))
        {
            wclear(gameWin);
            goto begin;
        }
    }

    /*END OF PROGRAM*/
    endwin();

    return 0;
}

bool checkIfTouches(BIRD bird,SCENARIO scenario)
{
    /*Bird touching gameWin boards*/
    if(bird.points[0].pos[0]==0 || bird.points[0].pos[0]==GAME_WIN_HEIGTH-1 || bird.points[1].pos[0]==0 || bird.points[1].pos[0]==GAME_WIN_HEIGTH-1 )
    {
        return true;
    }
    
    //TODO

    return false;
}

void thick(WINDOW *gameWin,SCENARIO *scenario,int module,BIRD *bird,bool up)
{
    drawScenario(gameWin,scenario);
    drawBird(gameWin,bird); 

    updateScenario(scenario,module);

    if(up)
    {
        updateUpBird(bird);
    }
    else
    {
        updateDownBird(bird);
    }

    wclear(gameWin);
    usleep(100000);
}
void updateUpBird(BIRD *bird)
{
    bird->points[0].pos[0]-=3;
    bird->points[1].pos[0]-=3;
}

void updateDownBird(BIRD *bird)
{
    bird->points[0].pos[0]+=1;
    bird->points[1].pos[0]+=1;
}

BIRD initBird(void)
{
    BIRD result;

    result.points[0].pos[0]=10;
    result.points[0].pos[1]=28;
    result.points[1].pos[0]=10;
    result.points[1].pos[1]=29;
    
    return result;
}

void drawBird(WINDOW *gameWin,BIRD *bird)
{
    mvwaddch(gameWin,bird->points[0].pos[0],bird->points[0].pos[1],'6');
    mvwaddch(gameWin,bird->points[1].pos[0],bird->points[1].pos[1],'>');

    wrefresh(gameWin);
}

void updateScenario(SCENARIO *scenario,int module)
{
    for(int i=0;i<6;i++)
    {
        for(int j=0;j<scenario->obstacleCouples[i].obstacles[1].length;j++)
        {
            int mod=scenario->obstacleCouples[i].obstacles[1].positions[j].pos[1]-1==-1?module-GAME_WIN_WITDH/2+10:scenario->obstacleCouples[i].obstacles[1].positions[j].pos[1]-1;
            scenario->obstacleCouples[i].obstacles[1].positions[j].pos[1]=mod;
        }
        for(int j=0;j<scenario->obstacleCouples[i].obstacles[0].length;j++)
        {
            int mod=scenario->obstacleCouples[i].obstacles[0].positions[j].pos[1]-1==-1?module-GAME_WIN_WITDH/2+10:scenario->obstacleCouples[i].obstacles[0].positions[j].pos[1]-1;
            scenario->obstacleCouples[i].obstacles[0].positions[j].pos[1]=mod;
        }
    }
}

void drawScenario(WINDOW *gameWin,SCENARIO *scenario)
{
    for(auto elem :scenario->obstacleCouples)
        drawObstacleCouple(gameWin,&elem);

}

OBSTACLECOUPLE generateObstacleCouple(int offset)
{
    OBSTACLECOUPLE result;
    int obstacleHeigth=rand()%(OBSTACLE_MAX_HEIGTH-3)+3;
    
    OBSTACLE upper=generateUpperObstacle(offset,obstacleHeigth);
    OBSTACLE bottom=generateRelativeBottomObstacle(upper);

    result.obstacles[0]=upper;
    result.obstacles[1]=bottom;

    return result;
}

SCENARIO generateScenario(void)
{
    SCENARIO scenario;
    scenario.length=0;

    for(int i=1;i<=74;i+=14)
    {
        scenario.obstacleCouples.push_back(generateObstacleCouple(GAME_WIN_WITDH/2+1+i));
        scenario.length++;
    }
    /*It generates six couples of obstacles*/
    return scenario;
}

OBSTACLE generateRelativeBottomObstacle(OBSTACLE obstacle)
{

    int distance=GAME_WIN_HEIGTH-obstacle.positions[obstacle.length-1].pos[0];
    int obstacleRows= distance - OBSTACLE_VERTICAL_DIST;

    OBSTACLE result;
    result.length=0;

    int initialX;
    int initialY=GAME_WIN_HEIGTH-1;

    for(int times=0;times<obstacleRows;times++)
    {
        COORDINATE coordinate;
        initialX=obstacle.positions[0].pos[1];

        if(times==obstacleRows-2)
        {
            initialX--;

            for(int rectRows=0;rectRows<2;rectRows++)
            {
                COORDINATE coordinateRect;    
                initialX=obstacle.positions[0].pos[1]-1;
                for(int rectCols=0;rectCols<5;rectCols++)
                {
                    coordinateRect.pos[0]=initialY;
                    coordinateRect.pos[1]=initialX;
                    result.positions.push_back(coordinateRect);
                    result.length++;
                    initialX++;
                }
                initialY--;
            }

            break;
        }
        
        for(int squareCols=0;squareCols<3;squareCols++)
        {
            coordinate.pos[0]=initialY;
            coordinate.pos[1]=initialX;
            result.positions.push_back(coordinate);
            result.length++;
            initialX++;
        }

        initialY--;
    }

    return result;
}

OBSTACLE generateUpperObstacle(int offset,int obstacleHeigth)
{
    OBSTACLE obstacle;
    obstacle.length=0;

     for(int squareRow=0;squareRow<obstacleHeigth;squareRow++)
    {
        COORDINATE coordinate;

        for(int squareCol=0;squareCol<3;squareCol++)
        {
            coordinate.pos[0]=squareRow;
            coordinate.pos[1]=squareCol+offset;
            obstacle.positions.push_back(coordinate);
            obstacle.length++;
        }
        if(squareRow==obstacleHeigth-1)
        {
            for(int bottomSquareRow=0;bottomSquareRow<2;bottomSquareRow++)
            {

                for(int bottomSquareCol=0;bottomSquareCol<5;bottomSquareCol++)
                {
                    coordinate.pos[0]=bottomSquareRow+squareRow;
                    coordinate.pos[1]=offset-1+bottomSquareCol;
                    obstacle.positions.push_back(coordinate);
                    obstacle.length++; 
                }
            }
        }
    }

    return obstacle;
}

void drawObstacleCouple(WINDOW *gameWin,OBSTACLECOUPLE *obstacleCouple)
{
    drawObastacle(gameWin,&obstacleCouple->obstacles[0]);
    drawObastacle(gameWin,&obstacleCouple->obstacles[1]);
}

void drawObastacle(WINDOW *gameWin,OBSTACLE *obstacle)
{
    for(auto elem:obstacle->positions)
        mvwaddch(gameWin,elem.pos[0],elem.pos[1],'a');
    
    wrefresh(gameWin);
}