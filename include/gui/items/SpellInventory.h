#pragma once

#include "gui/GuiItem.h"
#include "gui/GuiConstants.h"
#include "gui/items/GuiSpellStorage.h"
#include "Player.h"

class Spell;

class SpellInventory : public GuiItem {
private:
    Player* player = nullptr;
    struct Layout {
        float startX = GUI_MARGIN;
        float startY = GUI_MARGIN;
        float maxRowWidth = 0.0f;
    };

    mutable GuiSpellStorage storageGui;

    [[nodiscard]] Layout computeLayout() const;
    bool quickSwapToHeldWand(int slotIndex);
public:
    explicit SpellInventory(Player* player);

    void update(float dt, Vector2 virtualMousePos) override;
    void draw() const override;

    void beginSlotDrag(int slotIndex, Vector2 mousePos);
    void dropOnSlot(int fromSlot, int toSlot);
    void dropToWorld(int fromSlot);
};
