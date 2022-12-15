#include "Application.h"
#include "Utils.h"
#include "Textures.h"
#include "PixelFont.h"
#include "debug.h"

Game game;
Music music;
PixelFont font;
int introTimer = 0;
bool usedCheat = false;

int localScoreMultiplier;
int localScore;
int lineAnimTimer = 0;
int borderThinkness = 4;
int levelClearTimer;
int levelCleared;

int nextFallingShape;
int nextFallingColor;

std::vector<int> filledLines;
std::vector<SnowParticle> snowParticles;

bool musicEnabled = true;
bool sfxEnabled = true;

void UpdateIntro();
void UpdateEnding();

void UpdateGame(Application* app);
void ResetGame();
void DrawSnowParticles();
void DrawBoard(Rectangle boardRect, float scale);
void DrawShape(Vector2 pos, int type, int rotation, int color, int scale, float alpha=1.0f);
void DrawFallingShape(Rectangle boardRect, float scale);
void DrawUI(Rectangle boardRect);
void UpdateLineAnimation();
void SpawnNewShape();
void MoveShape();
void OnShapeLand();
void OnLineComplete();
void OnNextLevel();
void RemoveLines(std::vector<int> lines);
bool CheckForCollisions();
int GetBorderType(std::vector<int> &matrix, Vector2 target, int width);
int GetAtMatrix(std::vector<int> &matrix, Vector2 target, int width);
Vector2 IndexToPos(int index, int width);

void PrintBoard();

// Initialize the application
void Application::Initialize() {
    game = Game {
        Board(13, 20)
    };

    ResetGame();
    game.level = 0;

    if (showIntro)
        currentState = ApplicationStates::Intro;
    else
        currentState = ApplicationStates::Running;

    // Particles
    for (int index = 0; index < GetRandomValue(100, 120); index++) {
        snowParticles.push_back(SnowParticle {
            Vector2 {                   // Position
                (float) GetRandomValue(5, screenWidth / bgZoom - 5),
                (float) GetRandomValue(-50, screenHeight / bgZoom - 5)
            },
            GetRandomValue(1, 3),       // Size
            GetRandomValue(8, 12) / 4,  // Speed
            GetRandomValue(0, 960)      // Current Lifetime
        });
    }
}

// Run the application
void Application::Run() {
    while (!WindowShouldClose()) {
        if (musicEnabled)
            UpdateMusicStream(music);

        switch (currentState)
        {
        case ApplicationStates::Intro:
            UpdateIntro();
            
            if (introTimer > 220) {
                currentState = ApplicationStates::Running;
                AddTransition(new ArrowTransitionReversed(20, 260, darkdarkBlue));
            }

            break;

        case ApplicationStates::Running:
            UpdateGame(this);
            break;

        case ApplicationStates::Ending:
            UpdateEnding();
            break;

        default:
            break;
        }
    }
}

// Load the application
void Application::Load() {
    InitAudioDevice();
    InitWindow(screenWidth, screenHeight, "Christmas Tetris");
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    SetTargetFPS(60);
    LoadAssets();

    SetSoundVolume(GetSound(Sounds::sfxPieceFail), 0.3f);

    music = LoadMusicStream("assets/sound/sleigh-ride.mp3");
    SetMusicVolume(music, 0.8f);
    PlayMusicStream(music);

    font = PixelFont(GetTexture(Textures::fontText), "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789:/\"!", 4);
}

// Unload the applicaiton
void Application::Unload() {
    UnloadAssets();
    CloseWindow();
}

/* ============= Functions ============= */

void UpdateIntro() {
    introTimer++;

    if (introTimer == 180) {
        AddTransition(new ArrowTransition(20, 260, darkdarkBlue));
    }

    BeginDrawing();
        if (introTimer >= 200) {
            ClearBackground(darkdarkBlue);

        } else {

            ClearBackground({0, 4, 13, 255});

            if (introTimer > 10) {
                float alpha = (float) (introTimer - 10) / 90;
                if (alpha > 1) alpha = 1;
                Vector2 textPos = {(float) (screenWidth - font.Measure("Made by Jake") * 2) / 2, GetScreenHeight() / 2 - font.height * 2};
                font.RenderColored({"Made by ", "Jake"}, textPos, 2, {ColorAlpha(WHITE, alpha), ColorAlpha(darkBlue, alpha)});
            }
        }

        DrawTransitions();

    EndDrawing();
}

