#pragma once
#include <vector>
#include "main.h"

void resizeScreenRender(int width, int height);

void clearScreen();

void resetRendering();

void renderLevel(Level levelData, bool updateScreen = true);

void updateScreenRender();

void forcePixelChange(Color color, Vector2 position, std::string content);

void showUI(std::vector<std::string>& lines);

void renderLighting(Vector2 lightPosition, int lightStrength);

void applyBlur(int strength);

void showMoves(Level levelData, int moves);

void renderLevelStatusPixels(Level levelData, int currentLevelUnlock);