#pragma once
#include "pch.h"
#include "Transitions.h"

const Color borderColor = {62, 67, 156, 255};
const Color darkBlue = {91, 110, 255, 255};
const Color darkdarkBlue = {63, 63, 116, 255};
const Color midBlue = {99, 155, 255, 255};
const Color cyan = {95, 205, 228, 255};
const int bgZoom = 5;
const int screenWidth = 176 * bgZoom;
const int screenHeight = 128 * bgZoom;
const int totalColors = 5;
const int tileSize = 14;

enum ApplicationStates {
    Running,
    Intro,
    Ending
};

class Application {
public:
    bool showIntro = true;
    ApplicationStates currentState;
    
    Application() = default;
    
    void Initialize();
    void Run();
    void Load();
    void Unload();
};

class Board {
public:
    int width, height;
    std::vector<int> values;

    Board() = default;
    Board(int _width, int _height) : width(_width), height(_height), values(std::vector<int>(_width * _height,  0)) {};
    
    void Reset();
    void SetAt(int x, int y, int color);
    int GetAt(int x, int y);
    bool isOnBoard(int x, int y);
    std::vector<int> FindFilledLines();

};

struct Game {
    Board board;

    Vector2 fallingPos;
    int fallingShape;
    int fallingRotation;
    int fallingColor;
    int fallingTimer;

    int level;
    int score;
    int linesCleared;
};

struct SnowParticle {
    Vector2 pos;
    int size;
    int speed;
    int lifetime;
};

struct Shape {
    int width;
    int height;
    Vector2 center;
    std::vector<std::vector<int>> rotations;
};

struct Level {
    int fallInterval;
    int requiredLines;
};

const int totalShapes = 7;
const Shape shapes[totalShapes] = {
    Shape {3, 3, {1, 1}, {
        {
            0, 0, 0,
            1, 1, 1,
            0, 0, 1
        }, {
            0, 1, 0,
            0, 1, 0,
            1, 1, 0
        }, {
            1, 0, 0,
            1, 1, 1,
            0, 0, 0
        }, {
            0, 1, 1,
            0, 1, 0,
            0, 1, 0
        }
    }},
    Shape {3, 3, {1, 1}, {
        {
            0, 0, 0,
            1, 1, 1,
            1, 0, 0
        }, {
            1, 1, 0,
            0, 1, 0,
            0, 1, 0
        }, {
            0, 0, 1,
            1, 1, 1,
            0, 0, 0
        }, {
            0, 1, 0,
            0, 1, 0,
            0, 1, 1
        }
    }},
    Shape {3, 3, {1, 1}, {
        {
            0, 1, 0,
            1, 1, 1,
            0, 0, 0
        }, {
            0, 1, 0,
            0, 1, 1,
            0, 1, 0
        }, {
            0, 0, 0,
            1, 1, 1,
            0, 1, 0
        }, {
            0, 1, 0,
            1, 1, 0,
            0, 1, 0
        }
    }},
    Shape {3, 3, {1, 1}, {
        {
            0, 0, 0,
            0, 1, 1,
            1, 1, 0
        }, {
            1, 0, 0,
            1, 1, 0,
            0, 1, 0
        }, {
            0, 1, 1,
            1, 1, 0,
            0, 0, 0
        }, {
            0, 1, 0,
            0, 1, 1,
            0, 0, 1
        }
    }},
    Shape {3, 3, {1, 1}, {
        {
            0, 0, 0,
            1, 1, 0,
            0, 1, 1
        }, {
            0, 1, 0,
            1, 1, 0,
            1, 0, 0
        }, {
            1, 1, 0,
            0, 1, 1,
            0, 0, 0
        }, {
            0, 0, 1,
            0, 1, 1,
            0, 1, 0
        }
    }},
    Shape {2, 2, {0, 0}, {
        {
            1, 1,
            1, 1
        }
    }},
    Shape {4, 4, {2, 2}, {
        {
            0, 0, 0, 0,
            0, 0, 0, 0,
            1, 1, 1, 1,
            0, 0, 0, 0,
        },
        {
            0, 0, 1, 0,
            0, 0, 1, 0,
            0, 0, 1, 0,
            0, 0, 1, 0,
        },
    }}
};

const int totalLevels = 5;
const Level levels[totalLevels] = {
    Level {40, 4},
    Level {32, 8},
    Level {22, 16},
    Level {10, 24},
    Level {4, 32}
};