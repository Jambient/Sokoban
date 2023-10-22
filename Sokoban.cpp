#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>
#include "main.h"
#include "blocks.h"
#include "rendering.h"
#include <vector>

using namespace std;

// 2d map array of walls and floors
// floodfill map array to generate floors
// 
// array for pushable blocks

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
    string filePath = "Levels/Level" + to_string(levelNumber) + ".txt";
    string textLine;
    ifstream rawLevelData(filePath);

    // get map height and width to create map
    int mapWidth = 0;
    int mapHeight = 0;

    for (mapHeight = 0; getline(rawLevelData, textLine); mapHeight++) {
        if (textLine.size() > mapWidth) {
            mapWidth = textLine.size();
        }
    }

    Map levelBase(mapWidth, mapHeight);
    vector<PushableBox> boxes;
    Vector2 playerPosition;
    vector<Vector2> goals;

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
                playerPosition = {i, currentLineIndex};
            }
            else if (symbol == '$') {
                boxes.push_back({ {i, currentLineIndex} });
            }
            else if (symbol == '.') {
                goals.push_back({ i, currentLineIndex });
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

void handleMovement(Level& levelData, Vector2 direction) {
    Vector2 nextPosition = { levelData.playerPosition.x + direction.x, levelData.playerPosition.y + direction.y };
    Vector2 furtherNextPosition = { levelData.playerPosition.x + direction.x * 2, levelData.playerPosition.y + direction.y * 2 };

    MapTile nextBlock = levelData.levelBase.Get(nextPosition.x, nextPosition.y);
    if (nextBlock == MapTile::WALL) { return; }

    int boxIndex = getBoxIndexAtPosition(levelData, nextPosition);
    if (boxIndex != -1) {
        int furtherBoxIndex = getBoxIndexAtPosition(levelData, furtherNextPosition);
        if (furtherBoxIndex != -1) { return; }

        MapTile furtherNextBlock = levelData.levelBase.Get(furtherNextPosition.x, furtherNextPosition.y);
        if (furtherNextBlock == MapTile::WALL) { return; }

        levelData.boxes[boxIndex].position = furtherNextPosition;
    }

    levelData.playerPosition = nextPosition;
}

bool isLevelSolved(Level levelData) {
    bool isSolved = true;
    for (Vector2 goalPos : levelData.goals) {
        int furtherBoxIndex = getBoxIndexAtPosition(levelData, goalPos);
        if (furtherBoxIndex == -1) { isSolved = false; }
    }
    return isSolved;
}

int main()
{
    initialiseBlocks();

    Level levelData = LoadLevel(1);

    Vector2 previousPlayerPosition = levelData.playerPosition;
    bool wasKeyPressed = false;

    // show level menu
    for (int i = 0; i < 7; i++) {
        Vector2 iconPosition = { (i < 4 ? 1 : 5), (i % 4) * 6 + 1 };
        cout << "\033[" << iconPosition.x << ";" << iconPosition.y << "H";
        cout << "*---*";
        cout << "\033[" << iconPosition.x + 1 << ";" << iconPosition.y << "H";
        cout << "| " << i + 1 << " |";
        cout << "\033[" << iconPosition.x + 2 << ";" << iconPosition.y << "H";
        cout << "*---*";
    }

    Sleep(2000);

    clearScreen();
    renderScreen(levelData);

    while (!isLevelSolved(levelData)) {
        if (!(levelData.playerPosition.x == previousPlayerPosition.x && levelData.playerPosition.y == previousPlayerPosition.y)) {
            renderScreen(levelData);
        }

        if (wasKeyPressed) {
            Sleep(150);
            wasKeyPressed = false;
        }

        previousPlayerPosition.x = levelData.playerPosition.x;
        previousPlayerPosition.y = levelData.playerPosition.y;

        if ((GetKeyState('W') & 0x8000)) {
            handleMovement(levelData, { 0, -1 });
            wasKeyPressed = true;
        } else if ((GetKeyState('S') & 0x8000)) {
            handleMovement(levelData, { 0, 1 });
            wasKeyPressed = true;
        } else if ((GetKeyState('A') & 0x8000)) {
            handleMovement(levelData, { -1, 0 });
            wasKeyPressed = true;
        } else if ((GetKeyState('D') & 0x8000)) {
            handleMovement(levelData, { 1, 0 });
            wasKeyPressed = true;
        }
        else if ((GetKeyState('R') & 0x8000)) { // reset level
            levelData = LoadLevel(1);
            wasKeyPressed = true;
        }
        else if ((GetKeyState('F') & 0x8000)) { // fix button incase rendering breaks (when resizing screen)
            fixRendering();
            renderScreen(levelData);
            wasKeyPressed = true;
        }
    }

    renderScreen(levelData);
    Sleep(2000);
    clearScreen();
    cout << "LEVEL COMPLETE!";
}
