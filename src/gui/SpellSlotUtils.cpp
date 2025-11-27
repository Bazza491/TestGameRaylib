#include "gui/SpellSlotUtils.h"

#include "guns/Spell.h"
#include <algorithm>
#include <string>

static SpellDragState dragState;

SpellDragState& GetSpellDragState() {
    return dragState;
}

void StartSpellDrag(SpellStorage* storage, int slot, const Rectangle& slotRect, Vector2 mousePos, float slotSize) {
    dragState.active = true;
    dragState.source = storage;
    dragState.slot = slot;
    dragState.offset = {mousePos.x - slotRect.x, mousePos.y - slotRect.y};
    dragState.mousePos = mousePos;
    dragState.slotSize = slotSize;
}

void EndSpellDrag() {
    dragState = SpellDragState{};
}

void UpdateDragMouse(Vector2 mousePos) {
    dragState.mousePos = mousePos;
}

bool IsDraggingSpellFrom(const SpellDragState& state, const SpellStorage* storage, int slot) {
    return state.active && state.source == storage && state.slot == slot;
}

Color GetSpellColor(const Spell* spell) {
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

void DrawSpellLabelFitted(const Spell* spell, const Rectangle& rect, int baseFontSize, Color color) {
    if (!spell) return;
    const std::string& label = spell->getName();
    int maxWidth = (int)(rect.width - 4.0f);
    if (maxWidth <= 0) return;

    int textWidth = MeasureText(label.c_str(), baseFontSize);
    float fontSize = (float)baseFontSize;
    if (textWidth > maxWidth && textWidth > 0) {
        float scale = (float)maxWidth / (float)textWidth;
        fontSize = std::max(8.0f, fontSize * scale);
    }

    int fitWidth = MeasureText(label.c_str(), (int)fontSize);
    float textX = rect.x + (rect.width - (float)fitWidth) * 0.5f;
    float textY = rect.y + rect.height - fontSize - 4.0f;
    DrawText(label.c_str(), (int)textX, (int)textY, (int)fontSize, color);
}

void DrawDraggedSpellSprite(const SpellDragState& state) {
    if (!state.active || !state.source) return;
    Spell* spell = state.source->getSpell(state.slot);
    if (!spell) return;

    Rectangle rect = {
        state.mousePos.x - state.slotSize * 0.5f,
        state.mousePos.y - state.slotSize * 0.5f,
        state.slotSize,
        state.slotSize
    };

    Color fill = GetSpellColor(spell);
    DrawRectangleRec(rect, fill);
    DrawRectangleLinesEx(rect, SPELL_SLOT_BORDER, SPELL_SLOT_OUTLINE_COLOR);
    DrawSpellLabelFitted(spell, rect, 14, GRAY);
}
