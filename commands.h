#pragma once
#include "common.h"
#include "levels.h"

class Command
{
public:
	virtual void Execute(Level& levelData) = 0;
	virtual void Undo(Level& levelData) = 0;
	virtual ~Command() = default;
};

// move command
class MoveCommand : public Command
{
private:
	Vector2 direction;
	Vector2 previousPlayerPosition;
	int pushedBoxIndex = -1;
	Vector2 previousBoxPosition;

public:
	MoveCommand(Vector2 dir) : direction(dir) {}

	void Execute(Level& levelData) override
	{
        previousPlayerPosition = levelData.playerPosition;

        Vector2 nextPosition = { levelData.playerPosition.x + direction.x, levelData.playerPosition.y + direction.y };
        Vector2 furtherNextPosition = { levelData.playerPosition.x + direction.x * 2, levelData.playerPosition.y + direction.y * 2 };

        // check that the tile the player is trying to move into is not a wall
        MapTile nextBlock = levelData.levelBase.Get(nextPosition.x, nextPosition.y);
        if (nextBlock == MapTile::WALL) { return; }

        // check if the player is attempting to push a box
        pushedBoxIndex = getBoxIndexAtPosition(levelData, nextPosition);
        if (pushedBoxIndex != -1) {
            // make sure the box is not being pushed into another box
            int furtherBoxIndex = getBoxIndexAtPosition(levelData, furtherNextPosition);
            if (furtherBoxIndex != -1) { return; }

            // make sure the box is not being pushed into a wall
            MapTile furtherNextBlock = levelData.levelBase.Get(furtherNextPosition.x, furtherNextPosition.y);
            if (furtherNextBlock == MapTile::WALL) { return; }

            // update the box position
            previousBoxPosition = levelData.boxes[pushedBoxIndex].position;
            levelData.boxes[pushedBoxIndex].position = furtherNextPosition;
        }

        // update the player position
        levelData.playerPosition = nextPosition;
        levelData.moveCount++;
	}
    
    void Undo(Level& levelData) override
    {
        levelData.playerPosition = previousPlayerPosition;

        if (pushedBoxIndex != -1)
        {
            levelData.boxes[pushedBoxIndex].position = previousBoxPosition;
        }

        levelData.moveCount--;
    }
};

// reset command
class ResetCommand : public Command
{
private:
    Level* previousLevelData;

public:
    ResetCommand() : previousLevelData(nullptr) {};

    void Execute(Level& levelData) override
    {
        previousLevelData = new Level(levelData);
        levelData = LoadLevel(levelData.levelNum);
    }

    void Undo(Level& levelData) override
    {
        levelData = *previousLevelData;

        delete previousLevelData;
        previousLevelData = nullptr;
    }
};