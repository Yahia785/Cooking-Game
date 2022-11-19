/**
 * Starter code for Project 2. Good luck!
 */

#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <FUNCTIONS.h> //New file which contains all functions declarations
#include <HAL/HAL.h>
#include <HAL/Timer.h>
#include <Application.h>
#include <Joystick.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <HAL/colors.h>


#define TTLE_SCREEN_WAIT 3000
#define UP_TRESHOLD 14000
#define DOWN_TRESHOLD 2000
#define MIDDLE_TRESHOLD_MAX_Y 8000
#define MIDDLE_TRESHOLD_MIN_Y 6000
#define ARROW_MOVING_CONSTANT 15 //this constant is added to or subtracted from arrow coordinates to move the arrow up and down
#define MAX_STRIKES 3
#define GAME_TIME 31
#define NUM_OF_INGREDIENTS 8
#define NUM_OF_SELECTIONS 3
#define NULL_STATE -1
#define MOVE_SQUARE 7
#define NUM_OF_RECIPES 10
#define SUCCESS_STATE 1
#define SCORE_ADDITION 5
#define TWO_STRIKES 2

// Non-blocking check. Whenever Launchpad S1 is pressed, LED1 turns on.
static void InitNonBlockingLED()
{
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1);
}

// Non-blocking check. Whenever Launchpad S1 is pressed, LED1 turns on.
static void PollNonBlockingLED()
{
    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);
    if (GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN1) == 0) {
        GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0);
    }
}

int main()
{
    WDT_A_holdTimer();

    // Initialize the system clock and background hardware timer, used to enable
        // software timers to time their measurements properly.
        InitSystemTiming();

        initialize();

        Timer32_initModule(TIMER32_0_BASE, // There are two timers, we are using the one with the index 0
                           TIMER32_PRESCALER_1, // The prescaler value is 1; The clock is not divided before feeding the counter
                           TIMER32_32BIT, // The counter is used in 32-bit mode; the alternative is 16-bit mode
                           TIMER32_PERIODIC_MODE); //This options is irrelevant for a one-shot timer
        // Initialize the maiN HAL object
        HAL hal = HAL_construct();
        Application app = Application_construct();
        // Do not remove this line. This is your non-blocking check.
        InitNonBlockingLED();

        ShowTitleScreen (&hal, &hal.g_sContext);

    // Do not remove this line. This is your non-blocking check.
    InitNonBlockingLED();
    while (1)
    {
        // Do not remove this line. This is your non-blocking check

        BusyKitchen_loop(&hal, &app);
        PollNonBlockingLED();
        getSampleJoyStick(&app.x, &app.y);
        HAL_refresh(&hal);

    }
}
//Main switch statement with all game states in it
void BusyKitchen_loop(HAL* hal_p, Application* app_p )
{
    switch (app_p->state)
    {
        case TITLE_SCREEN:
           HandleTitleScreen (hal_p, app_p);
            break;

        case MENU_SCREEN:
            HandleMenuScreen(hal_p, app_p);//ArrowLoop(hal_p, app_p);
         break;

        case HOW_TO_COOK:
             HandleHowToCookScreen(hal_p, app_p);
        break;

        case HIGH_SCORES:
            HandleHighScoresScreen(hal_p, app_p);
            break;

        case BURGER_TIME:
            HandleBurgerTimeScreen(hal_p, app_p);
            break;

        case LOSE_SCREEN:
            HandleLoseScreen(hal_p, app_p);
            break;

        case WIN_SCREEN:
            HandleWinScreen(hal_p, app_p);
            break;

        default:
            break;

    }

}

//Function for graphics display of title screen
void ShowTitleScreen (HAL* hal_p, Graphics_Context* graphics_p)
{
    Graphics_clearDisplay(graphics_p);
    Graphics_drawString(graphics_p, (int8_t *)"Spring 2022", -1, 25, 20, true);
    Graphics_drawString(graphics_p, (int8_t *)"Project 2", -1, 35, 30, true);
    Graphics_drawString(graphics_p, (int8_t *)"Yahia Tawfik", -1, 25, 50, true);
    Graphics_drawString(graphics_p, (int8_t *)"H-3Ave.N", -1, 35, 70, true);
    Graphics_drawString(graphics_p, (int8_t *)"Utopian Burger", -1, 25, 90, true);
    Graphics_drawString(graphics_p, (int8_t *)"making action!", -1, 25, 100, true);
}

// function that handles title screen. A timer of 3 seconds is constructed and when it
// expires the screen automatically goes to menu screen
void HandleTitleScreen(HAL *hal_p, Application *app_p)
{

     if (SWTimer_expired(&app_p->TitleScreenTimer))
        {
            app_p->state = MENU_SCREEN;
            ShowMenuScreen(hal_p, app_p);
        }
}

//graphics display for menu screen
void ShowMenuScreen(HAL*hal_p, Application* app_p)
{

    Graphics_clearDisplay(&hal_p->g_sContext);
    Graphics_drawString(&hal_p->g_sContext, (int8_t *)"H-3Ave.N Version 2.0", -1, 5, 20, true);
    Graphics_drawString(&hal_p->g_sContext, (int8_t *)"-----Main Menu-----", -1, 5, 40, true);
    Graphics_drawString(&hal_p->g_sContext, (int8_t *)"Burger Time", -1, 30, 60, true);
    Graphics_drawString(&hal_p->g_sContext, (int8_t *)"High Scores", -1, 30, 75, true);
    Graphics_drawString(&hal_p->g_sContext, (int8_t *)"How To Cook", -1, 30, 90, true);


}

