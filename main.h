#pragma once
#include <vector>

enum MapTile
{
	EMPTY,
	WALL,
	FLOOR
};

struct Vector2 {
	int x = 0;
	int y = 0;
};

class Map {

private:
	int _mapWidth, _mapHeight;
	MapTile* world;

	size_t index(int x, int y) const { return x + _mapWidth * y; }
public:
	Map(int width, int height) {
		_mapWidth = width;
		_mapHeight = height;

		world = new MapTile[width * height];
		for (int i = 0; i < width * height; i++) {
			world[i] = MapTile::EMPTY;
		}
	}

	MapTile Get(int x, int y) {
		return world[index(x, y)];
	}

	void Set(int x, int y, MapTile value) {
		//world[index(x, y)] = value;
		world[2] = value;
	}

	int GetWidth() {
		return _mapWidth;
	}
	int GetHeight() {
		return _mapHeight;
	}
};

struct PushableBlock {
	Vector2 position;
};

struct Level {
	Map levelBase;
	std::vector<PushableBlock> blocks;
};