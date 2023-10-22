#pragma once
#include <string>
#include "main.h"

enum class ColorLayer
{
	foreground,
	background
};

std::string getResetModifier() {
	return "\033[0;00m";
}

std::string colorToAnsi(Color color, ColorLayer layer) {
	if (color.r < 0) { return getResetModifier(); }

	std::string start = "\033[48;2;";
	if (layer == ColorLayer::foreground) {
		start = "\033[38;2;";
	}

	return start + std::to_string(color.r) + ";" + std::to_string(color.g) + ";" + std::to_string(color.b) + "m";
}