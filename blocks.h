#pragma once

#include "main.h"
#include <vector>

enum class BlockName
{
	EMPTY,
	WALL,
	FLOOR,
	PLAYER,
	BOX,
	GOAL,
};

struct Block 
{
	std::vector<std::vector<Color>> texture;
};

const int blockTextureSize = 5;

void initialiseBlocks();
Block getBlockData(BlockName block);