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
	Map() {
		_mapWidth = 1;
		_mapHeight = 1;

		world = new MapTile[1]();
	}

	Map(int width, int height) {
		_mapWidth = width;
		_mapHeight = height;

		world = new MapTile[width * height]();
		//for (int i = 0; i < width * height; i++) {
		//	/*if (i == 2) {
		//		world[i] = MapTile::FLOOR;
		//	} else {
		//		world[i] = MapTile::EMPTY;
		//	}*/

		//	world[i] = MapTile::FLOOR;
		//}
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

struct PushableBlock {
	Vector2 position;
};

struct Level {
	Map levelBase;
	std::vector<PushableBlock> blocks;

	Level(Map levelBase_, std::vector<PushableBlock> blocks_) {
		levelBase = levelBase_;
		blocks = blocks_;
	}
};