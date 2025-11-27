#include "gui/items/SpellInventory.h"

#include "guns/Spell.h"
#include <algorithm>
#include <string>

SpellInventory::SpellInventory(Player* player)
    : player(player) {}

void SpellInventory::update(float dt, Vector2 virtualMousePos) {
    (void)dt;
    if (!player) return;

    Vector2 mouse = virtualMousePos;
    int totalSlots = player->getSpellInventory().getCapacity();

    SpellDragState& drag = GetSpellDragState();
    UpdateDragMouse(virtualMousePos);

    if (!drag.active && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        for (int i = 0; i < totalSlots; ++i) {
            Rectangle slot = getSlotRect(i);
            if (CheckCollisionPointRec(mouse, slot) && player->getSpellInventory().getSpell(i)) {
                beginSlotDrag(i, mouse);
                break;
            }
        }
    }

    if (drag.active && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
        int targetSlot = -1;
        for (int i = 0; i < totalSlots; ++i) {
            Rectangle slot = getSlotRect(i);
            if (CheckCollisionPointRec(mouse, slot)) {
                targetSlot = i;
                break;
            }
        }

        if (targetSlot >= 0) {
            dropOnSlot(drag.slot, targetSlot);
        }

        EndSpellDrag();
    }
}

SpellInventory::Layout SpellInventory::computeLayout() const {
    Layout layout{};
    if (!player) return layout;

    int totalSlots = player->getSpellInventory().getCapacity();
    if (totalSlots <= 0) return layout;

    layout.slotSize = SPELL_SLOT_SIZE;
    layout.spacing = SPELL_SLOT_SPACING;

    float baseWidth = layout.slotSize * totalSlots + layout.spacing * (totalSlots - 1);

    float wandCount = (float) player->getWandSlots().size();
    float wandWidth = (wandCount > 0)
                      ? wandCount * (WAND_SLOT_SIZE + WAND_SLOT_SPACING) - WAND_SLOT_SPACING
                      : 0.0f;

    float minStartX = GUI_MARGIN + wandWidth + GUI_MARGIN;
    float maxEndX = SCREEN_W - BAR_WIDTH - GUI_MARGIN;
    float availableAfterWands = maxEndX - minStartX;

    if (availableAfterWands > 0 && baseWidth > availableAfterWands) {
        float scale = availableAfterWands / baseWidth;
        layout.slotSize *= scale;
        layout.spacing *= scale;
        baseWidth = layout.slotSize * totalSlots + layout.spacing * (totalSlots - 1);
    }

    layout.totalWidth = baseWidth;

    layout.startX = minStartX;
    layout.startY = GUI_MARGIN;

    return layout;
}

Rectangle SpellInventory::getSlotRect(int index) const {
    Layout layout = computeLayout();
    return {
        layout.startX + index * (layout.slotSize + layout.spacing),
        layout.startY,
        layout.slotSize,
        layout.slotSize
    };
}

void SpellInventory::drawSpellInSlot(int index, const Rectangle& rect) const {
    const Spell* spell = player->getSpellInventory().getSpell(index);
    if (!spell) return;
    if (IsDraggingSpellFrom(GetSpellDragState(), &player->getSpellInventory(), index)) return;

    DrawSpellLabelFitted(spell, rect, 14, GRAY);
}

void SpellInventory::draw() const {
    if (!player) return;

    const SpellStorage& storage = player->getSpellInventory();
    int totalSlots = storage.getCapacity();
    Layout layout = computeLayout();

    for (int i = 0; i < totalSlots; ++i) {
        Rectangle rect = getSlotRect(i);
        const Spell* spell = storage.getSpell(i);
        bool hiddenByDrag = IsDraggingSpellFrom(GetSpellDragState(), &player->getSpellInventory(), i);

        Color fill = SPELL_SLOT_COLOR;
        if (spell && !hiddenByDrag) {
            fill = GetSpellColor(spell);
        } else if (spell) {
            fill = SPELL_SLOT_OCCUPIED_COLOR;
        }

        DrawRectangleRec(rect, fill);
        DrawRectangleLinesEx(rect, SPELL_SLOT_BORDER, SPELL_SLOT_OUTLINE_COLOR);
        if (spell && !hiddenByDrag) {
            drawSpellInSlot(i, rect);
        }
    }

    float labelX = layout.startX;
    float labelY = layout.startY + layout.slotSize + 6.0f;
    DrawText("Spells", (int)labelX, (int)labelY, 20, GRAY);

    DrawDraggedSpellSprite(GetSpellDragState());
}

void SpellInventory::beginSlotDrag(int slotIndex, Vector2 mousePos) {
    if (!player) return;
    SpellStorage& storage = player->getSpellInventory();
    if (slotIndex < 0 || slotIndex >= storage.getCapacity()) return;
    if (!storage.getSpell(slotIndex)) return;

    Rectangle rect = getSlotRect(slotIndex);
    StartSpellDrag(&storage, slotIndex, rect, mousePos, computeLayout().slotSize);
}

void SpellInventory::dropOnSlot(int fromSlot, int toSlot) {
    if (!player) return;
    SpellStorage& storage = player->getSpellInventory();
    SpellDragState& drag = GetSpellDragState();
    if (drag.source == &storage) {
        storage.swapSpells(fromSlot, toSlot);
    } else if (drag.source) {
        drag.source->swapSpells(storage, fromSlot, toSlot);
    }
}

void SpellInventory::dropToWorld(int fromSlot) {
    (void)fromSlot;
}

void SpellInventory::drawDraggedSpell() const {
    DrawDraggedSpellSprite(GetSpellDragState());
}
