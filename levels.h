#pragma once
#include "common.h"

/**
* Load a specified level from a text file.
*
* @param levelNumber - The number of the level.
* @return Level data about the level.
*/
Level LoadLevel(int levelNumber);

/**
* Get the array index of the box at a given position.
*
* @param levelData - The data for the level.
* @param position - The position in the level to check.
* @return Index in levelData.boxes array or -1 if no box exists.
*/
int getBoxIndexAtPosition(Level levelData, Vector2 position);

/**
* Check if a given level is solved.
*
* @param levelData - The level data to check.
* @return True if all boxes are on a goal point, otherwise False.
*/
bool isLevelSolved(Level levelData);