void UpdateEnding() {
    BeginDrawing();
        ClearBackground({0, 4, 13, 255});

        Vector2 textPos = {(float) (screenWidth - font.Measure("You have completed Jolly Tetris") * 2) / 2, GetScreenHeight() / 2 - font.height * 4};
        font.RenderColored({"You have completed ", "Jolly Tetris"}, textPos, 2, {WHITE, darkBlue});
        font.RenderCentered({"Congrats"}, {GetScreenWidth() * 0.5f, textPos.y + font.height * 2 + 16}, 2, darkBlue, true, false);

        if (!usedCheat) {
            font.RenderColored({"Say to jake ", "\"I am the jolliest tetris player in the world\"", " to receive a prize!"}, {8, (float) GetScreenHeight() - font.height - 5}, 1, {WHITE, darkBlue, WHITE});
        }

        DrawTransitions();
    EndDrawing();
}

void UpdateGame(Application* app) {
    if (!levelCleared) {
        if (IsKeyPressed(KEY_R))
            ResetGame();

        // Cheat
        if (IsKeyDown(KEY_J) && IsKeyDown(KEY_A) && IsKeyDown(KEY_K) && IsKeyPressed(KEY_E)) {
            if (!IsKeyDown(KEY_C))
                usedCheat = true;
            levelCleared = true;
            levelClearTimer = 160;
        }

        if (lineAnimTimer) {
            lineAnimTimer--;

            if (!lineAnimTimer)
                OnLineComplete();
            else
                UpdateLineAnimation();

        } else {
            MoveShape();
        }
    } else {
        levelClearTimer--;
        if (levelClearTimer <= 0) {
            if (levelClearTimer == 0)
                AddTransition(new ArrowTransition(20, 260, darkdarkBlue, "level-clear"));
            else if (levelClearTimer < -40) {
                AddTransition(new ArrowTransitionReversed(20, 260, darkdarkBlue));

                if (game.level < totalLevels - 1)
                    OnNextLevel();
                else
                    app->currentState = ApplicationStates::Ending;
            }
        }
    }

    if (localScore != game.score) {
        if (std::abs(localScore - game.score) < localScoreMultiplier)
            localScore = game.score;
        else
            if (localScore < game.score)
                localScore += localScoreMultiplier;
            else
                localScore -= localScoreMultiplier;
    }

    // Drawing
    BeginDrawing();
        ClearBackground(BLACK);
        Texture2D &bgTexture = GetTexture(Textures::snowyBg);
        DrawTexturePro(GetTexture(Textures::snowyBg), 
            {0, 0, (float) bgTexture.width, (float) bgTexture.height}, {0, 0, screenWidth, screenHeight}, {0, 0}, 0, WHITE);
        
        BeginMode2D(Camera2D {{0, 0}, {0, 0}, 0, bgZoom});
            DrawSnowParticles();
        EndMode2D();

        float scale = 2;
        Rectangle boardRect = {screenWidth / 2 - game.board.width * 14.0f - 16, screenHeight / 2 - game.board.height * 14.0f, 
            game.board.width * 14.0f * scale, 14.0f * scale * game.board.height};
        DrawRectangleRec(boardRect, ColorAlpha(BLACK, 0.9));
        DrawRectangleLinesEx({boardRect.x - borderThinkness, boardRect.y - borderThinkness,  boardRect.width + borderThinkness * 2, 
            boardRect.height + borderThinkness * 2}, borderThinkness, borderColor);

        if (!levelCleared || (levelCleared && levelClearTimer % 50 < 25))
            DrawBoard(boardRect, scale);

        if (!lineAnimTimer && !levelCleared)
            DrawFallingShape(boardRect, scale);
    
        DrawUI(boardRect);

        if (levelCleared && levelClearTimer <= -20)
            DrawRectangleRec({0, 0, (float) GetScreenWidth(), (float) GetScreenHeight()}, darkdarkBlue);

        DrawTransitions();

    EndDrawing();
}

