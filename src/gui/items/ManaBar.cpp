#include "gui/items/ManaBar.h"

ManaBar::ManaBar(const Player* player)
        : player(player)
{
    if (player)
        displayValue = (float)player->getMana();
}

void ManaBar::update(float dt) {
    if (!player) return;

    float target = (float)player->getMana();
    float t = 1.0f - expf(-lerpSpeed * dt);

    displayValue = Lerp(displayValue, target, t);
}

void ManaBar::draw() const {
    if (!player) return;

    int maxMana = player->getMaxMana();
//    if (maxMana <= 0) return;

    float pct = Clamp(displayValue / (float)maxMana, 0.0f, 1.0f);

    DrawRectangle(x, y, w, h, Color{40,40,40,255});
    DrawRectangle(x, y, w * pct, h, BLUE);
    DrawRectangleLines(x, y, w, h, BLACK);

    DrawText("Mana",
             x - 80,
             y,
             20,
             WHITE);
}
