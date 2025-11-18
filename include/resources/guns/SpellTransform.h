//
// Created by Bailey on 3/10/2025.
//

#pragma once

#include "raylib.h"

struct SpellTransform {
    Vector2 position;
    float rotation;

    SpellTransform() : position({0, 0}), rotation(0) {}
};
