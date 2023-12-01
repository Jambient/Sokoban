#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>
#include "common.h"
#include "blocks.h"
#include "rendering.h"
#include "ansi.h"
#include "levels.h"
#include "ascii.h"
#include <vector>

using namespace std;

// a global storing the users data
UserData userData;

/**
* Handle the movement of the player based on a given movement direction.
*
* @param levelData The data for the current level.
* @param direction The direction the player should attempt to move in.
*/
void handleMovement(Level& levelData, Vector2 direction) {
    Vector2 nextPosition = { levelData.playerPosition.x + direction.x, levelData.playerPosition.y + direction.y };
    Vector2 furtherNextPosition = { levelData.playerPosition.x + direction.x * 2, levelData.playerPosition.y + direction.y * 2 };

    // check that the tile the player is trying to move into is not a wall
    MapTile nextBlock = levelData.levelBase.Get(nextPosition.x, nextPosition.y);
    if (nextBlock == MapTile::WALL) { return; }

    // check if the player is attempting to push a box
    int boxIndex = getBoxIndexAtPosition(levelData, nextPosition);
    if (boxIndex != -1) {
        // make sure the box is not being pushed into another box
        int furtherBoxIndex = getBoxIndexAtPosition(levelData, furtherNextPosition);
        if (furtherBoxIndex != -1) { return; }

        // make sure the box is not being pushed into a wall
        MapTile furtherNextBlock = levelData.levelBase.Get(furtherNextPosition.x, furtherNextPosition.y);
        if (furtherNextBlock == MapTile::WALL) { return; }

        // update the box position
        levelData.boxes[boxIndex].position = furtherNextPosition;
    }

    // update the player position
    levelData.playerPosition = nextPosition;
}

/**
* Displays the level selection menu.
*
* @return The level number the user chose.
*/
int showLevelMenu() {
    Level levelData = LoadLevel(0);
    bool hasChosenLevel = false;
    int chosenLevelNum = 0;
    Vector2 previousPlayerPosition = levelData.playerPosition;
    bool wasKeyPressed = false;
    bool isRenderingLighting = false;

    // intial screen render
    renderLevel(levelData, false);
    renderLevelStatusPixels(levelData, userData.currentLevelUnlock);
    updateScreenRender();

    // loop until the user has chosen a level
    while (!hasChosenLevel) {
        // check if the players position has changed
        if (!(levelData.playerPosition.x == previousPlayerPosition.x && levelData.playerPosition.y == previousPlayerPosition.y)) {
            renderLevel(levelData, false);
            renderLevelStatusPixels(levelData, userData.currentLevelUnlock);
            if (isRenderingLighting) {
                renderLighting({ levelData.playerPosition.x * blockTextureSize + 2, levelData.playerPosition.y * blockTextureSize + 2 }, 20);
            }

            // check if the player is standing on a level spot
            if (levelData.playerPosition.x % 2 == 0 && levelData.playerPosition.y % 2 == 0) {
                // get the level number they are standing on
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
                if (chosenLevelNum == userData.currentLevelUnlock + 1) {
                    uiText = "UNCOMPLETED";
                }
                else if (chosenLevelNum > userData.currentLevelUnlock + 1) {
                    uiText = "LOCKED";
                }

                levelUI.push_back("|  " + string((lineLength - uiText.length()) / 2, ' ') + uiText + string((lineLength - uiText.length()) - ((lineLength - uiText.length()) / 2), ' ') + "  |");
                levelUI.push_back("|  " + string(lineLength, ' ') + "  |");
                levelUI.push_back('*' + string(lineLength + 4, '-') + '*');

                showUI(levelUI);
            }

            updateScreenRender();
        }

        // delay inputs if key was pressed last loop
        if (wasKeyPressed) {
            Sleep(150);
            wasKeyPressed = false;
        }

        // update player position
        previousPlayerPosition.x = levelData.playerPosition.x;
        previousPlayerPosition.y = levelData.playerPosition.y;

        // get key pressed
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
            renderLevelStatusPixels(levelData, userData.currentLevelUnlock);
            updateScreenRender();
            isRenderingLighting = false;
        }
        else if ((GetKeyState('L') & 0x8000)) { // toggle lighting
            isRenderingLighting = !isRenderingLighting;
            previousPlayerPosition.x--;
        }
        else if ((GetKeyState(VK_RETURN) & 0x8000)) { // select level
            if (levelData.playerPosition.x % 2 == 0 && levelData.playerPosition.y % 2 == 0 && chosenLevelNum <= userData.currentLevelUnlock + 1) {
                hasChosenLevel = true;
            }
        }
        else {
            wasKeyPressed = false;
        }
    }

    return chosenLevelNum;
}