void ResetGame() {
    game.score = 0;
    game.linesCleared = 0;
    localScore = 0;
    levelCleared = false;
    levelClearTimer = 0;
    game.board.Reset();

    nextFallingShape = GetRandomValue(0, totalShapes - 1);
    nextFallingColor = GetRandomValue(1, totalColors);

    SpawnNewShape();
}
 
void DrawSnowParticles() {
    // Draw The particles
    for (auto &particle : snowParticles) {
        particle.lifetime += particle.speed;
        particle.pos.y += (float) particle.speed / 3;

        if (particle.pos.y > screenWidth / bgZoom)
            particle.pos.y = -particle.size;

        int xOffset = 0;
        int animationIndex = particle.lifetime % 960;
        if (animationIndex < 440)
            xOffset = -1;
        else if (animationIndex < 480)
            xOffset = 0;
        else if (animationIndex < 920)
            xOffset = 1;
        else
            xOffset = 1;

        DrawRectangle(particle.pos.x - xOffset, particle.pos.y, particle.size, particle.size, Color {250, 250, 250, 200});
    }
}

void DrawBoard(Rectangle boardRect, float scale) {
    Texture2D &presentText = GetTexture(Textures::presents);

    for (int y = 0; y < game.board.height; y++) {
        for (int x = 0; x < game.board.width; x++) {
            int color = game.board.GetAt(x, y);
            if (color == 0) continue;

            int borderType = GetBorderType(game.board.values, {(float) x, (float) y}, game.board.width);
            Vector2 sourcePos = IndexToPos(borderType, 4);
            Rectangle source = {tileSize * sourcePos.x, (float) 16 + tileSize * sourcePos.y + (color - 1) * 80, tileSize, tileSize};
            Rectangle dest = {boardRect.x + x * tileSize * scale, boardRect.y + y * tileSize * scale, tileSize * scale, tileSize * scale};

            DrawTexturePro(presentText, source, dest, {0, 0}, 0, WHITE);
        }
    }
}

void DrawShape(Vector2 pos, int type, int rotation, int color, int scale, float alpha) {
    Texture2D &presentText = GetTexture(Textures::presents);
    Shape shape = shapes[type];

    for (int index = 0; index < shape.width * shape.height; index++) {
        if (shape.rotations[rotation][index] == 0) continue;

        Vector2 matrixPos = IndexToPos(index, shape.width);
        Rectangle dest = {pos.x + matrixPos.x * tileSize * scale, pos.y + matrixPos.y * tileSize * scale, (float) tileSize * scale, (float) tileSize * scale};
        

        int borderType = GetBorderType(shape.rotations[rotation], matrixPos, shape.width);
        Vector2 sourcePos = IndexToPos(borderType, 4);
        Rectangle source = {tileSize * sourcePos.x, (float) 16 + (tileSize * sourcePos.y) + (color - 1) * 80, tileSize, tileSize};

        DrawTexturePro(presentText, source, dest, {0, 0}, 0, ColorAlpha(WHITE, alpha));
    }
}

void DrawFallingShape(Rectangle boardRect, float scale) {
    Shape shape = shapes[game.fallingShape];
    float startY = game.fallingPos.y;

    Vector2 shapePos = {
        boardRect.x + (game.fallingPos.x - shape.center.x) * tileSize * scale,
        boardRect.y + (game.fallingPos.y - shape.center.y) * tileSize * scale,
    };

    DrawShape(shapePos, game.fallingShape, game.fallingRotation, game.fallingColor, scale);

    
    int maxY = 0;

    while (true) {
        game.fallingPos.y++;
        if (CheckForCollisions()) {
            DrawShape({shapePos.x, shapePos.y + (game.fallingPos.y - startY - 1) * tileSize * scale}, game.fallingShape, game.fallingRotation, game.fallingColor, scale, 0.35f);
            break;
        }
    }
    game.fallingPos.y = startY;
}

