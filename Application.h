/*
 * Application.h
 *
 *  Created on: Mar 30, 2022
 *      Author: Yahia Tawfik
 */

#ifndef APPLICATION_H_
#define APPLICATION_H_

#define TITLE_SCREEN_WAIT 3000
#define GAME_TIMER 1000

enum _GameState
{
    TITLE_SCREEN, MENU_SCREEN, BURGER_TIME, HIGH_SCORES, HOW_TO_COOK, LOSE_SCREEN, WIN_SCREEN
};
typedef enum _GameState GameState;

enum _JoystickPosition
{
    MIDDLE, UP, UP_MIDDLE, DOWN, DOWN_MIDDLE, RIGHT , LEFT
};
typedef enum _JoystickPosition JSposition;

struct _Application
{
    // Put your application members and FSM state variables here!
    // =========================================================================
    JSposition STATE;
    GameState state;
    SWTimer TitleScreenTimer;
    SWTimer GameTime;
    unsigned int x, y; //coordinates for joystick
    int a, score_int; //constant that is added to arrow coordinates to move up and down
    char score_char[2];
    int counter;
    char counter_char[3];
    char tickets_char_r[9];
    int FirstTimeCondition;
    int user_array[8];
    char O_array[10];
    int o;
    int index;
    int curr_ticket[8];
    char recipe_names[10][6];
    char ingredient_names [8][10];
    int tickets[10][8];
    int r;
    int strikes;
    int highScore[3];
    char highScore_char[3];
    int highCount;

};
typedef struct _Application Application;

Application Application_construct()
{
    Application app;
    app.highCount = 0;
    app.state = TITLE_SCREEN;
    app.x = 0;
    app.y = 0;
    app.a = 0;
    app.STATE = MIDDLE;
    app.TitleScreenTimer = SWTimer_construct(TITLE_SCREEN_WAIT);
    app.GameTime = SWTimer_construct(GAME_TIMER);
    app.score_int = 0;
    app.counter = 31;
    app.FirstTimeCondition = 1;
    app.o = 8;
    app.index = 0;
    app.strikes = 0;
    int i, h;
    for(i = 0; i < 3; i++)
    {
        app.score_char[i] = 0;
        app.counter_char[i] = 0;
        app.highScore[i]= 0;
        app.highScore_char[i] = 0;
    }
    for(i = 0; i < 9; i++)
        {
        app.tickets_char_r[i] = 0;
        }
    int p;
    for(p = 0; p < 9; p++)
        {
        app.user_array[p] = 0;
        }
    for (h = 0; h < 10; h++)
    {
        app.O_array[h] = 0;
    }
    char recp[10][6] = {
                 "Mr.B", "M.N.S", "C.S", "C.D", "MR.S", "S.G", "T.C.B", "D.C.B", "O.S", "A.S"
                };
    memcpy(&app.recipe_names[0][0], &recp[0][0], 10*6*sizeof(recp[0][0]));

    char ingredientNames [8][10] =
    {
     "Tomato", "Jalepeno", "Pickles", "Lettuce", "Patty", "Cheese", "Mushroom", "Onion"
    };

    memcpy(&app.ingredient_names[0][0], &ingredientNames[0][0], 8*10*sizeof(ingredientNames[0][0]));

    int Tickets[10][8]  =
    {
     {3,0,1,3,3,0,0,0},
     {0,0,0,0,1,2,3,0},
     {1,0,0,1,1,0,0,0},
     {1,0,0,1,2,0,0,0},
     {0,2,2,0,1,0,0,1},
     {2,0,0,2,1,0,0,0},
     {0,0,0,0,3,3,0,1},
     {0,0,2,0,2,2,0,1},
     {0,0,0,0,1,2,0,3},
     {1,0,1,0,2,1,0,0}
    };

    memcpy(&app.tickets[0][0], &Tickets[0][0], 10*8*sizeof(Tickets[0][0]));

    SWTimer_start(&app.TitleScreenTimer);
    return app;
}

// Called only a single time - inside of main(), where the application is constructed
Application Application_construct();

// Called once per super-loop of the main application.



#endif /* APPLICATION_H_ */
