#pragma once
#include <string>
#include "main.h"

enum class ColorLayer
{
	foreground,
	background
};

std::string getResetModifier();

std::string colorToAnsi(Color color, ColorLayer layer);

std::string moveCursorToPosition(Vector2 position);