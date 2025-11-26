#pragma once

#include "gui/GuiItem.h"
#include "gui/GuiConstants.h"
#include "Player.h"

class WandInventory : public GuiItem {
private:
    Player* player = nullptr;
    bool dragging = false;
    int draggedSlot = -1;
    Vector2 dragPos{0.0f, 0.0f};

    [[nodiscard]] Rectangle getSlotRect(int index) const;
    void drawWandInSlot(int index, const Rectangle& rect) const;
    void drawDraggedWand() const;

public:
    explicit WandInventory(Player* player);

    void update(float dt, Vector2 virtualMousePos) override;
    void draw() const override;

    // Interaction hooks (logic to be implemented later)
    void beginSlotDrag(int slotIndex, Vector2 mousePos);
    void dropOnSlot(int fromSlot, int toSlot);
    void dropToWorld(int fromSlot);
};
