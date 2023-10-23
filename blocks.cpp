#include "blocks.h"

Block blocks[6];

void initialiseBlocks() {
	Block blockData;

	// set up colors
	Color BLANK = { -1, -1, -1 };
	Color BRICK_RED = { 170, 74, 68 };
	Color DARK_BRICK_RED = { 160, 64, 58 };
	Color DARK_GRAY = { 30, 30, 30 };
	Color WHITE = { 255, 255, 255 };
	Color BROWN = { 150, 75, 0 };
	Color DARK_BROWN = { 130, 55, 0 };
	Color GREEN = { 0, 100, 0 };

	// empty
	blockData.texture = {
		{BLANK, BLANK, BLANK, BLANK, BLANK},
		{BLANK, BLANK, BLANK, BLANK, BLANK},
		{BLANK, BLANK, BLANK, BLANK, BLANK},
		{BLANK, BLANK, BLANK, BLANK, BLANK},
		{BLANK, BLANK, BLANK, BLANK, BLANK}
	};
	blocks[0] = blockData;
	blockData = Block();

	// wall
	blockData.texture = {
		{DARK_BRICK_RED, DARK_BRICK_RED, DARK_BRICK_RED, DARK_BRICK_RED, DARK_BRICK_RED},
		{DARK_BRICK_RED, BRICK_RED, BRICK_RED, BRICK_RED, DARK_BRICK_RED},
		{DARK_BRICK_RED, BRICK_RED, BRICK_RED, BRICK_RED, DARK_BRICK_RED},
		{DARK_BRICK_RED, BRICK_RED, BRICK_RED, BRICK_RED, DARK_BRICK_RED},
		{DARK_BRICK_RED, DARK_BRICK_RED, DARK_BRICK_RED, DARK_BRICK_RED, DARK_BRICK_RED}
	};
	blocks[1] = blockData;
	blockData = Block();

	// floor
	blockData.texture = {
		{DARK_GRAY, DARK_GRAY, DARK_GRAY, DARK_GRAY, DARK_GRAY},
		{DARK_GRAY, DARK_GRAY, DARK_GRAY, DARK_GRAY, DARK_GRAY},
		{DARK_GRAY, DARK_GRAY, DARK_GRAY, DARK_GRAY, DARK_GRAY},
		{DARK_GRAY, DARK_GRAY, DARK_GRAY, DARK_GRAY, DARK_GRAY},
		{DARK_GRAY, DARK_GRAY, DARK_GRAY, DARK_GRAY, DARK_GRAY}
	};
	blocks[2] = blockData;
	blockData = Block();

	// player
	blockData.texture = {
		{BLANK, WHITE, WHITE, WHITE, BLANK},
		{WHITE, WHITE, WHITE, WHITE, WHITE},
		{WHITE, WHITE, WHITE, WHITE, WHITE},
		{WHITE, WHITE, WHITE, WHITE, WHITE},
		{BLANK, WHITE, WHITE, WHITE, BLANK}
	};
	blocks[3] = blockData;
	blockData = Block();

	// box
	blockData.texture = {
		{DARK_BROWN, DARK_BROWN, DARK_BROWN, DARK_BROWN, DARK_BROWN},
		{DARK_BROWN, BROWN, BROWN, DARK_BROWN, DARK_BROWN},
		{DARK_BROWN, BROWN, DARK_BROWN, BROWN, DARK_BROWN},
		{DARK_BROWN, DARK_BROWN, BROWN, BROWN, DARK_BROWN},
		{DARK_BROWN, DARK_BROWN, DARK_BROWN, DARK_BROWN, DARK_BROWN}
	};
	blocks[4] = blockData;
	blockData = Block();

	// goal
	blockData.texture = {
		{BLANK, BLANK, BLANK, BLANK, BLANK},
		{BLANK, BLANK, BLANK, BLANK, BLANK},
		{BLANK, BLANK, GREEN, BLANK, BLANK},
		{BLANK, BLANK, BLANK, BLANK, BLANK},
		{BLANK, BLANK, BLANK, BLANK, BLANK}
	};
	blocks[5] = blockData;
	blockData = Block();
}

Block getBlockData(BlockName block) {
	int blockIndex = (int)block;
	return blocks[blockIndex];
}