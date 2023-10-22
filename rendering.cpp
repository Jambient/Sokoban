#include <iostream>
#include <windows.h>
#include "main.h"
#include "rendering.h"
#include "blocks.h"
#include "ansi.h"

struct RenderTile {
    Color color;
    Vector2 position;
};

std::vector<std::vector<Color>> screenRender;
std::vector<std::vector<Color>> previousScreenRender;
std::vector<RenderTile> changedTiles;

void resizeScreenRender(int width, int height) {
    screenRender.resize((height + 1) * blockTextureSize, std::vector<Color>((width + 1) * blockTextureSize, Color()));
    previousScreenRender.resize((height + 1) * blockTextureSize, std::vector<Color>((width + 1) * blockTextureSize, Color()));
}

void drawTextureToScreen(Block blockData, Vector2 position) {
    for (int i = 0; i < blockTextureSize; i++) {
        for (int j = 0; j < blockTextureSize; j++) {
            Color color = blockData.texture[i][j];
            if (color.r < 0) { continue; }

            screenRender[position.y * blockTextureSize + i][position.x * blockTextureSize + j] = color;
        }
    }
}

void clearScreen() {
    system("CLS");
}

void fixRendering() {
    clearScreen();

    for (int y = 0; y < previousScreenRender.size(); y++) {
        for (int x = 0; x < previousScreenRender[0].size(); x++) {
            previousScreenRender[y][x] = Color();
        }
    }
}

void renderScreen(Level levelData) {
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

    // create goal sprites
    Block goalBlock = getBlockData(BlockName::GOAL);
    for (Vector2 goalPos : levelData.goals) {
        drawTextureToScreen(goalBlock, goalPos);
    }

    // create box sprites
    Block boxBlock = getBlockData(BlockName::BOX);
    for (PushableBox box : levelData.boxes) {
        drawTextureToScreen(boxBlock, box.position);
    }

    // create player sprite
    Block playerBlock = getBlockData(BlockName::PLAYER);
    drawTextureToScreen(playerBlock, levelData.playerPosition);

    // calculate changed tiles
    changedTiles.clear();

    for (int y = 0; y < levelBase.GetHeight() * blockTextureSize; y++) {
        for (int x = 0; x < levelBase.GetWidth() * blockTextureSize; x++) {
            Color currentColor = screenRender[y][x];
            Color previousColor = previousScreenRender[y][x];

            if (currentColor.r != previousColor.r || currentColor.g != previousColor.g || currentColor.b != previousColor.b) {
                changedTiles.push_back({ currentColor, {x, y} });
            }
        }
    }
    previousScreenRender = screenRender;

    // draw screen
    Color BLACK = { 0, 0, 0 };

    for (const RenderTile tile : changedTiles) {
        std::cout << "\033[" << tile.position.y << ";" << tile.position.x*2 << "H";
        std::cout << colorToAnsi(screenRender[tile.position.y][tile.position.x], ColorLayer::background) << "  " << getResetModifier();
    }
    std::cout << "\033[" << levelBase.GetHeight()*blockTextureSize << ";1H";
}