//updates position of arrow in menu screen according to treshold value of joystick. "a" is the constant added
// to y coordinate of arrow position. This causes the arrow to be drawn below or above old arrow drawn
void UpdateArrow(HAL *hal_p, Application *app_p)
{
    Graphics_setForegroundColor(&hal_p->g_sContext, GRAPHICS_COLOR_LIGHT_BLUE);
    JSposition state = ArrowLoop(hal_p, app_p);

    Graphics_drawString(&hal_p->g_sContext, (int8_t *)">", -1, 20, 60 + app_p->a, true);

    if (state == UP && app_p->a > 0)
    {
        Graphics_setForegroundColor(&hal_p->g_sContext, GRAPHICS_COLOR_BLACK);
        Graphics_drawString(&hal_p->g_sContext, (int8_t*) ">", -1, 20, 60 + app_p->a,
        true);
        app_p->a = app_p->a - ARROW_MOVING_CONSTANT;
        Graphics_setForegroundColor(&hal_p->g_sContext,
        GRAPHICS_COLOR_LIGHT_BLUE);
        Graphics_drawString(&hal_p->g_sContext, (int8_t*) ">", -1, 20, 60 + app_p->a,
        true);
    }
    else if (state == DOWN && app_p->a < 30)
    {
        Graphics_setForegroundColor(&hal_p->g_sContext, GRAPHICS_COLOR_BLACK);
        Graphics_drawString(&hal_p->g_sContext, (int8_t*) ">", -1, 20, 60 + app_p->a,
        true);
        app_p->a = app_p->a + ARROW_MOVING_CONSTANT;
        Graphics_setForegroundColor(&hal_p->g_sContext,
        GRAPHICS_COLOR_LIGHT_BLUE);
        Graphics_drawString(&hal_p->g_sContext, (int8_t*) ">", -1, 20, 60 + app_p->a,
        true);
    }

}

//treshold value initialized for joystcik when it is down. if joystick is down function returns true
bool JoystickIsDown(HAL *hal_p, Application *app_p)
{
    //getSampleJoyStick(&app_p->x, &app_p->y);
    if (app_p->y < DOWN_TRESHOLD)
    {
        return true;
    }
    return false;
}

//treshold value initialized for joystcik when it is in the middle. if joystick is in middle position function returns true
bool JoystickIsMiddle(HAL* hal_p, Application* app_p)
{
    //getSampleJoyStick(&app_p->x, &app_p->y);
        if (MIDDLE_TRESHOLD_MIN_Y < app_p->y && app_p->y < MIDDLE_TRESHOLD_MAX_Y)
        {
            return true;
        }
        return false;
}
//treshold value initialized for joystcik when it is up. if joystick is in up position function returns true
bool JoystickIsUp(HAL* hal_p, Application* app_p)
{
        if ( app_p->y > UP_TRESHOLD)
        {
            return true;
        }
        return false;
}

//switch statement for joystick position. up_middle is the state where the joystick is pressed up then released back to middle
//down_middle is the state where arrow is pressed down and then released back to middle position
JSposition ArrowLoop(HAL *hal_p, Application *app_p)
{
    static JSposition state = MIDDLE;

    switch (state)
    {
    case MIDDLE:
        if (JoystickIsUp(hal_p, app_p))
        {
            state = UP_MIDDLE;
        }
        else if (JoystickIsDown(hal_p, app_p))
        {
            state = DOWN_MIDDLE;
        }
        break;

    case UP_MIDDLE:
        if (JoystickIsMiddle(hal_p, app_p))
        {
            state = MIDDLE;
            return UP;
        }
        break;

    case DOWN_MIDDLE:
        if (JoystickIsMiddle(hal_p, app_p))
        {
            state = MIDDLE;
            return DOWN;
        }
        break;

    }
    return MIDDLE;
}

//function that handles menu screen. arrow position is updated and when the joystick button is pressed according to the arrow position
// the screen changes to either game screen, instruction screen or high score screen.
void HandleMenuScreen(HAL* hal_p, Application* app_p)
{
    UpdateArrow(hal_p, app_p);
    if (app_p->a > ARROW_MOVING_CONSTANT)
    {
        if (Button_isTapped(&hal_p->boosterpackJS))
        {
        ShowHowToCookScreen(hal_p, app_p);
        app_p->state = HOW_TO_COOK;
        }
    }
    else if (app_p->a == ARROW_MOVING_CONSTANT)
    {
        if (Button_isTapped(&hal_p->boosterpackJS))
                {
                ShowHighScoreScreen(hal_p, app_p);
                app_p->state = HIGH_SCORES;
                }
    }
    else if (app_p->a < ARROW_MOVING_CONSTANT)
    {
        if (Button_isTapped(&hal_p->boosterpackJS))
                        {
            ShowBurgerTimeScreen(hal_p, app_p);
            app_p->state = BURGER_TIME;
                        }
    }
}

