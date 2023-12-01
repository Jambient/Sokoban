#pragma once
#include <string>
#include "common.h"

enum class ColorLayer
{
	foreground,
	background
};

/**
* Get the ANSI reset modifier used to reset changed done to the terminal.
*
* @return ANSI reset modifier.
*/
std::string getResetModifier();

/**
* Get the ANSI modifier to change the color of displayed text.
*
* @param color - The color of the text layer.
* @param layer - The layer the color should affect.
* @return ANSI color modifier.
*/
std::string colorToAnsi(Color color, ColorLayer layer);

/**
* Get the ANSI modifier to move the cursor to a specified location in the terminal.
*
* @param position - The position in the terminal.
* @return ANSI cursor modifier.
*/
std::string moveCursorToPosition(Vector2 position);