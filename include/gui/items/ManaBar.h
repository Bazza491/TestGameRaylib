#pragma once
#include "gui/GuiItem.h"
#include "Player.h"
#include "raymath.h"
#include "gui/GuiConstants.h"

class ManaBar : public GuiItem {
private:
    const Player* player = nullptr;

    float displayValue = 0.0f;
    float lerpSpeed    = 8.0f;

    float x = SCREEN_W - BAR_WIDTH - GUI_MARGIN;
    float y = GUI_MARGIN + BAR_HEIGHT + BAR_SPACING;
    float w = BAR_WIDTH;
    float h = BAR_HEIGHT;

public:
    explicit ManaBar(const Player* player);

    void update(float dt, Vector2 virtualMousePos) override;
    void draw()  const override;
};