//graphics display for how to cook screen which is the game screen.
void ShowHowToCookScreen(HAL* hal_p, Application* app_p)
{
    Graphics_clearDisplay(&hal_p->g_sContext);
    Graphics_drawString(&hal_p->g_sContext, (int8_t*) "How to play", -1, 1, 5, true);
    Graphics_drawString(&hal_p->g_sContext, (int8_t*) "-----------", -1, 1, 13, true);
    Graphics_drawString(&hal_p->g_sContext, (int8_t*) "Move joystick towards", -1, 1, 20, true);
    Graphics_drawString(&hal_p->g_sContext, (int8_t*) "an ingredient to", -1, 1, 30, true);
    Graphics_drawString(&hal_p->g_sContext, (int8_t*) "highlight it.", -1, 1, 40, true);
    Graphics_drawString(&hal_p->g_sContext, (int8_t*) "Press boostS1 to add", -1, 1, 55, true);
    Graphics_drawString(&hal_p->g_sContext, (int8_t*) "one serving to dish.", -1, 1, 65, true);
    Graphics_drawString(&hal_p->g_sContext, (int8_t*) "Press boostS2 to", -1, 1, 80, true);
    Graphics_drawString(&hal_p->g_sContext, (int8_t*) "reset dish.", -1, 1, 90, true);
    Graphics_drawString(&hal_p->g_sContext, (int8_t*) "Press joystick in", -1, 1, 105, true);
    Graphics_drawString(&hal_p->g_sContext, (int8_t*) "to serve dish.", -1, 1, 115, true);
    Graphics_drawString(&hal_p->g_sContext, (int8_t*) "Follow the recipe", -1, 1, 130, true);
    Graphics_drawString(&hal_p->g_sContext, (int8_t*) "ticket carefully!", -1, 1, 140, true);
}

//handling the instruction screen. if joystick button is pressed screen changes to menu screen
void HandleHowToCookScreen(HAL* hal_p, Application* app_p)
{
    if (Button_isTapped(&hal_p->boosterpackJS))
            {
            ShowMenuScreen(hal_p, app_p);
            app_p->state = MENU_SCREEN;
            }
}

//handling highscores screen. This function updates the high scores and orders them from biggest to smallest
void HandleHighScores(HAL *hal_p, Application *app_p)
{
//    if (app_p->highScore[0] < app_p->score_int)
//    {
//        app_p->highScore[0] = app_p->score_int;
//    }
//    else if (app_p->score_int > app_p->highScore[1]
//            && app_p->score_int < app_p->highScore[0])
//    {
//        app_p->highScore[1] = app_p->score_int;
//    }
//    else if (app_p->score_int > app_p->highScore[2]
//            && app_p->score_int < app_p->highScore[1])
//    {
//        app_p->highScore[2] = app_p->score_int;
//    }

    int score = app_p->score_int;
    int i;
    if(app_p->highCount < 3)
    {
        app_p->highScore[app_p->highCount] = score;
        return;
    }
    for(i = 0; i<3; i++)
    {
        if( score > app_p->highScore[i])
        {
            int temp = app_p->highScore[i];
            app_p->highScore[i] = score;
            score = temp;
        }
    }
}

//graphics display for high score screen
void ShowHighScoreScreen(HAL* hal_p, Application* app_p)
{
       Graphics_clearDisplay(&hal_p->g_sContext);
       Graphics_drawString(&hal_p->g_sContext, (int8_t*) "High Scores", -1, 1, 5, true);
       Graphics_drawString(&hal_p->g_sContext, (int8_t*) "-----------", -1, 1, 13, true);
       sprintf(app_p->highScore_char, "%d", app_p->highScore[0]);
       Graphics_drawString(&hal_p->g_sContext, (int8_t*)app_p->highScore_char, -1, 20, 23, true);
       sprintf(app_p->highScore_char, "%d", app_p->highScore[1]);
       Graphics_drawString(&hal_p->g_sContext, (int8_t*)app_p->highScore_char, -1, 20, 33, true);
       sprintf(app_p->highScore_char, "%d", app_p->highScore[2]);
       Graphics_drawString(&hal_p->g_sContext, (int8_t*)app_p->highScore_char, -1, 20, 43, true);
       Graphics_drawString(&hal_p->g_sContext, (int8_t*) "$", -1, 1, 23, true);
       Graphics_drawString(&hal_p->g_sContext, (int8_t*) "$", -1, 1, 33, true);
       Graphics_drawString(&hal_p->g_sContext, (int8_t*) "$", -1, 1, 43, true);
}

