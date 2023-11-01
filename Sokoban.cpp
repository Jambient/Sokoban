#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>
#include "main.h"
#include "blocks.h"
#include "rendering.h"
#include "ansi.h"
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

void showStartScreen() {
    cout << ".----------------.  .----------------.  .----------------.  .----------------.  .----------------.  .----------------.  .-----------------." << endl
        << "| .--------------. || .--------------. || .--------------. || .--------------. || .--------------. || .--------------. || .--------------. |" << endl
        << "| |    _______   | || |     ____     | || |  ___  ____   | || |     ____     | || |   ______     | || |      __      | || | ____  _____  | |" << endl
        << "| |   /  ___  |  | || |   .'    `.   | || | |_  ||_  _|  | || |   .'    `.   | || |  |_   _ \\    | || |     /  \\     | || ||_   \\ |_  _| | |" << endl
        << "| |  |  (__ \\_|  | || |  /  .--.  \\  | || |   | |_/ /    | || |  /  .--.  \\  | || |    | |_) |   | || |    / /\\ \\    | || |  |   \\ | |   | |" << endl
        << "| |   '.___`-.   | || |  | |    | |  | || |   |  __'.    | || |  | |    | |  | || |    |  __'.   | || |   / ____ \\   | || |  | |\\ \\| |   | |" << endl
        << "| |  |`\\____) |  | || |  \\  `--'  /  | || |  _| |  \\ \\_  | || |  \\  `--'  /  | || |   _| |__) |  | || | _/ /    \\ \\_ | || | _| |_\\   |_  | |" << endl
        << "| |  |_______.'  | || |   `.____.'   | || | |____||____| | || |   `.____.'   | || |  |_______/   | || ||____|  |____|| || ||_____|\\____| | |" << endl
        << "| |              | || |              | || |              | || |              | || |              | || |              | || |              | |" << endl
        << "| '--------------' || '--------------' || '--------------' || '--------------' || '--------------' || '--------------' || '--------------' |" << endl
        << " '----------------'  '----------------'  '----------------'  '----------------'  '----------------'  '----------------'  '----------------'" << endl << endl;

    cout << "Press enter to continue...";
    cin.ignore();
}

string asciiNumbers[10][6] = {
    {
        " _____ ",
        "|  _  |",
        "| |/' |",
        "|  /| |",
        "\\ |_/ /",
        " \\___/ "
    },
    {
        " __  ",
        "/  | ",
        "`| | ",
        " | | ",
        "_| |_",
        "\\___/"
    },
    {
        " _____ ",
        "/ __  \\",
        "`' / /'",
        "  / /  ",
        "./ /___",
        "\\_____/"
    },
    {
        " _____ ",
        "|____ |",
        "    / /",
        "    \\ \\",
        ".___/ /",
        "\\____/ "
    },
    {
        "   ___ ",
        "  /   |",
        " / /| |",
        "/ /_| |",
        "\\___  |",
        "    |_/"
    },
    {
        " _____ ",
        "|  ___|",
        "|___ \\ ",
        "    \\ \\",
        "/\\__/ /",
        "\\____/ "
    },
    {
        "  ____ ",
        " / ___|",
        "/ /___ ",
        "| ___ \\",
        "| \\_/ |",
        "\\_____/"
    },
    {
        " ______",
        "|___  /",
        "   / / ",
        "  / /  ",
        "./ /   ",
        "\\_/    "
    },
    {
        " _____ ",
        "|  _  |",
        " \\ V / ",
        " / _ \\ ",
        "| |_| |",
        "\\_____/"
    },
    {
        " _____ ",
        "|  _  |",
        "| |_| |",
        "\\____ |",
        ".___/ /",
        "\\____/ "
    }
};

int currentLevelUnlock = 0;

int printNumber(int num, Vector2 position) {
    int longestLine = 0;
    for (int i = 0; i < 6; i++) {
        string line = asciiNumbers[num][i];
        cout << moveCursorToPosition({ position.x, position.y + i });
        cout << line;
        if (line.size() > longestLine) {
            longestLine = line.size();
        }
    }
    return longestLine;
}

void showMoves(Level levelData, int moves) {
    string numStr = to_string(moves);
    int xPos = 1;
    int yPos = (levelData.levelBase.GetHeight() + 1) * blockTextureSize;

    for (int i = numStr.size(); i < 4; i++) {
        numStr.insert(numStr.begin(), '0');
    }

    for (int y = 0; y < 6; y++) {
        cout << moveCursorToPosition({ xPos, yPos + y });
        cout << string(50, ' ');
    }

    int totalNumLength = 0;
    for (char c : numStr) {
        totalNumLength += printNumber(c - '0', { xPos + totalNumLength + 2, yPos });
    }
    cout << moveCursorToPosition({ xPos, yPos - 1 });
    cout << "NUMBER OF MOVES:";
}

