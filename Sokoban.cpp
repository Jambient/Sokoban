#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>
#include "main.h"
#include "blocks.h"
#include "rendering.h"
#include "ansi.h"
#include "levels.h"
#include "ascii.h"
#include <vector>

using namespace std;

int currentLevelUnlock = 0;

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

int showLevelMenu() {
    Level levelData = LoadLevel(0);
    bool hasChosenLevel = false;
    int chosenLevelNum = 0;
    Vector2 previousPlayerPosition = levelData.playerPosition;
    bool wasKeyPressed = false;
    bool isRenderingLighting = false;

    renderLevel(levelData, false);
    renderLevelStatusPixels(levelData, currentLevelUnlock);
    updateScreenRender();

    while (!hasChosenLevel) {
        if (!(levelData.playerPosition.x == previousPlayerPosition.x && levelData.playerPosition.y == previousPlayerPosition.y)) {
            renderLevel(levelData, false);
            renderLevelStatusPixels(levelData, currentLevelUnlock);
            if (isRenderingLighting) {
                renderLighting({ levelData.playerPosition.x * blockTextureSize + 2, levelData.playerPosition.y * blockTextureSize + 2 }, 20);
            }

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

        wasKeyPressed = true;
        if ((GetKeyState('W') & 0x8000)) {
            handleMovement(levelData, { 0, -1 });
        }
        else if ((GetKeyState('S') & 0x8000)) {
            handleMovement(levelData, { 0, 1 });
        }
        else if ((GetKeyState('A') & 0x8000)) {
            handleMovement(levelData, { -1, 0 });
        }
        else if ((GetKeyState('D') & 0x8000)) {
            handleMovement(levelData, { 1, 0 });
        }
        else if ((GetKeyState('F') & 0x8000)) { // fix button incase rendering breaks (when resizing screen)
            resetRendering();
            renderLevel(levelData, false);
            renderLevelStatusPixels(levelData, currentLevelUnlock);
            updateScreenRender();
            isRenderingLighting = false;
        }
        else if ((GetKeyState('L') & 0x8000)) {
            isRenderingLighting = !isRenderingLighting;
            previousPlayerPosition.x--;
        }
        else if ((GetKeyState(VK_RETURN) & 0x8000)) {
            if (levelData.playerPosition.x % 2 == 0 && levelData.playerPosition.y % 2 == 0 && chosenLevelNum <= currentLevelUnlock + 1) {
                hasChosenLevel = true;
            }
        }
        else {
            wasKeyPressed = false;
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
    bool isRenderingLighting = false;
    
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
                renderLevel(levelData, false);
                if (isRenderingLighting) {
                    renderLighting({ levelData.playerPosition.x * blockTextureSize + 2, levelData.playerPosition.y * blockTextureSize + 2 }, 20);
                }
                updateScreenRender();

                moves += 1;
                showMoves(levelData, moves);
            }

            if (wasKeyPressed) {
                Sleep(150);
                wasKeyPressed = false;
            }

            previousPlayerPosition.x = levelData.playerPosition.x;
            previousPlayerPosition.y = levelData.playerPosition.y;

            wasKeyPressed = true;
            if ((GetKeyState('W') & 0x8000)) {
                handleMovement(levelData, { 0, -1 });
            }
            else if ((GetKeyState('S') & 0x8000)) {
                handleMovement(levelData, { 0, 1 });
            }
            else if ((GetKeyState('A') & 0x8000)) {
                handleMovement(levelData, { -1, 0 });
            }
            else if ((GetKeyState('D') & 0x8000)) {
                handleMovement(levelData, { 1, 0 });
            }
            else if ((GetKeyState('R') & 0x8000)) { // reset level
                levelData = LoadLevel(levelNum);
                moves = 0;
                renderLevel(levelData, true);
                showMoves(levelData, moves);
                previousPlayerPosition.x = levelData.playerPosition.x;
                previousPlayerPosition.y = levelData.playerPosition.y;
                isRenderingLighting = false;
            }
            else if ((GetKeyState('F') & 0x8000)) { // fix button incase rendering breaks (when resizing screen)
                resetRendering();
                renderLevel(levelData);
                showMoves(levelData, moves);
                isRenderingLighting = false;
            }
            else if ((GetKeyState('L') & 0x8000)) {
                isRenderingLighting = !isRenderingLighting;
                renderLevel(levelData, false);
                if (isRenderingLighting) {
                    renderLighting({ levelData.playerPosition.x * blockTextureSize + 2, levelData.playerPosition.y * blockTextureSize + 2 }, 20);
                }
                updateScreenRender();
            }
            else if ((GetKeyState(VK_BACK) & 0x8000)) { // leave level
                hasQuitLevel = true;
            }
            else {
                wasKeyPressed = false;
            }
        }

        if (!hasQuitLevel) {
            renderLevel(levelData, false);
            if (isRenderingLighting) {
                renderLighting({ levelData.playerPosition.x * blockTextureSize + 2, levelData.playerPosition.y * blockTextureSize + 2 }, 20);
            }

            for (int y = 0; y < levelData.levelBase.GetHeight() * blockTextureSize; y++) {
                for (int x = 0; x < levelData.levelBase.GetWidth() * blockTextureSize; x++) {
                    if (x % 5 == 0) {
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

            showUI(levelCompleteUI);
            Sleep(3000);
        }

        resetRendering();
        clearScreen();
    }
}