//graphics display for game screen
void ShowBurgerTimeScreen(HAL* hal_p, Application* app_p)
{
    Graphics_clearDisplay(&hal_p->g_sContext);
    Graphics_drawImage(&hal_p->g_sContext, &colors8BPP_UNCOMP, 0, 0);
    Graphics_drawCircle(&hal_p->g_sContext, 64, 64, 35);
    Graphics_drawCircle(&hal_p->g_sContext, 64, 64, 60);
    Graphics_drawLineV(&hal_p->g_sContext, 64, 99, 124);
    Graphics_drawLineV(&hal_p->g_sContext, 64, 29, 4);
    Graphics_drawLineH(&hal_p->g_sContext, 29, 4, 64);
    Graphics_drawLineH(&hal_p->g_sContext, 99, 124, 64);
    Graphics_drawLine(&hal_p->g_sContext, 88.74, 88.74, 106.43, 106.43);
    Graphics_drawLine(&hal_p->g_sContext, 88.74, 39.26, 106.43, 21.57);
    Graphics_drawLine(&hal_p->g_sContext, 39.26, 88.74, 21.57, 106.43);
    Graphics_drawLine(&hal_p->g_sContext, 39.26, 39.26, 21.57, 21.57);
    Graphics_drawString(&hal_p->g_sContext, (int8_t*) "$", -1, 0, 0, true);
    sprintf(app_p->score_char,"%d", app_p->score_int);
    Graphics_drawString(&hal_p->g_sContext, (int8_t*) app_p->score_char, -1, 6, 0, true);
    Graphics_drawString(&hal_p->g_sContext, (int8_t*) "00:", -1, 97, 0, true);
    DrawRectangles_V(hal_p, app_p);
    DrawRectangles_H(hal_p, app_p);

}

//handling game screen where all functions responsible for the game are called here. The functions called here are responsible for highliting ingredients,
//selecting ingredients, recipe names, ingredients, checking plate when order is submitted, calculating highscores and ordering them, timer for game and strikes.
void HandleBurgerTimeScreen(HAL *hal_p, Application *app_p)
{
    HandleGameTime(hal_p, app_p);
    IngredientStates(hal_p, app_p);
    sprintf(app_p->counter_char, "%02d", app_p->counter);
    Graphics_drawString(&hal_p->g_sContext, (int8_t*) app_p->counter_char, -1,
                        113, 0, true);
    HighlightIngredient(hal_p, app_p);
    DontHighlightIngredient(hal_p, app_p);
    if (Button_isTapped(&hal_p->boosterpackJS))
    {
        ShowBurgerTimeScreen(hal_p, app_p);
    }
    if (Button_isTapped(&hal_p->boosterpackJS)
            || app_p->FirstTimeCondition == 1)
    {
        if (app_p->FirstTimeCondition == 0)
        {
            CheckOrder(hal_p, app_p);
            for (app_p->index = 0; app_p->index < 8; app_p->index++)
            {
                app_p->user_array[app_p->index] = 0;
            }
            HandleHighScores(hal_p, app_p);
        }
       if (app_p->strikes < MAX_STRIKES)
       {
        HandleTickets(hal_p, app_p);
       }
        app_p->FirstTimeCondition = 0;
    }
    HandleSquares(hal_p, app_p);
    WhenTimeEnds(hal_p, app_p);
}

// function responsible for when time ends. If the score is 0 lose screen is displayed and when score is greater
//than zero win screen is displayed
void WhenTimeEnds(HAL *hal_p, Application *app_p)
{
    if (app_p->counter == 0)
    {
        if (app_p->score_int != 0)
        {
            ShowWinScreen(hal_p, app_p);
            app_p->state = WIN_SCREEN;
            app_p->counter = GAME_TIME;
            app_p->FirstTimeCondition = 1;
            HandleHighScores(hal_p, app_p);
            for (app_p->index = 0; app_p->index < NUM_OF_INGREDIENTS; app_p->index++)
            {
                app_p->user_array[app_p->index] = 0;
            }
            app_p->strikes = 0;
        }
        else
        {
            ShowLoseScreen(hal_p, app_p);
            app_p->state = LOSE_SCREEN;
            app_p->counter = GAME_TIME;
            app_p->FirstTimeCondition = 1;
            for (app_p->index = 0; app_p->index < NUM_OF_INGREDIENTS; app_p->index++)
            {
                app_p->user_array[app_p->index] = 0;
            }
            app_p->strikes = 0;
        }
    }
}

//Function that handles squares drawn to select ingredients. when bbs1 is pressed squares are drawn to select number of ingredients
//when bbs2 is pressed last square selected for ingredient is ereased.
void HandleSquares(HAL *hal_p, Application *app_p)
{
    if (Button_isTapped(&hal_p->boosterpackS1))
    {
        app_p->index = IngredientStates(hal_p, app_p);
        if (app_p->index != NULL_STATE)
        {
            if (app_p->user_array[app_p->index] < NUM_OF_SELECTIONS)
            {
                Graphics_setForegroundColor(&hal_p->g_sContext,
                GRAPHICS_COLOR_LIGHT_BLUE);
                FillSquare(hal_p, app_p->index, app_p->user_array[app_p->index],
                           app_p);
                app_p->user_array[app_p->index] += 1;
            }
        }
    }
    if (Button_isTapped(&hal_p->boosterpackS2))
    {
        app_p->index = IngredientStates(hal_p, app_p);
        if (app_p->index != NULL_STATE)
        {
            if (app_p->user_array[app_p->index] > 0)
            {
                app_p->user_array[app_p->index]--;
                Graphics_setForegroundColor(&hal_p->g_sContext, GRAPHICS_COLOR_BLACK);
                FillSquare(hal_p, app_p->index, app_p->user_array[app_p->index], app_p);
                Graphics_setForegroundColor(&hal_p->g_sContext, GRAPHICS_COLOR_LIGHT_BLUE);
                DrawSquare(hal_p, app_p->index, app_p->user_array[app_p->index],
                           app_p);
            }
        }
    }
}

