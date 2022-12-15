#include "Textures.h"

std::map<Textures, Texture2D> loadedTextures;
std::map<Sounds, Sound> loadedSounds;

void LoadAssets() {
    for (auto &[name, path] : texturePaths) {
        loadedTextures[name] = LoadTexture(path);
    }
    
    for (auto &[name, path] : soundPaths) {
        loadedSounds[name] = LoadSound(path);
    }
}

void UnloadAssets() {
    for (auto &[name, texture] : loadedTextures) {
        UnloadTexture(texture);
    }
    for (auto &[name, sound] : loadedSounds) {
        UnloadSound(sound);
    }
}

Texture2D &GetTexture(Textures texture) {
    return loadedTextures.at(texture);
}

Sound &GetSound(Sounds sound) {
    return loadedSounds.at(sound);
}