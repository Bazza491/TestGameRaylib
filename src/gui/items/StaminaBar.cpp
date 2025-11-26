#include "gui/items/StaminaBar.h"

StaminaBar::StaminaBar(const Player* player)
        : player(player)
{
    if (player)
        displayValue = (float)player->getStamina();
}

void StaminaBar::update(float dt, Vector2 virtualMousePos) {
    if (!player) return;

    float target = (float)player->getStamina();
    float t = 1.0f - expf(-lerpSpeed * dt);

    displayValue = Lerp(displayValue, target, t);
}

void StaminaBar::draw() const {
    if (!player) return;

    int maxStamina = player->getMaxStamina();
    if (maxStamina <= 0) return;

    float pct = Clamp(displayValue / (float)maxStamina, 0.0f, 1.0f);

    DrawRectangle(x, y, w, h, Color{40,40,40,255});
    DrawRectangle(x, y, w * pct, h, YELLOW);
    DrawRectangleLines(x, y, w, h, BLACK);

    DrawText("Stamina",
             x - 80,
             y,
             20,
             WHITE);
}