string* numberToAscii(int num) {
    static string ascii[6];
    string numStr = to_string(num);
    int longestLine = 0;

    if (numStr.length() < 2) {
        numStr.insert(numStr.begin(), '0');
    }

    //// get first number longest line
    //for (int i = 0; i < 6; i++) {
    //    int lineLength = asciiNumbers[numStr[0] - '0'][i].length();
    //    if (lineLength > longestLine) {
    //        longestLine = lineLength;
    //    }
    //}

    //// create first number
    //for (int i = 0; i < 6; i++) {
    //    string line = asciiNumbers[numStr[0] - '0'][i];
    //    ascii[i] = line + string((longestLine - line.length()) + 1, ' ');
    //}
    //
    //// create second number
    //for (int i = 0; i < 6; i++) {
    //    string line = asciiNumbers[numStr[1] - '0'][i];
    //    ascii[i] += line;
    //}
    for (int i = 0; i < 6; i++) {
        ascii[i] = asciiNumbers[numStr[0] - '0'][i] + ' ' + asciiNumbers[numStr[1] - '0'][i];
    }

    return ascii;
}

void renderLevelStatusPixels(Level levelData) {
    for (int y = 0; y < 3; y++) {
        for (int x = 0; x < 5; x++) {
            Vector2 pixelPos = { (2 + x * 2) * blockTextureSize + 2, (2 + y * 2) * blockTextureSize + 2 };
            if ((pixelPos.x - 2) / blockTextureSize == levelData.playerPosition.x && (pixelPos.y - 2) / blockTextureSize == levelData.playerPosition.y) {
                continue;
            }
            Color pixelColor = { 0, 200, 0 };
            int levelNum = ((pixelPos.x - 4) / blockTextureSize) / 2 + 1 + 5 * (((pixelPos.y - 4) / blockTextureSize) / 2);
            if (levelNum == currentLevelUnlock + 1) {
                pixelColor = { 50, 50, 255 };
            }
            else  if (levelNum > currentLevelUnlock + 1) {
                pixelColor = { 200, 0, 0 };
            }

            forcePixelChange(pixelColor, pixelPos, "  ");
        }
    }
}

int showLevelMenu() {
    Level levelData = LoadLevel(0);
    bool hasChosenLevel = false;
    int chosenLevelNum = 0;
    Vector2 previousPlayerPosition = levelData.playerPosition;
    bool wasKeyPressed = false;

    renderLevel(levelData, false);
    renderLevelStatusPixels(levelData);
    updateScreenRender();

    while (!hasChosenLevel) {
        if (!(levelData.playerPosition.x == previousPlayerPosition.x && levelData.playerPosition.y == previousPlayerPosition.y)) {
            renderLevel(levelData, false);
            renderLevelStatusPixels(levelData);
            //renderLighting({levelData.playerPosition.x * blockTextureSize + 2, levelData.playerPosition.y * blockTextureSize + 2}, 20);

            if (levelData.playerPosition.x % 2 == 0 && levelData.playerPosition.y % 2 == 0) {
                chosenLevelNum = (levelData.playerPosition.x - 2) / 2 + 1 + 5 * ((levelData.playerPosition.y - 2) / 2);

                string* levelNumAscii = numberToAscii(chosenLevelNum);
                int lineLength = levelNumAscii[0].length() + 1;
                vector<string> levelUI;

                // create level ui
                levelUI.push_back('*' + string(lineLength + 4, '-') + '*');
                levelUI.push_back("|  " + string(lineLength, ' ') + "  |");
                levelUI.push_back("|  " + string((lineLength - 5) / 2, ' ') + "LEVEL" + string((lineLength - 5) - ((lineLength - 5) / 2), ' ') + "  |");
                for (int i = 0; i < 6; i++) {
                    string line = levelNumAscii[i];
                    levelUI.push_back("|  " + line + "   |");
                }
                levelUI.push_back("|  " + string(lineLength, ' ') + "  |");

                string uiText = "COMPLETED";
                if (chosenLevelNum == currentLevelUnlock + 1) {
                    uiText = "UNCOMPLETED";
                }
                else if (chosenLevelNum > currentLevelUnlock + 1) {
                    uiText = "LOCKED";
                }

                levelUI.push_back("|  " + string((lineLength - uiText.length()) / 2, ' ') + uiText + string((lineLength - uiText.length()) - ((lineLength - uiText.length()) / 2), ' ') + "  |");
                levelUI.push_back("|  " + string(lineLength, ' ') + "  |");
                levelUI.push_back('*' + string(lineLength + 4, '-') + '*');

                showUI(levelUI);
            }

            updateScreenRender();
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
        }
        else if ((GetKeyState('S') & 0x8000)) {
            handleMovement(levelData, { 0, 1 });
            wasKeyPressed = true;
        }
        else if ((GetKeyState('A') & 0x8000)) {
            handleMovement(levelData, { -1, 0 });
            wasKeyPressed = true;
        }
        else if ((GetKeyState('D') & 0x8000)) {
            handleMovement(levelData, { 1, 0 });
            wasKeyPressed = true;
        }
        else if ((GetKeyState('F') & 0x8000)) { // fix button incase rendering breaks (when resizing screen)
            resetRendering();
            renderLevel(levelData, false);
            renderLevelStatusPixels(levelData);
            updateScreenRender();
            wasKeyPressed = true;
        }
        else if ((GetKeyState(VK_RETURN) & 0x8000)) {
            if (levelData.playerPosition.x % 2 == 0 && levelData.playerPosition.y % 2 == 0 && chosenLevelNum <= currentLevelUnlock + 1) {
                hasChosenLevel = true;
            }
        }
    }

    return chosenLevelNum;
}

