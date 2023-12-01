#include <string>
#include <fstream>
#include <vector>
#include "common.h"
#include "levels.h"
#include "rendering.h"

/**
* Recursively flood fills a level starting from the given position.
*
* @param level - Map data for the level.
* @param position - The starting position.
*/
void FloodFillRecursive(Map level, Vector2 position) {
    if (level.Get(position.x, position.y) != MapTile::EMPTY) { return; }
    level.Set(position.x, position.y, MapTile::FLOOR);

    FloodFillRecursive(level, { position.x + 1, position.y });
    FloodFillRecursive(level, { position.x - 1, position.y });
    FloodFillRecursive(level, { position.x, position.y + 1 });
    FloodFillRecursive(level, { position.x, position.y - 1 });
}

Level LoadLevel(int levelNumber)
{
    // load raw level data from text file
    std::string filePath = "Levels/Level" + std::to_string(levelNumber) + ".txt";
    std::string textLine;
    std::ifstream rawLevelData(filePath);

    // get map height and width to create map
    int mapWidth = 0;
    int mapHeight = 0;

    for (mapHeight = 0; std::getline(rawLevelData, textLine); mapHeight++) {
        if (textLine.size() > mapWidth) {
            mapWidth = textLine.size();
        }
    }

    Map levelBase(mapWidth, mapHeight);
    std::vector<PushableBox> boxes;
    Vector2 playerPosition;
    std::vector<Vector2> goals;

    // reset getline() function
    rawLevelData.clear();
    rawLevelData.seekg(0);

    // create level data
    int currentLineIndex = 0;
    while (getline(rawLevelData, textLine)) {
        for (int i = 0; i < textLine.size(); i++) {
            char symbol = textLine[i];
            if (symbol == '#') {
                levelBase.Set(i, currentLineIndex, MapTile::WALL);
            }
            else if (symbol == '@') {
                playerPosition = { i, currentLineIndex };
            }
            else if (symbol == '$') {
                boxes.push_back({ {i, currentLineIndex} });
            }
            else if (symbol == '.') {
                goals.push_back({ i, currentLineIndex });
            }
            else if (symbol == '*') {
                goals.push_back({ i, currentLineIndex });
                boxes.push_back({ {i, currentLineIndex} });
            }
            else if (symbol == '+') {
                goals.push_back({ i, currentLineIndex });
                playerPosition = { i, currentLineIndex };
            }
        }

        currentLineIndex++;
    }

    // generate floor tiles for the map
    FloodFillRecursive(levelBase, playerPosition);

    // close file
    rawLevelData.close();

    // update screen render
    resizeScreenRender(levelBase.GetWidth(), levelBase.GetHeight());

    // build level data
    Level levelData(levelBase, boxes, goals, playerPosition);

    return levelData;
}

int getBoxIndexAtPosition(Level levelData, Vector2 position) {
    for (int i = 0; i < levelData.boxes.size(); i++) {
        PushableBox box = levelData.boxes[i];
        if (box.position.x == position.x && box.position.y == position.y) {
            return i;
        }
    }

    return -1;
}

bool isLevelSolved(Level levelData) {
    bool isSolved = true;
    for (Vector2 goalPos : levelData.goals) {
        int furtherBoxIndex = getBoxIndexAtPosition(levelData, goalPos);
        if (furtherBoxIndex == -1) { isSolved = false; }
    }
    return isSolved;
}