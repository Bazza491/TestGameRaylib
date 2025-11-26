#pragma once
#include "raylib.h"

class GuiItem {
public:
    virtual ~GuiItem() = default;

    virtual void update(float dt, Vector2 virtualMousePos) = 0;
    virtual void draw() const = 0;

    bool visible = true;
};