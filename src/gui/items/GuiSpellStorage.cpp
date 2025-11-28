#include "gui/items/GuiSpellStorage.h"

#include "guns/Spell.h"

#include <algorithm>
#include <cmath>

GuiSpellStorage* GuiSpellStorage::activeDragOwner = nullptr;

GuiSpellStorage::GuiSpellStorage(SpellStorage* storage, Config cfg)
    : storage(storage), config(cfg) {}

void GuiSpellStorage::update(float dt, Vector2 virtualMousePos) {
    (void)dt;
    updateDragMouse(virtualMousePos);
}

void GuiSpellStorage::draw() const {
    // This GUI element is intended to be embedded; drawing is managed by callers via renderGrid().
    drawDraggedSpellSprite();
}

SpellStorage* GuiSpellStorage::getStorage() const {
    return storage;
}

void GuiSpellStorage::setStorage(SpellStorage* newStorage) {
    storage = newStorage;
}

void GuiSpellStorage::setSlotSize(float size) {
    config.slotSize = size;
}

void GuiSpellStorage::setPadding(float pad) {
    config.padding = pad;
}

void GuiSpellStorage::setAlpha(float newAlpha) {
    config.alpha = newAlpha;
}

void GuiSpellStorage::setInteractive(bool enabled) {
    config.interactive = enabled;
}

void GuiSpellStorage::setBaseFontSize(int size) {
    config.baseFontSize = size;
}

bool GuiSpellStorage::isDragOwner() const {
    return activeDragOwner == this;
}

const SpellDragState* GuiSpellStorage::getActiveDrag() {
    return activeDragOwner ? &activeDragOwner->dragState : nullptr;
}

GuiSpellStorage* GuiSpellStorage::getActiveDragOwner() {
    return activeDragOwner;
}

bool GuiSpellStorage::isDraggingSpellFrom(const SpellStorage* sourceStorage, int slot) const {
    const SpellDragState* drag = getActiveDrag();
    return drag && drag->active && drag->source == sourceStorage && drag->slot == slot;
}

void GuiSpellStorage::beginSlotDrag(int slotIndex, const Rectangle& slotRect, Vector2 mousePos) {
    if (!config.interactive || !storage) return;
    if (slotIndex < 0 || slotIndex >= storage->getCapacity()) return;

    Spell* spell = storage->getSpell(slotIndex);
    if (!spell) return;

    dragState.active = true;
    dragState.source = storage;
    dragState.slot = slotIndex;
    dragState.offset = {mousePos.x - slotRect.x, mousePos.y - slotRect.y};
    dragState.mousePos = mousePos;
    dragState.slotSize = slotRect.width;
    activeDragOwner = this;
}

void GuiSpellStorage::endDrag() {
    if (isDragOwner()) {
        dragState = SpellDragState{};
        activeDragOwner = nullptr;
    }
}

void GuiSpellStorage::updateDragMouse(Vector2 mousePos) {
    if (isDragOwner()) {
        dragState.mousePos = mousePos;
    }
}

float GuiSpellStorage::updateGridLayout(Vector2 origin, float maxRowWidth) const {
    cachedSlotRects.clear();

    if (!storage || config.slotSize <= 0.0f) {
        cachedGridArea = {origin.x, origin.y, 0.0f, 0.0f};
        return 0.0f;
    }

    int totalSlots = storage->getCapacity();
    if (totalSlots <= 0) {
        cachedGridArea = {origin.x, origin.y, 0.0f, 0.0f};
        return 0.0f;
    }

    float slotSize = config.slotSize;
    float padding = config.padding;
    int columns = std::max(1, (int)std::floor((maxRowWidth + padding) / (slotSize + padding)));
    int rows = std::max(1, (int)std::ceil((float)totalSlots / (float)columns));

    float slotsWidth = columns * (slotSize + padding) - padding;
    float slotsHeight = rows * (slotSize + padding) - padding;
    cachedGridArea = {origin.x, origin.y, slotsWidth, slotsHeight};

    for (int i = 0; i < totalSlots; ++i) {
        int col = i % columns;
        int row = i / columns;
        Rectangle rect{
            origin.x + col * (slotSize + padding),
            origin.y + row * (slotSize + padding),
            slotSize,
            slotSize};

        cachedSlotRects.push_back(rect);
    }

    return slotsHeight;
}

Rectangle GuiSpellStorage::computeGridArea(Vector2 origin, float maxRowWidth) const {
    updateGridLayout(origin, maxRowWidth);
    return cachedGridArea;
}

Rectangle GuiSpellStorage::getSlotRect(Vector2 origin, float maxRowWidth, int index) const {
    updateGridLayout(origin, maxRowWidth);
    if (index < 0 || index >= (int)cachedSlotRects.size()) return {0, 0, 0, 0};
    return cachedSlotRects[index];
}

int GuiSpellStorage::slotAtPosition(Vector2 origin, float maxRowWidth, Vector2 pos) const {
    updateGridLayout(origin, maxRowWidth);
    for (int i = 0; i < (int)cachedSlotRects.size(); ++i) {
        if (CheckCollisionPointRec(pos, cachedSlotRects[i])) return i;
    }
    return -1;
}

float GuiSpellStorage::renderGrid(Vector2 origin, float maxRowWidth) const {
    float slotsHeight = updateGridLayout(origin, maxRowWidth);

    if (!storage) return slotsHeight;

    for (int i = 0; i < (int)cachedSlotRects.size(); ++i) {
        Rectangle rect = cachedSlotRects[i];

        const Spell* spell = storage->getSpell(i);
        bool hiddenByDrag = config.interactive && isDraggingSpellFrom(storage, i);

        Color fill = SPELL_SLOT_COLOR;
        if (spell && !hiddenByDrag) {
            fill = GetSpellColor(spell);
        } else if (spell) {
            fill = SPELL_SLOT_OCCUPIED_COLOR;
        }

        DrawRectangleRec(rect, Fade(fill, config.alpha));
        DrawRectangleLinesEx(rect, SPELL_SLOT_BORDER, Fade(SPELL_SLOT_OUTLINE_COLOR, config.alpha));
        if (spell && !hiddenByDrag) {
            DrawSpellLabelFitted(spell, rect, config.baseFontSize, Fade(GRAY, config.alpha));
        }
    }

    return slotsHeight;
}

void GuiSpellStorage::drawDraggedSpellSprite() const {
    const SpellDragState* state = getActiveDrag();
    if (!state || !state->active || !state->source) return;

    Spell* spell = state->source->getSpell(state->slot);
    if (!spell) return;

    Rectangle rect = {
        state->mousePos.x - state->slotSize * 0.5f,
        state->mousePos.y - state->slotSize * 0.5f,
        state->slotSize,
        state->slotSize
    };

    Color fill = GetSpellColor(spell);
    DrawRectangleRec(rect, fill);
    DrawRectangleLinesEx(rect, SPELL_SLOT_BORDER, SPELL_SLOT_OUTLINE_COLOR);
    DrawSpellLabelFitted(spell, rect, 14, GRAY);
}

