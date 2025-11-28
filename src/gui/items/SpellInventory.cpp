#include "gui/items/SpellInventory.h"

#include "guns/Spell.h"
#include <cmath>

SpellInventory::SpellInventory(Player* player)
    : player(player), storageGui(player ? &player->getSpellInventory() : nullptr) {}

void SpellInventory::update(float dt, Vector2 virtualMousePos) {
    if (!player) return;

    Vector2 mouse = virtualMousePos;

    storageGui.setStorage(&player->getSpellInventory());
    storageGui.setSlotSize(SPELL_SLOT_SIZE);
    storageGui.setPadding(SPELL_SLOT_SPACING);
    storageGui.setInteractive(true);
    storageGui.setAlpha(1.0f);
    storageGui.setBaseFontSize(16);

    const SpellDragState* drag = GuiSpellStorage::getActiveDrag();
    storageGui.update(dt, virtualMousePos);

    Layout layout = computeLayout();
    Vector2 origin{layout.startX, layout.startY};

    if ((!drag || !drag->active) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        int hit = storageGui.slotAtPosition(origin, layout.maxRowWidth, mouse);
        if (hit >= 0 && player->getSpellInventory().getSpell(hit)) {
            beginSlotDrag(hit, mouse);
        }
    }

    drag = GuiSpellStorage::getActiveDrag();
    if (drag && drag->active && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
        int targetSlot = storageGui.slotAtPosition(origin, layout.maxRowWidth, mouse);

        if (targetSlot >= 0) {
            dropOnSlot(drag->slot, targetSlot);
            storageGui.endDrag();
        }
        // If not dropped on this inventory, let other GUIs resolve the release.
    }
}

SpellInventory::Layout SpellInventory::computeLayout() const {
    Layout layout{};
    if (!player) return layout;

    int totalSlots = player->getSpellInventory().getCapacity();
    if (totalSlots <= 0) return layout;

    const int columns = 8;
    const int rows = 2;

    layout.maxRowWidth = columns * SPELL_SLOT_SIZE + SPELL_SLOT_SPACING * (columns - 1);

    float wandCount = (float) player->getWandSlots().size();
    float wandWidth = (wandCount > 0)
                      ? wandCount * (WAND_SLOT_SIZE + WAND_SLOT_SPACING) - WAND_SLOT_SPACING
                      : 0.0f;

    layout.startX = GUI_MARGIN + wandWidth + GUI_MARGIN;
    layout.startY = GUI_MARGIN;

    (void)rows; // rows reserved for future dynamic layouts

    return layout;
}

void SpellInventory::draw() const {
    if (!player) return;

    Layout layout = computeLayout();
    Vector2 origin{layout.startX, layout.startY};

    storageGui.setStorage(&player->getSpellInventory());
    storageGui.setSlotSize(SPELL_SLOT_SIZE);
    storageGui.setPadding(SPELL_SLOT_SPACING);
    storageGui.setInteractive(true);
    storageGui.setAlpha(1.0f);
    storageGui.setBaseFontSize(16);

    float renderedHeight = storageGui.renderGrid(origin, layout.maxRowWidth);

    float labelX = layout.startX;
    float labelY = layout.startY + renderedHeight + 6.0f;
    DrawText("Spells", (int)labelX, (int)labelY, 20, GRAY);

    storageGui.drawDraggedSpellSprite();
}

void SpellInventory::beginSlotDrag(int slotIndex, Vector2 mousePos) {
    if (!player) return;
    SpellStorage& storage = player->getSpellInventory();
    storageGui.setStorage(&storage);
    Layout layout = computeLayout();
    Vector2 origin{layout.startX, layout.startY};
    Rectangle rect = storageGui.getSlotRect(origin, layout.maxRowWidth, slotIndex);
    storageGui.beginSlotDrag(slotIndex, rect, mousePos);
}

void SpellInventory::dropOnSlot(int fromSlot, int toSlot) {
    if (!player) return;
    SpellStorage& storage = player->getSpellInventory();
    const SpellDragState* drag = GuiSpellStorage::getActiveDrag();
    if (!drag) return;

    if (drag->source == &storage) {
        storage.swapSpells(fromSlot, toSlot);
    } else if (drag->source) {
        drag->source->swapSpells(storage, fromSlot, toSlot);
    }
}

void SpellInventory::dropToWorld(int fromSlot) {
    (void)fromSlot;
}
