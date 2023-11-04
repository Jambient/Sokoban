#pragma once
#include <vector>
#include "main.h"

extern std::vector<std::string> levelCompleteUI;

void showStartScreen();

int printNumber(int num, Vector2 position);

std::string* numberToAscii(int num);