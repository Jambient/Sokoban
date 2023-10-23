#include <string>
#include "main.h"
#include "ansi.h"

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

std::string moveCursorToPosition(Vector2 position) {
	return "\033[" + std::to_string(position.y) + ";" + std::to_string(position.x) + "H";
}