#pragma once

#include "raylib.h"
#include "gui/GuiConstants.h"
#include "guns/SpellStorage.h"

struct SpellDragState {
    bool active = false;
    SpellStorage* source = nullptr;
    int slot = -1;
    Vector2 offset{0.0f, 0.0f};
    Vector2 mousePos{0.0f, 0.0f};
    float slotSize = SPELL_SLOT_SIZE;
};

SpellDragState& GetSpellDragState();

void StartSpellDrag(SpellStorage* storage, int slot, const Rectangle& slotRect, Vector2 mousePos, float slotSize);
void EndSpellDrag();
void UpdateDragMouse(Vector2 mousePos);

bool IsDraggingSpellFrom(const SpellDragState& state, const SpellStorage* storage, int slot);

Color GetSpellColor(const Spell* spell);
void DrawSpellLabelFitted(const Spell* spell, const Rectangle& rect, int baseFontSize, Color color);
void DrawDraggedSpellSprite(const SpellDragState& dragState);
