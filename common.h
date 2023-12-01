#pragma once
#include <vector>

enum class MapTile
{
	EMPTY,
	WALL,
	FLOOR
};

struct Vector2 {
	int x = 0;
	int y = 0;
};

struct Color {
	int r = 0;
	int g = 0;
	int b = 0;
};

struct UserData {
	int currentLevelUnlock;
};

class Map {
private:
	int _mapWidth, _mapHeight;
	MapTile* world;

	size_t index(int x, int y) const { return x + _mapWidth * y; }
public:
	Map() {
		_mapWidth = 1;
		_mapHeight = 1;

		world = new MapTile[1]();
	}

	Map(int width, int height) {
		_mapWidth = width;
		_mapHeight = height;

		world = new MapTile[width * height]();
	}

	MapTile Get(int x, int y) {
		return world[index(x, y)];
	}

	void Set(int x, int y, MapTile value) {
		world[index(x, y)] = value;
	}

	int GetWidth() {
		return _mapWidth;
	}
	int GetHeight() {
		return _mapHeight;
	}
};

struct PushableBox {
	Vector2 position;
};

struct Level {
	Map levelBase;
	std::vector<PushableBox> boxes;
	Vector2 playerPosition;
	std::vector<Vector2> goals;

	Level(Map levelBase_, std::vector<PushableBox> boxes_, std::vector<Vector2> goals_, Vector2 playerPosition_) {
		levelBase = levelBase_;
		boxes = boxes_;
		playerPosition = playerPosition_;
		goals = goals_;
	}
};