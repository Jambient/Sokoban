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
#include "commands.h"
#include <vector>
#include <stack>
#include <memory>

using namespace std;

// a global storing the users data
UserData userData;
std::stack<std::unique_ptr<Command>> commandStack;

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
            std::unique_ptr<Command> command = std::make_unique<MoveCommand>(Vector2{ 0, -1 });
            command->Execute(levelData);
        }
        else if ((GetKeyState('S') & 0x8000)) {
            std::unique_ptr<Command> command = std::make_unique<MoveCommand>(Vector2{ 0, 1 });
            command->Execute(levelData);
        }
        else if ((GetKeyState('A') & 0x8000)) {
            std::unique_ptr<Command> command = std::make_unique<MoveCommand>(Vector2{ -1, 0 });
            command->Execute(levelData);
        }
        else if ((GetKeyState('D') & 0x8000)) {
            std::unique_ptr<Command> command = std::make_unique<MoveCommand>(Vector2{ 1, 0 });
            command->Execute(levelData);
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
    bool hasQuitLevel;
    bool isRenderingLighting = false;
    
    // game loop
    while (true) {
        // get user to choose a level
        int chosenLevel = showLevelMenu();

        // play level
        Level levelData = LoadLevel(chosenLevel);

        // reset game variables
        previousPlayerPosition = levelData.playerPosition;
        wasKeyPressed = false;
        hasQuitLevel = false;

        // intial screen render
        resetRendering();
        clearScreen();
        renderLevel(levelData);
        showMoves(levelData);

        while (!commandStack.empty()) {
            commandStack.pop();
        }

        // loop until the level has been solved or the user has quit the level
        while (!isLevelSolved(levelData) && !hasQuitLevel) {
            // check if the players position has changed
            if (!(levelData.playerPosition.x == previousPlayerPosition.x && levelData.playerPosition.y == previousPlayerPosition.y)) {
                renderLevel(levelData, false);
                if (isRenderingLighting) {
                    renderLighting({ levelData.playerPosition.x * blockTextureSize + 2, levelData.playerPosition.y * blockTextureSize + 2 }, 20);
                }
                updateScreenRender();
                showMoves(levelData);
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
                std::unique_ptr<Command> command = std::make_unique<MoveCommand>(Vector2{ 0, -1 });
                command->Execute(levelData);
                commandStack.push(std::move(command));
            }
            else if ((GetKeyState('S') & 0x8000)) {
                std::unique_ptr<Command> command = std::make_unique<MoveCommand>(Vector2{ 0, 1 });
                command->Execute(levelData);
                commandStack.push(std::move(command));
            }
            else if ((GetKeyState('A') & 0x8000)) {
                std::unique_ptr<Command> command = std::make_unique<MoveCommand>(Vector2{ -1, 0 });
                command->Execute(levelData);
                commandStack.push(std::move(command));
            }
            else if ((GetKeyState('D') & 0x8000)) {
                std::unique_ptr<Command> command = std::make_unique<MoveCommand>(Vector2{ 1, 0 });
                command->Execute(levelData);
                commandStack.push(std::move(command));
            }
            else if ((GetKeyState('Z') & 0x8000)) {
                if (!commandStack.empty()) {
                    std::unique_ptr<Command> command = std::move(commandStack.top());
                    commandStack.pop();
                    command->Undo(levelData);
                }
            }
            else if ((GetKeyState('R') & 0x8000)) { // reset level
                std::unique_ptr<Command> command = std::make_unique<ResetCommand>();
                command->Execute(levelData);
                commandStack.push(std::move(command));

                renderLevel(levelData, true);
                showMoves(levelData);
                previousPlayerPosition.x = levelData.playerPosition.x;
                previousPlayerPosition.y = levelData.playerPosition.y;
                isRenderingLighting = false;
            }
            else if ((GetKeyState('F') & 0x8000)) { // fix button incase rendering breaks (when resizing screen)
                resetRendering();
                renderLevel(levelData);
                showMoves(levelData);
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