void DrawUI(Rectangle boardRect) {
    // Side Board
    float boardLeft = boardRect.x + boardRect.width;
    Rectangle infoRect = {boardLeft + (screenWidth - boardLeft - 185) / 2, (screenHeight - 200) / 2, 185, 225};
    DrawRectangleRec(infoRect, ColorAlpha(BLACK, 0.9));
    DrawRectangleLinesEx({infoRect.x - borderThinkness, infoRect.y - borderThinkness,  infoRect.width + borderThinkness * 2, 
        infoRect.height + borderThinkness * 2}, borderThinkness, borderColor);

    // Next Shape
    Shape shape = shapes[nextFallingShape];
    font.RenderCentered("Next", {infoRect.x + infoRect.width / 2, infoRect.y + 12}, 2, darkBlue, true);

    Vector2 emptyRows = {-1, -1};
    for (int y = 0; y < shape.height; y++) {
        for (int type = 0; type < 2; type++) {
            int index = type == 0 ? y : shape.height - y - 1;
            for (int x = 0; x < shape.width; x++) {
                if (shape.rotations[0][index * shape.width + x] != 0) {
                    if (type == 0 && emptyRows.x == -1)
                        emptyRows.x = y;
                    else if (emptyRows.y == -1)
                        emptyRows.y = y;
                    break;
                }
            }
        }
    }

    int height = shape.width - emptyRows.x - emptyRows.y;
    Vector2 nextPeicePos = {infoRect.x + infoRect.width / 2 - shape.width * tileSize, 
        infoRect.y + infoRect.height / 2 - (height + emptyRows.x * 2) * tileSize - 42};
    DrawShape(nextPeicePos, nextFallingShape, 0, nextFallingColor, 2);

    Vector2 textStart = {infoRect.x + 16, infoRect.y + infoRect.height / 2};

    std::vector<std::string> scoreText = {std::string("Score: "), std::to_string(localScore)};
    if (localScore != game.score)
        font.RenderColored(scoreText, {textStart.x - GetRandomValue(-1, 1) * 3, textStart.y - GetRandomValue(-1, 1) * 3}, 2, {darkBlue, midBlue});
    else
        font.RenderColored(scoreText, textStart, 2, {darkBlue, midBlue});

    std::vector<std::string> levelsText = {std::string("Level: "), std::to_string(game.level + 1)};
    font.RenderColored(levelsText, {textStart.x, textStart.y + font.height * 2 + 8}, 2, {darkBlue, midBlue});
    std::vector<std::string> linesText = {std::string("Lines: "), std::to_string(game.linesCleared) + std::string("/")
        + std::to_string(levels[game.level].requiredLines)};
    font.RenderColored(linesText, {textStart.x, textStart.y + (font.height * 2 + 8) * 2}, 2, {darkBlue, midBlue});
}

void UpdateLineAnimation() {
    for (int y : filledLines) {
        for (int x = 0; x < game.board.width; x++) {
            if (game.board.width - x > game.board.width * ((float) lineAnimTimer / 20))
                game.board.SetAt(x, y, 0);
        }
    }
}

void SpawnNewShape() {
    game.fallingShape = nextFallingShape;
    game.fallingColor = nextFallingColor;
    game.fallingRotation = 0;
    game.fallingTimer = 0;
    game.fallingPos = {std::floor((float) game.board.width / 2), (float) shapes[game.fallingShape].height};

    while (true) {
        game.fallingPos.y--;
        if (CheckForCollisions()) {
            game.fallingPos.y++;
            break;
        }
    }

    nextFallingShape = GetRandomValue(0, totalShapes - 1);
    nextFallingColor = GetRandomValue(1, totalColors);

    if (CheckForCollisions()) {
        CheckForCollisions();
        ResetGame();
    }
}

