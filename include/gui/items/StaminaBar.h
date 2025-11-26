#pragma once
#include "gui/GuiItem.h"
#include "Player.h"
#include "raymath.h"
#include "gui/GuiConstants.h"

class StaminaBar : public GuiItem {
private:
    const Player* player = nullptr;

    float displayValue = 0.0f;
    float lerpSpeed    = 8.0f;

    float x = SCREEN_W - BAR_WIDTH - GUI_MARGIN;
    float y = GUI_MARGIN + 2*BAR_HEIGHT + 2*BAR_SPACING;
    float w = BAR_WIDTH;
    float h = BAR_HEIGHT;

public:
    explicit StaminaBar(const Player* player);

    void update(float dt, Vector2 virtualMousePos) override;
    void draw()  const override;
};