//function responsible for filling the squares under each ingredient. GetSquareCoor fuction gets the coordinates of the squares based on the joystick position.
//then an array with square coordinates is passed to the square coordiates in the drawing function
void FillSquare(HAL* hal_p, int ing_num, int square_num, Application* app_p)
{
    int arr[4];
    GetSquareCoor(arr,ing_num, square_num, hal_p, app_p);

    Graphics_Rectangle R;
    R.xMin = arr[0];
    R.xMax = arr[1];
    R.yMin = arr[2];
    R.yMax = arr[3];
    Graphics_fillRectangle(&hal_p->g_sContext, &R);
}

//same as fill square function but this function is used to erease drawn squares when bbs2 is pressed. Black square is drawn of the filled square
//then a hollow square is drawn over the black square
void DrawSquare(HAL* hal_p, int ing_num, int square_num, Application* app_p)
{
    int arr[4];
    GetSquareCoor(arr,ing_num, square_num, hal_p, app_p);

    Graphics_Rectangle R;
    R.xMin = arr[0];
    R.xMax = arr[1];
    R.yMin = arr[2];
    R.yMax = arr[3];
    Graphics_drawRectangle(&hal_p->g_sContext, &R);
}

//function that gets the coordinates of the square that needs to be filled based on the position of the joystick
void GetSquareCoor(int* arr,int ing_num, int square_num, HAL* hal_p, Application* app_p)
{
    if(ing_num == 0)
    {
        arr[0] = 67; //x.min
        arr[1] = 71; //x.max
        arr[2] = 7 + (app_p->user_array[app_p->index] * MOVE_SQUARE);  //y.min
        arr[3] = 11 + (app_p->user_array[app_p->index] * MOVE_SQUARE); //y.max

    }
    else if( ing_num == 1)
    {
        arr[0] = 106 + (app_p->user_array[app_p->index] * MOVE_SQUARE); //x.min
        arr[1] = 102 + (app_p->user_array[app_p->index] * MOVE_SQUARE); //x.max
        arr[2] = 60;  //y.min
        arr[3] = 56; //y.max
    }
    else if (ing_num == 2)
    {
        arr[0] = 106 + (app_p->user_array[app_p->index] * MOVE_SQUARE); //x.min
        arr[1] = 102 + (app_p->user_array[app_p->index] * MOVE_SQUARE); //x.max
        arr[2] = 68;  //y.min
        arr[3] = 72; //y.max
    }
    else if (ing_num == 3)
    {
        arr[0] = 67; //x.min
        arr[1] = 71; //x.max
        arr[2] = 103 + (app_p->user_array[app_p->index] * MOVE_SQUARE);  //y.min
        arr[3] = 107 + (app_p->user_array[app_p->index] * MOVE_SQUARE); //y.max
    }
    else if (ing_num == 4)
    {
        arr[0] = 58; //x.min
        arr[1] = 62; //x.max
        arr[2] = 103 + (app_p->user_array[app_p->index] * MOVE_SQUARE);  //y.min
        arr[3] = 107 + (app_p->user_array[app_p->index] * MOVE_SQUARE); //y.max
    }
    else if (ing_num == 5)
    {
        arr[0] = 7 + (app_p->user_array[app_p->index] * MOVE_SQUARE); //x.min
        arr[1] = 11 + (app_p->user_array[app_p->index] * MOVE_SQUARE); //x.max
        arr[2] = 68;  //y.min
        arr[3] = 72; //y.max
    }
    else if (ing_num == 6)
    {
        arr[0] = 7 + (app_p->user_array[app_p->index] * MOVE_SQUARE); //x.min
        arr[1] = 11 + (app_p->user_array[app_p->index] * MOVE_SQUARE); //x.max
        arr[2] = 60;  //y.min
        arr[3] = 56; //y.max
    }
    else if (ing_num == 7)
    {
        arr[0] = 58; //x.min
        arr[1] = 62; //x.max
        arr[2] = 7 + (app_p->user_array[app_p->index] * MOVE_SQUARE);  //y.min
        arr[3] = 11 + (app_p->user_array[app_p->index] * MOVE_SQUARE); //y.max
    }
}

//for handling game timer. A one second timer is constructed and when it expires the counter is decremented. The counter value is displayed
// on the sceen as the time
void HandleGameTime(HAL* hal_p, Application* app_p)
{
    if (SWTimer_expired(&app_p->GameTime))
    {
        app_p->counter--;
        app_p->GameTime = SWTimer_construct(GAME_TIMER);
        SWTimer_start(&app_p->GameTime);
    }
}

//graphics display for lose screen if score is 0
void ShowLoseScreen(HAL *hal_p, Application *app_p)
{
    Graphics_clearDisplay(&hal_p->g_sContext);
    Graphics_drawString(&hal_p->g_sContext, (int8_t*) "You Lost :(", -1, 35, 64, true);
}

//graphics display for win screen if score is greater than 0
void ShowWinScreen(HAL* hal_p, Application* app_p)
{
    app_p->highCount++;
    Graphics_clearDisplay(&hal_p->g_sContext);
    Graphics_drawString(&hal_p->g_sContext, (int8_t*) "You are a winner! :)", -1, 25, 64, true);
}