void loadUserData() {
    ifstream userData("UserData.txt");
    if (userData.is_open()) {
        string unlockedLevelsNum;
        getline(userData, unlockedLevelsNum);
        currentLevelUnlock = stoi(unlockedLevelsNum);
    }
    userData.close();
}

int main()
{
    initialiseBlocks();
    loadUserData();

    showStartScreen();
    clearScreen();

    Vector2 previousPlayerPosition;
    bool wasKeyPressed;
    int moves;
    bool hasQuitLevel;
    
    while (true) {
        int chosenLevel = showLevelMenu();

        // play level
        int levelNum = chosenLevel;
        Level levelData = LoadLevel(levelNum);

        previousPlayerPosition = levelData.playerPosition;
        wasKeyPressed = false;
        hasQuitLevel = false;
        moves = 0;

        resetRendering();
        clearScreen();
        renderLevel(levelData);
        showMoves(levelData, moves);

        while (!isLevelSolved(levelData) && !hasQuitLevel) {
            if (!(levelData.playerPosition.x == previousPlayerPosition.x && levelData.playerPosition.y == previousPlayerPosition.y)) {
                renderLevel(levelData);

                moves += 1;
                showMoves(levelData, moves);
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
            }
            else if ((GetKeyState('S') & 0x8000)) {
                handleMovement(levelData, { 0, 1 });
                wasKeyPressed = true;
            }
            else if ((GetKeyState('A') & 0x8000)) {
                handleMovement(levelData, { -1, 0 });
                wasKeyPressed = true;
            }
            else if ((GetKeyState('D') & 0x8000)) {
                handleMovement(levelData, { 1, 0 });
                wasKeyPressed = true;
            }
            else if ((GetKeyState('R') & 0x8000)) { // reset level
                levelData = LoadLevel(levelNum);
                moves = 0;
                renderLevel(levelData);
                showMoves(levelData, moves);
                previousPlayerPosition.x = levelData.playerPosition.x;
                previousPlayerPosition.y = levelData.playerPosition.y;
                wasKeyPressed = true;
            }
            else if ((GetKeyState('F') & 0x8000)) { // fix button incase rendering breaks (when resizing screen)
                resetRendering();
                renderLevel(levelData);
                showMoves(levelData, moves);
                wasKeyPressed = true;
            }
            else if ((GetKeyState(VK_BACK) & 0x8000)) { // leave level
                hasQuitLevel = true;
            }
        }

        if (!hasQuitLevel) {
            renderLevel(levelData);
            for (int y = 0; y < levelData.levelBase.GetHeight() * blockTextureSize; y++) {
                for (int x = 0; x < levelData.levelBase.GetWidth() * blockTextureSize; x++) {
                    if (x % 2 == 0) {
                        Sleep(2);
                    }
                    forcePixelChange({ 1, 1, 1 }, { x, y }, "  ");
                    updateScreenRender();
                }
            }

            currentLevelUnlock = max(currentLevelUnlock, chosenLevel);

            ofstream userData("UserData.txt");
            userData << currentLevelUnlock;
            userData.close();
        }

        resetRendering();
        clearScreen();
    }
}
