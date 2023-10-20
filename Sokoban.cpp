#include <iostream>
#include <fstream>
#include <string>
#include "main.h"
#include <vector>

using namespace std;

// 2d map array of walls and floors
// floodfill map array to generate floors
// 
// array for pushable blocks

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
    std::vector<PushableBlock> blocks;

    // reset getline() function
    rawLevelData.clear();
    rawLevelData.seekg(0);

    int currentLineIndex = 0;
    while (getline(rawLevelData, textLine)) {
        for (string::size_type i = 0; i < textLine.size(); i++) {
            if (textLine[i] == '#') {
                levelBase.Set(i, currentLineIndex, MapTile::WALL);
            }
        }

        currentLineIndex++;
    }

    // close file
    rawLevelData.close();

    Level levelData(levelBase, blocks);


    // printing map
    /*for (int y = 0; y < levelBase.GetHeight(); y++) {
        for (int x = 0; x < levelBase.GetWidth(); x++) {
            MapTile tile = levelBase.Get(x, y);
            if (tile == MapTile::EMPTY) {
                cout << ' ';
            }
            else if (tile == MapTile::WALL) {
                cout << '#';
            }
        }
        cout << endl;
    }*/

    return levelData;
}

int main()
{
    Level levelData = LoadLevel(1);
    Map levelBase = levelData.levelBase;

    const int blockTextureSize = 5;
    vector<vector<string>> screenRender;

    screenRender.resize((levelBase.GetHeight() + 1) * blockTextureSize, std::vector<string>((levelBase.GetWidth() + 1) * blockTextureSize, "  "));

    for (int y = 0; y < levelBase.GetHeight(); y++) {
        for (int x = 0; x < levelBase.GetWidth(); x++) {
            MapTile tile = levelBase.Get(x, y);
            string tileChar = "  ";

            if (tile == MapTile::EMPTY) {
                tileChar = "  ";
            }
            else if (tile == MapTile::WALL) {
                tileChar = "##";
            }

            for (int i = 0; i < blockTextureSize; i++) {
                for (int j = 0; j < blockTextureSize; j++) {
                    screenRender[y * blockTextureSize + i][x * blockTextureSize + j] = tileChar;
                }
            }
        }
    }

    for (int y = 0; y < levelBase.GetHeight() * blockTextureSize; y++) {
        for (int x = 0; x < levelBase.GetWidth() * blockTextureSize; x++) {
            cout << screenRender[y][x];
        }
        cout << endl;
    }


}
