#include "gui/items/WandInventory.h"
#include <algorithm>

WandInventory::WandInventory(Player* player)
    : player(player)
{}

void WandInventory::update(float dt, Vector2 virtualMousePos) {
    (void)dt;
    if (!player) return;

    Vector2 mouse = virtualMousePos;

    if (!dragging && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        for (int i = 0; i < (int)player->getWandSlots().size(); ++i) {
            Rectangle slot = getSlotRect(i);
            if (CheckCollisionPointRec(mouse, slot)) {
                player->selectWandSlot(i);
                beginSlotDrag(i, mouse);
                break;
            }
        }
    }

    if (dragging && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
        int targetSlot = -1;
        for (int i = 0; i < (int)player->getWandSlots().size(); ++i) {
            Rectangle slot = getSlotRect(i);
            if (CheckCollisionPointRec(mouse, slot)) {
                targetSlot = i;
                break;
            }
        }

        if (targetSlot >= 0) {
            dropOnSlot(draggedSlot, targetSlot);
        } else {
            dropToWorld(draggedSlot);
        }

        dragging = false;
        draggedSlot = -1;
    }
    dragPos = virtualMousePos;
}

Rectangle WandInventory::getSlotRect(int index) const {
    return {
        GUI_MARGIN + index * (WAND_SLOT_SIZE + WAND_SLOT_SPACING),
        GUI_MARGIN,
        WAND_SLOT_SIZE,
        WAND_SLOT_SIZE
    };
}

void WandInventory::drawWandInSlot(int index, const Rectangle& rect) const {
    const auto& wands = player->getWandSlots();
    if (!wands[index]) return;
    if (dragging && draggedSlot == index) return;

    Texture2D tex = wands[index]->getTexture();
    float maxW = WAND_SLOT_SIZE - 2 * WAND_SLOT_PADDING;
    float maxH = WAND_SLOT_SIZE - 2 * WAND_SLOT_PADDING;
    float scale = std::min(maxW / tex.width, maxH / tex.height);
    float drawW = tex.width * scale;
    float drawH = tex.height * scale;
    Rectangle dest = {
        rect.x + (rect.width - drawW) * 0.5f,
        rect.y + (rect.height - drawH) * 0.5f,
        drawW,
        drawH
    };

    DrawTexturePro(tex, {0, 0, (float)tex.width, (float)tex.height}, dest, {0, 0}, 0.0f, WHITE);
}

void WandInventory::draw() const {
    if (!player) return;

    const auto& wands = player->getWandSlots();
    int selectedSlot = player->getSelectedWandSlot();

    for (int i = 0; i < (int)wands.size(); ++i) {
        Rectangle rect = getSlotRect(i);

        Color fill = WAND_SLOT_COLOR;
        if (i == selectedSlot) {
            fill = WAND_SLOT_SELECTED_COLOR;
        } else if (wands[i]) {
            fill = WAND_SLOT_OCCUPIED_COLOR;
        }

        DrawRectangleRec(rect, fill);
        DrawRectangleLinesEx(rect, WAND_SLOT_BORDER, WAND_SLOT_OUTLINE_COLOR);
        drawWandInSlot(i, rect);
    }

    DrawText("Wands", (int)GUI_MARGIN, (int)(GUI_MARGIN + WAND_SLOT_SIZE + 5), 20, WHITE);

    drawDraggedWand();
}

void WandInventory::beginSlotDrag(int slotIndex, Vector2 mousePos) {
    if (!player) return;
    const auto& wands = player->getWandSlots();
    if (slotIndex < 0 || slotIndex >= (int)wands.size()) return;
    if (!wands[slotIndex]) return;

    dragging = true;
    draggedSlot = slotIndex;

    Rectangle rect = getSlotRect(slotIndex);
    dragPos = {mousePos.x - rect.x, mousePos.y - rect.y};
}

void WandInventory::dropOnSlot(int fromSlot, int toSlot) {
    if (!player) return;
    player->swapWands(fromSlot, toSlot);
}

void WandInventory::dropToWorld(int fromSlot) {
    (void)fromSlot;
}

void WandInventory::drawDraggedWand() const {
    if (!dragging || draggedSlot < 0 || !player) return;

    const auto& wands = player->getWandSlots();
    if (!wands[draggedSlot]) return;

    Texture2D tex = wands[draggedSlot]->getTexture();

    float maxW = WAND_SLOT_SIZE - 2 * WAND_SLOT_PADDING;
    float maxH = WAND_SLOT_SIZE - 2 * WAND_SLOT_PADDING;
    float scale = std::min(maxW / tex.width, maxH / tex.height);
    float drawW = tex.width * scale;
    float drawH = tex.height * scale;

    Rectangle dest = {
        dragPos.x /*+ (WAND_SLOT_SIZE - drawW) * 0.5f*/,
        dragPos.y /*+ (WAND_SLOT_SIZE - drawH) * 0.5f*/,
        drawW,
        drawH
    };

    DrawTexturePro(tex, {0, 0, (float)tex.width, (float)tex.height}, dest, {drawW * 0.5f, drawH * 0.5f}, 0.0f, WHITE);
}
