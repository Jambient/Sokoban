#include <iostream>
#include <fstream>
#include <string>
#include "main.h"

using namespace std;

// 2d map array of walls and floors
// floodfill map array to generate floors
// 
// array for pushable blocks

void LoadLevel(int levelNumber) 
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

    // reset getline()
    rawLevelData.clear();
    rawLevelData.seekg(0);

    cout << mapHeight << endl << mapWidth << endl;

    int currentLineIndex = 0;
    while (getline(rawLevelData, textLine)) {
        for (string::size_type i = 0; i < textLine.size(); i++) {
            if (textLine[i] == '#') {
                levelBase.Set(i, currentLineIndex, MapTile::WALL);
            }
        }

        cout << textLine << endl;

        currentLineIndex++;
    }

    rawLevelData.close();
}

int main()
{
    LoadLevel(1);
}
