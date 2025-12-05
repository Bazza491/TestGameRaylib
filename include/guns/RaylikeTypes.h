#pragma once

#include <cstdint>

// Minimal stand-ins for raylib geometry and color structures so the guns
// system can be used without directly depending on raylib.

struct Vector2 {
    float x = 0.0f;
    float y = 0.0f;
};

struct Rectangle {
    float x = 0.0f;
    float y = 0.0f;
    float width = 0.0f;
    float height = 0.0f;
};

struct Color {
    std::uint8_t r = 0;
    std::uint8_t g = 0;
    std::uint8_t b = 0;
    std::uint8_t a = 0;
};

struct Texture2D {
    unsigned int id = 0;
    int width = 0;
    int height = 0;
    int mipmaps = 1;
    int format = 0;
};

constexpr Color WHITE{255, 255, 255, 255};
constexpr Color RED{255, 0, 0, 255};
constexpr Color GREEN{0, 255, 0, 255};
constexpr float PI = 3.14159265358979323846f;

