#include <iostream>
#include <vector>
#include <windows.h>
#include <chrono>
#include "common.h"
#include "rendering.h"
#include "blocks.h"
#include "ansi.h"
#include "ascii.h"

// structs only used by this file
struct RenderTile {
    Color color;
    std::string content = "  ";
};

struct ChangedTile {
    RenderTile tileData;
    Vector2 position;
};

// variables only used by this file
std::vector<std::vector<RenderTile>> screenRender;
std::vector<std::vector<RenderTile>> previousScreenRender;
std::vector<ChangedTile> changedTiles;
Color BLACK = { 0, 0, 0 };

bool SHOW_DEBUG = true;

void resizeScreenRender(int width, int height) {
    screenRender.clear();
    previousScreenRender.clear();

    for (int y = 0; y < height * blockTextureSize; y++) {
        std::vector<RenderTile> temp;
        for (int x = 0; x < width * blockTextureSize; x++) {
            temp.push_back({ BLACK });
        }

        screenRender.push_back(temp);
        previousScreenRender.push_back(temp);
    }
}

/**
* Draws a block texture to the screen at the given position.
*
* @param blockData - The block data containing the texture.
* @param position - The position the top left of the texture should be drawn at.
*/
void drawTextureToScreen(Block blockData, Vector2 position) {
    for (int i = 0; i < blockTextureSize; i++) {
        for (int j = 0; j < blockTextureSize; j++) {
            Color color = blockData.texture[i][j];
            if (color.r < 0) { continue; }

            screenRender[position.y * blockTextureSize + i][position.x * blockTextureSize + j] = { color };
        }
    }
}

void clearScreen() {
    system("CLS");

    for (int y = 0; y < screenRender.size(); y++) {
        for (int x = 0; x < screenRender[0].size(); x++) {
            screenRender[y][x] = { Color() };
        }
    }
}

void resetRendering() {
    system("CLS");

    for (int y = 0; y < previousScreenRender.size(); y++) {
        for (int x = 0; x < previousScreenRender[0].size(); x++) {
            previousScreenRender[y][x] = { Color() };
        }
    }
}

void renderLevel(Level levelData, bool updateScreen) {
    Map levelBase = levelData.levelBase;

    for (int y = 0; y < levelBase.GetHeight(); y++) {
        for (int x = 0; x < levelBase.GetWidth(); x++) {
            MapTile tile = levelBase.Get(x, y);
            Block blockData;

            if (tile == MapTile::EMPTY) {
                blockData = getBlockData(BlockName::EMPTY);
            }
            else if (tile == MapTile::WALL) {
                blockData = getBlockData(BlockName::WALL);
            }
            else if (tile == MapTile::FLOOR) {
                blockData = getBlockData(BlockName::FLOOR);
            }


            drawTextureToScreen(blockData, { x, y });
        }
    }

    // create box sprites
    Block boxBlock = getBlockData(BlockName::BOX);
    for (PushableBox box : levelData.boxes) {
        drawTextureToScreen(boxBlock, box.position);
    }

    // create goal sprites
    Block goalBlock = getBlockData(BlockName::GOAL);
    for (Vector2 goalPos : levelData.goals) {
        drawTextureToScreen(goalBlock, goalPos);
    }

    // create player sprite
    Block playerBlock = getBlockData(BlockName::PLAYER);
    drawTextureToScreen(playerBlock, levelData.playerPosition);

    if (updateScreen) {
        updateScreenRender();
    }
}

void updateScreenRender() {
    auto start = std::chrono::high_resolution_clock::now();

    // calculate changed tiles
    changedTiles.clear();

    for (int y = 0; y < screenRender.size(); y++) {
        for (int x = 0; x < screenRender[0].size(); x++) {
            Color currentColor = screenRender[y][x].color;
            Color previousColor = previousScreenRender[y][x].color;

            if (currentColor.r != previousColor.r || currentColor.g != previousColor.g || currentColor.b != previousColor.b) {
                changedTiles.push_back({ screenRender[y][x], {x, y} });
            }
        }
    }
    previousScreenRender = screenRender;

    // draw screen
    int screenWidth = screenRender[0].size();

    bool shouldJump = true;
    for (int i = 0; i < changedTiles.size(); i++) {
        ChangedTile& tile = changedTiles[i];
        shouldJump = true;

        if (i > 0) {
            ChangedTile& previousTile = changedTiles[i-1];
            if (previousTile.position.x == tile.position.x - 1 && previousTile.position.y == tile.position.y) {
                shouldJump = false;
            }
        }

        if (shouldJump) {
            std::cout << "\033[" << tile.position.y + 1 << ";" << tile.position.x * 2 + 1 << "H";
        }
        std::cout << colorToAnsi(tile.tileData.color, ColorLayer::background) << tile.tileData.content << getResetModifier();
    }
    std::cout << "\033[" << screenRender.size() << ";1H";

    // calculate frame render time
    if (SHOW_DEBUG) {
        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
        std::cout << duration.count() << " milliseconds";
    }
}

