#pragma once

Level LoadLevel(int levelNumber);

int getBoxIndexAtPosition(Level levelData, Vector2 position);

bool isLevelSolved(Level levelData);