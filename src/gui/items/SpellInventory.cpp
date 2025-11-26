#include "gui/items/SpellInventory.h"

#include "guns/Spell.h"
#include <algorithm>
#include <string>

namespace {
    constexpr int SPELLS_PER_ROW = 8;
    constexpr float SPELL_GRID_OFFSET_Y = WAND_SLOT_SIZE + 50.0f;
}

SpellInventory::SpellInventory(Player* player)
    : player(player) {}

void SpellInventory::update(float dt, Vector2 virtualMousePos) {
    (void)dt;
    if (!player) return;

    Vector2 mouse = virtualMousePos;
    int totalSlots = player->getSpellInventory().getCapacity();

    if (!dragging && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        for (int i = 0; i < totalSlots; ++i) {
            Rectangle slot = getSlotRect(i);
            if (CheckCollisionPointRec(mouse, slot) && player->getSpellInventory().getSpell(i)) {
                beginSlotDrag(i, mouse);
                break;
            }
        }
    }

    if (dragging && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
        int targetSlot = -1;
        for (int i = 0; i < totalSlots; ++i) {
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

Rectangle SpellInventory::getSlotRect(int index) const {
    int row = index / SPELLS_PER_ROW;
    int col = index % SPELLS_PER_ROW;
    return {
        GUI_MARGIN + col * (SPELL_SLOT_SIZE + SPELL_SLOT_SPACING),
        GUI_MARGIN + SPELL_GRID_OFFSET_Y + row * (SPELL_SLOT_SIZE + SPELL_SLOT_SPACING),
        SPELL_SLOT_SIZE,
        SPELL_SLOT_SIZE
    };
}

Color SpellInventory::getSpellColor(const Spell* spell) const {
    if (!spell) return SPELL_SLOT_COLOR;

    switch (spell->getType()) {
        case PROJECTILE:         return ORANGE;
        case STATIC_PROJECTILE:  return SKYBLUE;
        case MODIFIER:           return PURPLE;
        case DRAW_MANY:          return GOLD;
        case PASSIVE:            return GREEN;
        case MATERIAL:           return BROWN;
        case UTILITY:            return DARKGREEN;
        case OTHER:
        default:                 return GRAY;
    }
}

void SpellInventory::drawSpellInSlot(int index, const Rectangle& rect) const {
    const Spell* spell = player->getSpellInventory().getSpell(index);
    if (!spell) return;
    if (dragging && draggedSlot == index) return;

    int fontSize = 14;
    std::string label = spell->getName();
    if ((int)label.size() > 12) {
        label = label.substr(0, 12) + "...";
    }
    int textWidth = MeasureText(label.c_str(), fontSize);
    float textX = rect.x + (rect.width - textWidth) * 0.5f;
    float textY = rect.y + rect.height - fontSize - 4.0f;
    DrawText(label.c_str(), (int)textX, (int)textY, fontSize, WHITE);
}

void SpellInventory::draw() const {
    if (!player) return;

    const SpellStorage& storage = player->getSpellInventory();
    int totalSlots = storage.getCapacity();

    for (int i = 0; i < totalSlots; ++i) {
        Rectangle rect = getSlotRect(i);
        const Spell* spell = storage.getSpell(i);
        bool hiddenByDrag = dragging && draggedSlot == i;

        Color fill = SPELL_SLOT_COLOR;
        if (spell && !hiddenByDrag) {
            fill = getSpellColor(spell);
        } else if (spell) {
            fill = SPELL_SLOT_OCCUPIED_COLOR;
        }

        DrawRectangleRec(rect, fill);
        DrawRectangleLinesEx(rect, SPELL_SLOT_BORDER, SPELL_SLOT_OUTLINE_COLOR);
        if (spell && !hiddenByDrag) {
            drawSpellInSlot(i, rect);
        }
    }

    float labelY = GUI_MARGIN + SPELL_GRID_OFFSET_Y - 24.0f;
    DrawText("Spells", (int)GUI_MARGIN, (int)labelY, 20, WHITE);

    drawDraggedSpell();
}

void SpellInventory::beginSlotDrag(int slotIndex, Vector2 mousePos) {
    if (!player) return;
    SpellStorage& storage = player->getSpellInventory();
    if (slotIndex < 0 || slotIndex >= storage.getCapacity()) return;
    if (!storage.getSpell(slotIndex)) return;

    dragging = true;
    draggedSlot = slotIndex;

    Rectangle rect = getSlotRect(slotIndex);
    dragPos = {mousePos.x - rect.x, mousePos.y - rect.y};
}

void SpellInventory::dropOnSlot(int fromSlot, int toSlot) {
    if (!player) return;
    player->getSpellInventory().swapSpells(fromSlot, toSlot);
}

void SpellInventory::dropToWorld(int fromSlot) {
    (void)fromSlot;
}

void SpellInventory::drawDraggedSpell() const {
    if (!dragging || draggedSlot < 0 || !player) return;

    const Spell* spell = player->getSpellInventory().getSpell(draggedSlot);
    if (!spell) return;

    Rectangle rect = {
        dragPos.x - SPELL_SLOT_SIZE * 0.5f,
        dragPos.y - SPELL_SLOT_SIZE * 0.5f,
        SPELL_SLOT_SIZE,
        SPELL_SLOT_SIZE
    };

    Color fill = getSpellColor(spell);
    DrawRectangleRec(rect, fill);
    DrawRectangleLinesEx(rect, SPELL_SLOT_BORDER, SPELL_SLOT_OUTLINE_COLOR);

    int fontSize = 14;
    std::string label = spell->getName();
    if ((int)label.size() > 12) {
        label = label.substr(0, 12) + "...";
    }
    int textWidth = MeasureText(label.c_str(), fontSize);
    float textX = rect.x + (rect.width - textWidth) * 0.5f;
    float textY = rect.y + rect.height - fontSize - 4.0f;
    DrawText(label.c_str(), (int)textX, (int)textY, fontSize, WHITE);
}