//This function assigns each ingredient with the threshold range for joystick. Therefore, when joystick moves between different ranges
//different ingredients are selected
int IngredientStates(HAL *hal_p, Application *app_p)
{

    if ((app_p->x >= 8400 && app_p->x <= 13000)
            && (app_p->y >= 13200 && app_p->y <= 16370))
    {
       app_p->o = 0;
    }
    if ((app_p->x >= 12990 && app_p->x <= 16000)
            && (app_p->y >= 7700 && app_p->y <= 13190))
    {
        app_p-> o = 1;
    }
    if ((app_p->x >= 13800 && app_p->x <= 15990)
            && (app_p->y >= 1400 && app_p->y <= 7690))
    {
        app_p->  o = 2;
    }
    if ((app_p->x >= 7200 && app_p->x <= 13790)
            && (app_p->y >= 15 && app_p->y <= 1390))
    {
        app_p-> o = 3;
    }
    if ((app_p->x >= 1800 && app_p->x <= 7150)
            && (app_p->y >= 3 && app_p->y <= 80))
    {
        app_p-> o = 4;
    }
    if ((app_p->x >= 5 && app_p->x <= 1750)
            && (app_p->y >= 90 && app_p->y <= 7000))
    {
        app_p->  o = 5;
    }
    if ((app_p->x >= 15 && app_p->x <= 3000)
            && (app_p->y >= 6990 && app_p->y <= 14990))
    {
        app_p->  o = 6;
    }
    if ((app_p->x >= 2990 && app_p->x <= 8380)
            && (app_p->y >= 15000 && app_p->y <= 16380))
    {
        app_p-> o = 7;
    }
    if ((app_p->x >= 7400 && app_p->x <= 7800)
            && (app_p->y >= 7500 && app_p->y <= 7900))
    {
        app_p->o = 8;
    }
 return app_p->o;
}

//handling tickets for the game. 2d array respnsible for each recipe with ingredients assigned to it. rand() function
// is used to assign different recipe numbers to be displayed on game screen
void HandleTickets(HAL *hal_p, Application *app_p)
{
    int x;
    x = 0;
    int i;

    app_p->r = rand() % NUM_OF_RECIPES;

   Graphics_drawString(&hal_p->g_sContext, (int8_t*) app_p->recipe_names[app_p->r], -1, 55, 35, true);

    for (i = 0; i < 8; i++)
        {
        if (app_p->tickets[app_p->r][i] != 0)
            {
            x++;
                sprintf(app_p->tickets_char_r, "%d", app_p->tickets[app_p->r][i]);
                Graphics_drawString(&hal_p->g_sContext, (int8_t*) app_p->tickets_char_r, -1, 40, 40 + (10 * x), true);
                Graphics_drawString(&hal_p->g_sContext, (int8_t*) app_p->ingredient_names[i], -1, 47, 40 + (10 * x), true);
            }
        }

}

//verticle Rectangles drawn for grame screen under each ingredient for game display
void DrawRectangles_V(HAL *hal_p, Application *app_p)
{
    int i;
    for (i = 0; i < 3; i++)
    {
        Graphics_Rectangle R;
        R.xMin = 67;
        R.xMax = 71;
        R.yMin = 103 + (i * MOVE_SQUARE);
        R.yMax = 107 + (i * MOVE_SQUARE);
        Graphics_drawRectangle(&hal_p->g_sContext, &R);

        R.xMin = 58;
        R.xMax = 62;
        R.yMin = 103 + (i * MOVE_SQUARE);
        R.yMax = 107 + (i * MOVE_SQUARE);
        Graphics_drawRectangle(&hal_p->g_sContext, &R);

        R.xMin = 58;
        R.xMax = 62;
        R.yMin = 7 + (i * MOVE_SQUARE);
        R.yMax = 11 + (i * MOVE_SQUARE);
        Graphics_drawRectangle(&hal_p->g_sContext, &R);

        R.xMin = 67;
        R.xMax = 71;
        R.yMin = 7 + (i * MOVE_SQUARE);
        R.yMax = 11 + (i * MOVE_SQUARE);
        Graphics_drawRectangle(&hal_p->g_sContext, &R);
    }
}

//Horizontal squares drawn for ingredients for display
void DrawRectangles_H(HAL *hal_p, Application *app_p)
{
    int i;
    for (i = 0; i < NUM_OF_SELECTIONS; i++)
    {
     Graphics_Rectangle R;

        R.xMin = 7 + (i * MOVE_SQUARE);
        R.xMax = 11 + (i * MOVE_SQUARE);
        R.yMin = 60;
        R.yMax = 56;
        Graphics_drawRectangle(&hal_p->g_sContext, &R);

        R.xMin = 7 + (i * MOVE_SQUARE);
        R.xMax = 11 + (i * MOVE_SQUARE);
        R.yMin = 68;
        R.yMax = 72;
        Graphics_drawRectangle(&hal_p->g_sContext, &R);

        R.xMin = 106 + (i * MOVE_SQUARE);
        R.xMax = 102 + (i * MOVE_SQUARE);
        R.yMin = 68;
        R.yMax = 72;
        Graphics_drawRectangle(&hal_p->g_sContext, &R);

        R.xMin = 106 + (i * MOVE_SQUARE);
        R.xMax = 102 + (i * MOVE_SQUARE);
        R.yMin = 60;
        R.yMax = 56;
        Graphics_drawRectangle(&hal_p->g_sContext, &R);

    }
}

