#include "gui/items/HealthBar.h"

HealthBar::HealthBar(const Player* player)
        : player(player)
{
    if (player)
        displayValue = (float)player->getHealth();
}

void HealthBar::update(float dt) {
    if (!player) return;

    float target = (float)player->getHealth();
    float t = 1.0f - expf(-lerpSpeed * dt);

    displayValue = Lerp(displayValue, target, t);
}

void HealthBar::draw() const {
    if (!player) return;

    int maxHP = player->getMaxHealth();
    if (maxHP <= 0) return;

    float pct = Clamp(displayValue / (float)maxHP, 0.0f, 1.0f);

    DrawRectangle(x, y, w, h, Color{40,40,40,255});
    DrawRectangle(x, y, w * pct, h, GREEN);
    DrawRectangleLines(x, y, w, h, BLACK);

    DrawText("Health",
             x - 80,
             y,
             20,
             WHITE);
}