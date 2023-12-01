#pragma once

#include "common.h"
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

/**
Create and store all the blocks for the game.
*/
void initialiseBlocks();

/**
* Get the block data for a specific block.
*
* @param block - The identifier of the block.
* @return The blocks data.
*/
Block getBlockData(BlockName block);