//highlighting lines between ingredients to select them based on the position of joystick. o is the value returned from
//ingrediet states function. It holds the value for igredient states depending on joystick position
void HighlightIngredient(HAL *hal_p, Application *app_p)
{
    if (app_p->o == 0)
    {
        Graphics_setForegroundColor(&hal_p->g_sContext, GRAPHICS_COLOR_RED);
        Graphics_drawLineV(&hal_p->g_sContext, 64, 29, 4);
        Graphics_drawLine(&hal_p->g_sContext, 88.74, 39.26, 106.43, 21.57);
    }
    else if (app_p->o == 1)
    {
        Graphics_setForegroundColor(&hal_p->g_sContext, GRAPHICS_COLOR_RED);
        Graphics_drawLine(&hal_p->g_sContext, 88.74, 39.26, 106.43, 21.57);
        Graphics_drawLineH(&hal_p->g_sContext, 99, 124, 64);
    }
    else if (app_p->o == 2)
    {
        Graphics_setForegroundColor(&hal_p->g_sContext, GRAPHICS_COLOR_RED);
        Graphics_drawLine(&hal_p->g_sContext, 88.74, 88.74, 106.43, 106.43);
        Graphics_drawLineH(&hal_p->g_sContext, 99, 124, 64);
    }
    else if (app_p->o == 3)
    {
        Graphics_setForegroundColor(&hal_p->g_sContext, GRAPHICS_COLOR_RED);
        Graphics_drawLine(&hal_p->g_sContext, 88.74, 88.74, 106.43, 106.43);
        Graphics_drawLineV(&hal_p->g_sContext, 64, 99, 124);
    }
    else if (app_p->o == 4)
    {
        Graphics_setForegroundColor(&hal_p->g_sContext, GRAPHICS_COLOR_RED);
        Graphics_drawLine(&hal_p->g_sContext, 39.26, 88.74, 21.57, 106.43);
        Graphics_drawLineV(&hal_p->g_sContext, 64, 99, 124);
    }
    else if (app_p->o == 5)
    {
        Graphics_setForegroundColor(&hal_p->g_sContext, GRAPHICS_COLOR_RED);
        Graphics_drawLine(&hal_p->g_sContext, 39.26, 88.74, 21.57, 106.43);
        Graphics_drawLineH(&hal_p->g_sContext, 29, 4, 64);
    }
    else if (app_p->o == 6)
    {
        Graphics_setForegroundColor(&hal_p->g_sContext, GRAPHICS_COLOR_RED);
        Graphics_drawLine(&hal_p->g_sContext, 39.26, 39.26, 21.57, 21.57);
        Graphics_drawLineH(&hal_p->g_sContext, 29, 4, 64);
    }
    else if (app_p->o == 7)
    {
        Graphics_setForegroundColor(&hal_p->g_sContext, GRAPHICS_COLOR_RED);
        Graphics_drawLine(&hal_p->g_sContext, 39.26, 39.26, 21.57, 21.57);
        Graphics_drawLineV(&hal_p->g_sContext, 64, 29, 4);
    }
  Graphics_setForegroundColor(&hal_p->g_sContext, GRAPHICS_COLOR_LIGHT_BLUE);
}

