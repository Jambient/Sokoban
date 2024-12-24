#pragma once
#include <vector>
#include "common.h"

/**
* Resize the current screen to the specified width and height.
*
* @param width - new screen width.
* @param height - new screen height.
*/
void resizeScreenRender(int width, int height);

/**
Clears the screen.
*/
void clearScreen();

/**
Clears any data about the past screen renders used for optimisation.
This should be run if the whole screen needs to be re-rendered
*/
void resetRendering();

/**
* Renders level data onto the screen.
*
* @param levelData - The level data to render.
* @param updateScreen - Specifies if the screen should update after this function.
*/
void renderLevel(Level levelData, bool updateScreen = true);

/**
Updates the screen to the current render.
*/
void updateScreenRender();

/**
* Change a pixel on the current render to contain specified data.
*
* @param color - The color of the pixel.
* @param position - The position of the pixel to change.
* @param content - A 2 character string displayed inside the pixel.
*/
void forcePixelChange(Color color, Vector2 position, std::string content);

/**
* Displayes the given UI onto the current render.
*
* @param lines - A vector of strings representing the lines of the UI.
*/
void showUI(std::vector<std::string>& lines);

/**
* Renders lighting based on the given parameters.
*
* @param lightPosition - Where the light should originate from.
* @param lightStrength - How strong the light should be (the radius of the light).
*/
void renderLighting(Vector2 lightPosition, int lightStrength);

/**
* Applies box blur to the current render with the given strength.
*
* @param strength - The strength of the box blur.
*/
void applyBlur(int strength);

/**
* Display the number of moves onto the screen.
*
* @param levelData - The level data of the current level.
* @param moves - The number of moves to display.
*/
void showMoves(Level& levelData);

/**
* Renders level status pixels that show the current state of a level by a color.
*
* @param levelData - The level data of the level screen.
* @param currentLevelUnlock - The highest unlocked level by the user.
*/
void renderLevelStatusPixels(Level levelData, int currentLevelUnlock);