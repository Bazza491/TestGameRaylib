#pragma once
#include "raylib.h"

class GuiItem {
public:
    virtual ~GuiItem() = default;

    virtual void update(float dt) = 0;
    virtual void draw() const = 0;

    bool visible = true;
};