void forcePixelChange(Color color, Vector2 position, std::string content) {
    screenRender[position.y][position.x] = { color, content };
}

void showUI(std::vector<std::string> &lines) {
    int uiHeight = lines.size();
    int uiWidth = 0;

    // get longest line
    for (const std::string& line : lines) {
        int lineLength = line.length();
        if (lineLength > uiWidth) {
            uiWidth = lineLength;
        }
    }

    Vector2 centerPos = { screenRender[0].size() / 2, screenRender.size() / 2 };

    for (int y = 0; y < uiHeight; y++) {
        for (int x = 0; x * 2 < uiWidth; x++) {
            std::string content = "  ";
            if (x * 2 < lines[y].length()) {
                content = lines[y].substr(x * 2, 2);
            }
            if (content.length() < 2) {
                content += ' ';
            }

            Color color = { 170, 74, 68 };
            if (y == 0 || y == uiHeight - 1 || x == 0 || x * 2 == uiWidth - 2) {
                color = { 160, 64, 58 };
            }
            forcePixelChange(color, { centerPos.x - uiWidth / 4 + x, centerPos.y - uiHeight / 2 + y }, content);
        }
    }

    updateScreenRender();
}

void renderLighting(Vector2 lightPosition, int lightStrength) {
    for (int y = 0; y < screenRender.size(); y++) {
        for (int x = 0; x < screenRender[0].size(); x++) {
            int dX = lightPosition.x - x;
            int dY = lightPosition.y - y;
            float distance = dX * dX + dY * dY;
            float colorCalc = (1 - abs(min(distance / (lightStrength * lightStrength), 1))) * 255;
            int colorValue = floor(colorCalc);

            Color currentColor = screenRender[y][x].color;
            Color newColor = { (currentColor.r * colorValue) / 255, (currentColor.g * colorValue) / 255, (currentColor.b * colorValue) / 255 };
            forcePixelChange(newColor, { x, y }, "  ");
        }
    }
}

void applyBlur(int strength) {
    int boxSize = 1 + 2 * strength;
    int width = screenRender[0].size();
    int height = screenRender.size();

    std::vector<std::vector<RenderTile>> tempRender(screenRender);
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            Color boxSum;

            for (int j = -strength; j < boxSize; j++) {
                for (int i = -strength; i < boxSize; i++) {
                    if (x + i >= 0 && x + i < width && y + j >= 0 && y + j < height) {
                        Color pixelColor = screenRender[y + j][x + i].color;
                        boxSum.r += pixelColor.r;
                        boxSum.g += pixelColor.g;
                        boxSum.b += pixelColor.b;
                    }
                }
            }

            boxSum.r /= boxSize * boxSize;
            boxSum.g /= boxSize * boxSize;
            boxSum.b /= boxSize * boxSize;

            tempRender[y][x] = { boxSum };
        }
    }

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            forcePixelChange(tempRender[y][x].color, {x, y}, "  ");
        }
    }
}

void showMoves(Level& levelData) {
    std::string numStr = std::to_string(levelData.moveCount);
    int xPos = 1;
    int yPos = (levelData.levelBase.GetHeight() + 1) * blockTextureSize;

    for (int i = numStr.size(); i < 4; i++) {
        numStr.insert(numStr.begin(), '0');
    }

    for (int y = 0; y < 6; y++) {
        std::cout << moveCursorToPosition({ xPos, yPos + y });
        std::cout << std::string(50, ' ');
    }

    int totalNumLength = 0;
    for (char c : numStr) {
        totalNumLength += printDigit(c - '0', { xPos + totalNumLength + 2, yPos });
    }
    std::cout << moveCursorToPosition({ xPos, yPos - 1 });
    std::cout << "NUMBER OF MOVES:";
}

void renderLevelStatusPixels(Level levelData, int currentLevelUnlock) {
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