void MoveShape() {
    Shape shape = shapes[game.fallingShape];
    Level level = levels[game.level];

    // Movement Controls
    bool left = IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT);
    bool right = IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT);

    if (left && !right) {
        game.fallingPos.x--;
        if (CheckForCollisions()) {
            game.fallingPos.x++;
            if (sfxEnabled)
                PlaySound(GetSound(Sounds::sfxPieceFail));
        } else {
            if (sfxEnabled)
                PlaySound(GetSound(Sounds::sfxPieceMove));
        }
    } else if (right && !left) {
        game.fallingPos.x++;
        if (CheckForCollisions()) {
            game.fallingPos.x--;
            if (sfxEnabled)
                PlaySound(GetSound(Sounds::sfxPieceFail));
        } else {
            if (sfxEnabled)
                PlaySound(GetSound(Sounds::sfxPieceMove));
        }
    }

    // Rotation Controls
    int totalRotations = (signed) shape.rotations.size();
    if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) {
        game.fallingRotation = mod(game.fallingRotation + 1, totalRotations);
        if (CheckForCollisions()) {
            game.fallingRotation = mod(game.fallingRotation - 1, totalRotations);
            if (sfxEnabled)
                PlaySound(GetSound(Sounds::sfxPieceFail));
        } else {
            if (sfxEnabled)
                PlaySound(GetSound(Sounds::sfxPieceRotate));
        }
    }

    // Fast Fall
    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) {
        if (game.fallingTimer < level.fallInterval - 2)
            game.fallingTimer = level.fallInterval - 2;
    }

    // Gravity
    game.fallingTimer++;
    if (game.fallingTimer > level.fallInterval) {
        game.fallingTimer = 0;
        game.fallingPos.y += 1;
        if (sfxEnabled)
            PlaySound(GetSound(Sounds::sfxPieceFall));

        if (CheckForCollisions()) {
            OnShapeLand();
        }
    }
}

void OnShapeLand() {
    Shape shape = shapes[game.fallingShape];

    for (int index = 0; index < shape.width * shape.height; index++) {
        if (shape.rotations[game.fallingRotation][index] == 0) continue;

        Vector2 matrixPos = IndexToPos(index, shape.width);
        Vector2 boardPos = {game.fallingPos.x + matrixPos.x - shape.center.x, game.fallingPos.y + matrixPos.y - shape.center.y - 1};

        game.board.SetAt(boardPos.x, boardPos.y, game.fallingColor);
    }
    
    filledLines = game.board.FindFilledLines();
    if (filledLines.size()) {
        lineAnimTimer = 20; 
        if (sfxEnabled)
            PlaySound(GetSound(Sounds::sfxLineClear));
    } else {
        SpawnNewShape();
    }

}

void OnLineComplete() {
    RemoveLines(filledLines);
    SpawnNewShape();

    int numLines = filledLines.size();

    if (numLines) {
        int additive = numLines * 200 - 100;
        game.score += additive;
        localScoreMultiplier = (int) ((float) additive / 200 + 1) * 3;
        game.linesCleared += numLines;

        if (game.linesCleared >= levels[game.level].requiredLines) {
            levelCleared = true;
            levelClearTimer = 160;
        }
    }
}

void OnNextLevel() {
    if (game.level >= totalLevels - 1) return;
    
    game.level++;
    nextFallingShape = GetRandomValue(0, totalShapes - 1);
    nextFallingColor = GetRandomValue(1, totalColors);
    ResetGame();
}

void RemoveLines(std::vector<int> lines) {
    if (!lines.size()) return;

    std::vector<int> newBoard = std::vector<int>(game.board.width * game.board.height, 0);

    for (int y = 0; y < game.board.height; y++) {
        int newY = y;
        
        for (int line : lines) {
            if (line == newY) {
                newY = -1;
                break;
            } else if (line > newY) {
                newY++;
            }   
        }

        if (newY == -1)
            continue;

        for (int x = 0; x < game.board.width; x++)
            newBoard[newY * game.board.width + x] = game.board.GetAt(x, y);
    }

    game.board.values = newBoard;
}

