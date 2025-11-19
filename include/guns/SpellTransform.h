#pragma once

#include "raylib.h"

struct SpellTransform {
    Vector2 position;
    float rotation;

    SpellTransform(Vector2 vector2, float d) : position(vector2), rotation(d) {}
};