//This function deselects an ingredient when the joystick is away from it
void DontHighlightIngredient(HAL *hal_p, Application *app_p)
{
    if (app_p->o != 0 || app_p->o == 8)
    {
        Graphics_setForegroundColor(&hal_p->g_sContext, GRAPHICS_COLOR_LIGHT_BLUE);
        Graphics_drawLineV(&hal_p->g_sContext, 64, 29, 4);
        Graphics_drawLine(&hal_p->g_sContext, 88.74, 39.26, 106.43, 21.57);
    }
   if (app_p->o != 1 || app_p->o == 8)
    {
        Graphics_setForegroundColor(&hal_p->g_sContext, GRAPHICS_COLOR_LIGHT_BLUE);
        Graphics_drawLine(&hal_p->g_sContext, 88.74, 39.26, 106.43, 21.57);
        Graphics_drawLineH(&hal_p->g_sContext, 99, 124, 64);
    }
    if (app_p->o != 2 ||app_p->o == 8)
    {
        Graphics_setForegroundColor(&hal_p->g_sContext, GRAPHICS_COLOR_LIGHT_BLUE);
        Graphics_drawLine(&hal_p->g_sContext, 88.74, 88.74, 106.43, 106.43);
        Graphics_drawLineH(&hal_p->g_sContext, 99, 124, 64);
    }
   if (app_p->o != 3 || app_p->o== 8)
    {
        Graphics_setForegroundColor(&hal_p->g_sContext, GRAPHICS_COLOR_LIGHT_BLUE);
        Graphics_drawLine(&hal_p->g_sContext, 88.74, 88.74, 106.43, 106.43);
        Graphics_drawLineV(&hal_p->g_sContext, 64, 99, 124);
    }
   if (app_p->o != 4 || app_p->o == 8)
    {
        Graphics_setForegroundColor(&hal_p->g_sContext, GRAPHICS_COLOR_LIGHT_BLUE);
        Graphics_drawLine(&hal_p->g_sContext, 39.26, 88.74, 21.57, 106.43);
        Graphics_drawLineV(&hal_p->g_sContext, 64, 99, 124);
    }
   if (app_p->o != 5 ||app_p->o == 8)
    {
        Graphics_setForegroundColor(&hal_p->g_sContext, GRAPHICS_COLOR_LIGHT_BLUE);
        Graphics_drawLine(&hal_p->g_sContext, 39.26, 88.74, 21.57, 106.43);
        Graphics_drawLineH(&hal_p->g_sContext, 29, 4, 64);
    }
   if (app_p->o != 6 || app_p->o == 8)
    {
        Graphics_setForegroundColor(&hal_p->g_sContext, GRAPHICS_COLOR_LIGHT_BLUE);
        Graphics_drawLine(&hal_p->g_sContext, 39.26, 39.26, 21.57, 21.57);
        Graphics_drawLineH(&hal_p->g_sContext, 29, 4, 64);
    }
   if (app_p->o != 7 || app_p->o == 8)
    {
        Graphics_setForegroundColor(&hal_p->g_sContext, GRAPHICS_COLOR_LIGHT_BLUE);
        Graphics_drawLine(&hal_p->g_sContext, 39.26, 39.26, 21.57, 21.57);
        Graphics_drawLineV(&hal_p->g_sContext, 64, 29, 4);
    }
}
//This function checks the plate submitted by comparing the array of user to ticket array. If they match
//then score increses by 5 if they don't match a strike is drawn on the screen. Number of strikes is incremented
// if player receives 3 strikes game ends and all values are resetted
void CheckOrder(HAL *hal_p, Application *app_p)
{
    int i;
    int grade = 0;
    for (i = 0; i < NUM_OF_INGREDIENTS; i++)
    {
        if (app_p->user_array[i] != app_p->tickets[app_p->r][i])
        {
            grade = NULL_STATE;
            i = NUM_OF_INGREDIENTS;
        }
        else
        {
            grade = SUCCESS_STATE;
        }
    }

    if (grade == SUCCESS_STATE)
    {
        app_p->score_int += SCORE_ADDITION;
        sprintf(app_p->score_char,"%d", app_p->score_int);
        Graphics_drawString(&hal_p->g_sContext, (int8_t*) app_p->score_char, -1, 6, 0, true);

    }
    else if(grade == NULL_STATE)
    {
        Graphics_drawString(&hal_p->g_sContext, (int8_t*) "x", -1,
                            0 + (app_p->strikes * 5), 7, true);
        app_p->strikes++;
        if (app_p->strikes == TWO_STRIKES)
        {
            Graphics_drawString(&hal_p->g_sContext, (int8_t*) "x", -1, 0, 7,
                                true);
        }
    }

    if (app_p->strikes == NUM_OF_SELECTIONS)
    {
        if (app_p->score_int != 0)
        {
            HandleHighScores(hal_p, app_p);
            app_p->state = WIN_SCREEN;
            ShowWinScreen(hal_p, app_p);
            app_p->counter = GAME_TIME;
            app_p->FirstTimeCondition = 1;
            app_p->score_int = 0;

            for(app_p->index = 0; app_p->index < 8; app_p->index++)
                    {
                     app_p->user_array[app_p->index] = 0;
                    }
        }
        else
        {
            app_p->state = LOSE_SCREEN;
            ShowLoseScreen(hal_p, app_p);
            app_p->counter = GAME_TIME;
            app_p->FirstTimeCondition = 1;
            app_p->score_int = 0;
           for(app_p->index = 0; app_p->index < NUM_OF_INGREDIENTS; app_p->index++)
           {
            app_p->user_array[app_p->index] = 0;
           }


        }
    }
}

//hanlding the win screen. Resetting all variables to initial values and when bbs1 is pressed screen changes
// to menu screen
void HandleWinScreen(HAL* hal_p, Application* app_p)
{
    if (Button_isTapped(&hal_p->boosterpackJS))
       {
           ShowMenuScreen(hal_p, app_p);
           app_p->FirstTimeCondition = 1;
           app_p->strikes = 0;
           app_p->state = MENU_SCREEN;
           app_p->score_int = 0;
       }
}

//hanlding the lose screen. Resetting all variables to initial values and when bbjs is pressed screen changes
// to menu screen
void HandleLoseScreen(HAL* hal_p, Application* app_p)
{
    if (Button_isTapped(&hal_p->boosterpackJS))
       {
           ShowMenuScreen(hal_p, app_p);
           app_p->FirstTimeCondition = 1;
           app_p->strikes = 0;
           app_p->state = MENU_SCREEN;
           app_p->score_int = 0;

       }
}

//handling highscore screen. when bbjs is pressed screen changes to menu screen
void HandleHighScoresScreen(HAL* hal_p, Application *app_p)
{
    if (Button_isTapped(&hal_p->boosterpackJS))
    {
        ShowMenuScreen(hal_p, app_p);
        app_p->state = MENU_SCREEN;
    }
}
