/*
 * FUNCTIONS.h
 *
 *  Created on: Mar 30, 2022
 *      Author: Yahia Tawfik
 */
#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_

#include <HAL/HAL.h>
#include <Application.h>

/*
//Function declerations
*/

void ShowTitleScreen (HAL* hal_p, Graphics_Context* graphics_p);
void BusyKitchen_loop(HAL* hal_p, Application* app_p );
void HandleTitleScreen (HAL* hal_p, Application* app_p);
void ShowMenuScreen(HAL*hal_p, Application* app_p);
void HandleMenuScreen(HAL *hal_p, Application *app_p);
bool JoystickIsMiddle(HAL* hal_p, Application* app_p);
bool JoystickIsUp(HAL* hal_p, Application* app_p);
bool JoystickIsDown(HAL* hal_p, Application* app_p);
JSposition ArrowLoop(HAL* hal_p, Application* app_p);
void UpdateArrow(HAL *hal_p, Application *app_p);
void ShowHowToCookScreen(HAL* hal_p, Application* app_p);
void HandleHowToCookScreen(HAL* hal_p, Application* app_p);
void HandleHighScoresScreen(HAL* hal_p, Application* app_p);
void ShowHighScoreScreen(HAL* hal_p, Application* app_p);
void ShowBurgerTimeScreen(HAL* hal_p, Application* app_p);
void HandleBurgerTimeScreen(HAL* hal_p, Application* app_p);
void HandleGameTime(HAL* hal_p, Application* app_p);
void ShowResultScreen(HAL* hal_p, Application* app_p);
void HandleResultScreen(HAL* hal_p, Application* app_p);
void HandleTickets(HAL *hal_p, Application *app_p);
void DrawRectangles_H(HAL* hal_p, Application* app_p);
void DrawRectangles_V(HAL *hal_p, Application *app_p);
void FillSquare(HAL* hal_p, int ing_num, int square_num, Application* app_p);
void GetSquareCoor(int* arr,int ing_num, int square_num, HAL* hal_P, Application* app_p);
int IngredientStates(HAL* hal_p, Application* app_p);
void HighlightIngredient(HAL* hal_p, Application* app_p);
void DontHighlightIngredient(HAL *hal_p, Application *app_p);
void CheckOrder(HAL *hal_p, Application *app_p);
void ShowLoseScreen(HAL *hal_p, Application *app_p);
void ShowWinScreen(HAL* hal_p, Application* app_p);
void HandleWinScreen(HAL* hal_p, Application* app_p);
void HandleLoseScreen(HAL* hal_p, Application* app_p);
void HandleSquares (HAL* hal_p, Application* app_p);
void WhenTimeEnds(HAL* hal_p, Application* app_p);
void DrawSquare(HAL* hal_p, int ing_num, int square_num, Application* app_p);
void HandleHighScores(HAL* hal_p, Application* app_p);
#endif /* FUNCTIONS_H_ */
