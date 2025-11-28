#pragma once

#include "gui/GuiItem.h"
#include "gui/GuiConstants.h"
#include "gui/SpellSlotUtils.h"
#include "Player.h"

class Spell;

class SpellInventory : public GuiItem {
private:
    Player* player = nullptr;
    struct Layout {
        float startX = GUI_MARGIN;
        float startY = GUI_MARGIN;
        float slotSize = SPELL_SLOT_SIZE;
        float spacing = SPELL_SLOT_SPACING;
        float totalWidth = 0.0f;
    };

    [[nodiscard]] Layout computeLayout() const;
    [[nodiscard]] Rectangle getSlotRect(int index) const;

public:
    explicit SpellInventory(Player* player);

    void update(float dt, Vector2 virtualMousePos) override;
    void draw() const override;

    void beginSlotDrag(int slotIndex, Vector2 mousePos);
    void dropOnSlot(int fromSlot, int toSlot);
    void dropToWorld(int fromSlot);
};