/**
Loads the user data from a text file.
*/
void loadUserData() {
    ifstream userDataRaw("UserData.txt");
    if (userDataRaw.is_open()) {
        string unlockedLevelsNum;
        getline(userDataRaw, unlockedLevelsNum);
        userData.currentLevelUnlock = stoi(unlockedLevelsNum);
    }
    userDataRaw.close();
}

/**
Saves the global user data to a text file.
*/
void saveUserData() {
    ofstream userDataRaw("UserData.txt");
    userDataRaw << userData.currentLevelUnlock;
    userDataRaw.close();
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
    
    // game loop
    while (true) {
        // get user to choose a level
        int chosenLevel = showLevelMenu();

        // play level
        int levelNum = chosenLevel;
        Level levelData = LoadLevel(levelNum);

        // reset game variables
        previousPlayerPosition = levelData.playerPosition;
        wasKeyPressed = false;
        hasQuitLevel = false;
        moves = 0;

        // intial screen render
        resetRendering();
        clearScreen();
        renderLevel(levelData);
        showMoves(levelData, moves);

        // loop until the level has been solved or the user has quit the level
        while (!isLevelSolved(levelData) && !hasQuitLevel) {
            // check if the players position has changed
            if (!(levelData.playerPosition.x == previousPlayerPosition.x && levelData.playerPosition.y == previousPlayerPosition.y)) {
                renderLevel(levelData, false);
                if (isRenderingLighting) {
                    renderLighting({ levelData.playerPosition.x * blockTextureSize + 2, levelData.playerPosition.y * blockTextureSize + 2 }, 20);
                }
                updateScreenRender();

                moves += 1;
                showMoves(levelData, moves);
            }

            // delay inputs if key was pressed last loop
            if (wasKeyPressed) {
                Sleep(150);
                wasKeyPressed = false;
            }

            // update player position
            previousPlayerPosition.x = levelData.playerPosition.x;
            previousPlayerPosition.y = levelData.playerPosition.y;

            // get key pressed
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
            else if ((GetKeyState('L') & 0x8000)) { // toggle lighting
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

        // is the user hasn't quit the level it means they solved it
        if (!hasQuitLevel) {
            renderLevel(levelData, false);
            if (isRenderingLighting) {
                renderLighting({ levelData.playerPosition.x * blockTextureSize + 2, levelData.playerPosition.y * blockTextureSize + 2 }, 20);
            }

            // transition to black screen
            for (int y = 0; y < levelData.levelBase.GetHeight() * blockTextureSize; y++) {
                for (int x = 0; x < levelData.levelBase.GetWidth() * blockTextureSize; x++) {
                    if (x % 5 == 0) {
                        Sleep(2);
                    }
                    forcePixelChange({ 12, 12, 12 }, { x, y }, "  ");
                    updateScreenRender();
                }
            }

            // update and save user data
            userData.currentLevelUnlock = max(userData.currentLevelUnlock, chosenLevel);
            saveUserData();

            showUI(levelCompleteUI);
            Sleep(3000);
        }

        resetRendering();
        clearScreen();
    }
}
