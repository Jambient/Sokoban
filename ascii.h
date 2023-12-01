#pragma once
#include <vector>
#include "common.h"

extern std::vector<std::string> levelCompleteUI;

/**
Displays the welcome screen shown at the start.
*/
void showStartScreen();

/**
* Outputs an ASCII digit to the screen at the specified position.
*
* @param digit - The single digit to output.
* @param position - The position where the digit will appear.
* @return The width of the ASCII digit.
*/
int printDigit(int digit, Vector2 position);

/**
* Returns an ASCII number as an array.
*
* @param levelData - The data for the current level.
* @return An array containing the ASCII number.
*/
std::string* numberToAscii(int num);