bool CheckForCollisions() {
    Shape shape = shapes[game.fallingShape];
    for (int index = 0; index < shape.width * shape.height; index++) {
        if (shape.rotations[game.fallingRotation][index] == 0) continue;

        Vector2 matrixPos = IndexToPos(index, shape.width);
        Vector2 boardPos = {game.fallingPos.x + matrixPos.x - shape.center.x, game.fallingPos.y + matrixPos.y - shape.center.y};

        if (boardPos.x < 0 || boardPos.x > game.board.width - 1) return true;
        if (boardPos.y < 0 || boardPos.y > game.board.height - 1) return true;
        if (game.board.GetAt(boardPos.x, boardPos.y) != 0) return true;
    }

    return false;
}

int GetBorderType(std::vector<int> &matrix, Vector2 target, int width) {
    int color = GetAtMatrix(matrix, target, width);
    if (color == 0) return 0;

    bool top = GetAtMatrix(matrix, {target.x, target.y - 1}, width) == color;
    bool right = GetAtMatrix(matrix, {target.x + 1, target.y}, width) == color;
    bool bottom = GetAtMatrix(matrix, {target.x, target.y + 1}, width) == color;
    bool left = GetAtMatrix(matrix, {target.x - 1, target.y}, width) == color;

    if (top) {
        if (!left && right && bottom) return 4;
        if (left && right && bottom) return 5;
        if (left && !right && bottom) return 6;
        if (!left && !right && bottom) return 7;

        if (!left && right && !bottom) return 8;
        if (left && right && !bottom) return 9;
        if (left && !right && !bottom) return 10;
        if (!left && !right && !bottom) return 11;
    } else {
        if (!left && right && bottom) return 0;
        if (left && right && bottom) return 1;
        if (left && !right && bottom) return 2;
        if (!left && !right && bottom) return 3;

        if (!left && right && !bottom) return 12;
        if (left && right && !bottom) return 13;
        if (left && !right && !bottom) return 14;
    }

    return 15;
}

int GetAtMatrix(std::vector<int> &matrix, Vector2 target, int width) {
    int index = target.y * width + target.x;
    
    if (!(0 <= target.x && target.x < width)) return -1;
    if (!(0 <= target.y && target.y < (signed) matrix.size() / width)) return -1;

    if (index >= 0 && index < (signed) matrix.size()) {
        return matrix[index];
    }
    return -1;
};

Vector2 IndexToPos(int index, int width) {
    return {(float) (index % width), (float) (int) (index / width)};
}

void PrintBoard() {
    print("> board <");
    for (int y = 0; y < game.board.height; y++) {
        std::string output = "";
        for (int x = 0; x < game.board.width; x++) {
            output += std::to_string(game.board.GetAt(x, y));
        }
        print(output);
    }
    print(">      <");
}

/* ============= Classes ============= */

void Board::Reset() {
    for (int index = 0; index < (signed) values.size(); index++) {
        values[index] = 0;
    }
}

int Board::GetAt(int x, int y) {
    if (isOnBoard(x, y)) {
        return values[y * width + x];
    }
    return 0;
}

void Board::SetAt(int x, int y, int color) {
    if (isOnBoard(x, y)) {
        values[y * width + x] = color;
    }
}

bool Board::isOnBoard(int x, int y) {
    if (x < 0 || x > width - 1)
        return false;
    if (y < 0 || y > height - 1)
        return false;
    return true;
}

std::vector<int> Board::FindFilledLines() {
    std::vector<int> filledLines;

    for (int y = 0; y < game.board.height; y++) {
        bool filled = true;
        for (int x = 0; x < game.board.width; x++) {
            if (game.board.GetAt(x, y) == 0) {
                filled = false;
                break;
            }
        }

        if (filled)
            filledLines.push_back(y);
    }

    return filledLines;
}
