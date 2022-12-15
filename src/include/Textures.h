#pragma once
#include "pch.h"

enum Textures {
    snowyBg,
    presents,
    fontText
};

enum Sounds {
    sfxPieceFall,
    sfxPieceMove,
    sfxPieceRotate,
    sfxPieceFail,
    sfxLineClear
};

inline std::map<Textures, const char*> texturePaths = {
    {snowyBg, "assets/image/snowybg.png"},
    {presents, "assets/image/presents.png"},
    {fontText, "assets/image/font.png"},
};

inline std::map<Sounds, const char*> soundPaths = {
    {sfxPieceFall, "assets/sound/SFX_PieceFall.ogg"},
    {sfxPieceMove, "assets/sound/SFX_PieceMove.ogg"},
    {sfxPieceRotate, "assets/sound/SFX_PieceRotate.ogg"},
    {sfxPieceFail, "assets/sound/SFX_PieceFail.ogg"},
    {sfxLineClear, "assets/sound/SFX_LineClear.mp3"}
};

void LoadAssets();
void UnloadAssets();

Texture2D &GetTexture(Textures texture);
Sound &GetSound(Sounds sound);
