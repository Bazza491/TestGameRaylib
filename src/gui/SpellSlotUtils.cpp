#include "gui/SpellSlotUtils.h"

#include "guns/Spell.h"
#include <algorithm>
#include <cmath>
#include <sstream>
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

static std::vector<std::string> WrapLabel(const std::string& text, int fontSize, float maxWidth) {
    std::vector<std::string> lines;
    std::istringstream iss(text);
    std::string word;
    std::string current;

    while (iss >> word) {
        std::string trial = current.empty() ? word : current + " " + word;
        if ((float)MeasureText(trial.c_str(), fontSize) <= maxWidth) {
            current = trial;
        } else {
            if (!current.empty()) {
                lines.push_back(current);
                current.clear();
            }

            // If a single word is too long, break it up.
            while ((float)MeasureText(word.c_str(), fontSize) > maxWidth && word.size() > 1) {
                std::string chunk;
                for (char c : word) {
                    std::string next = chunk + c;
                    if ((float)MeasureText(next.c_str(), fontSize) > maxWidth && !chunk.empty()) {
                        lines.push_back(chunk);
                        next = std::string(1, c);
                    }
                    chunk = next;
                }
                current = chunk;
                word.clear();
            }

            if (!word.empty()) {
                current = word;
            }
        }
    }

    if (!current.empty()) {
        lines.push_back(current);
    }

    if (lines.empty()) lines.push_back("");
    return lines;
}

void DrawSpellLabelFitted(const Spell* spell, const Rectangle& rect, int baseFontSize, Color color) {
    if (!spell) return;
    const std::string& label = spell->getName();
    float maxWidth = rect.width - 4.0f;
    float maxHeight = rect.height - 4.0f;
    if (maxWidth <= 0 || maxHeight <= 0) return;

    float fontSize = (float)baseFontSize;
    std::vector<std::string> lines;
    for (int i = 0; i < 3; ++i) {
        lines = WrapLabel(label, (int)fontSize, maxWidth);
        float lineHeight = fontSize + 2.0f;
        float totalHeight = lineHeight * (float)lines.size();
        float widest = 0.0f;
        for (const auto& ln : lines) {
            widest = std::max(widest, (float)MeasureText(ln.c_str(), (int)fontSize));
        }

        if (widest <= maxWidth && totalHeight <= maxHeight) break;

        float scaleW = maxWidth / std::max(widest, 1.0f);
        float scaleH = maxHeight / std::max(totalHeight, 1.0f);
        float scale = std::min(scaleW, scaleH);
        fontSize = std::max(8.0f, fontSize * scale);
    }

    float lineHeight = fontSize + 2.0f;
    float totalHeight = lineHeight * (float)lines.size();
    float startY = rect.y + (rect.height - totalHeight) * 0.5f;

    for (size_t i = 0; i < lines.size(); ++i) {
        const std::string& ln = lines[i];
        float textWidth = (float)MeasureText(ln.c_str(), (int)fontSize);
        float textX = rect.x + (rect.width - textWidth) * 0.5f;
        float textY = startY + lineHeight * (float)i;
        DrawText(ln.c_str(), (int)textX, (int)textY, (int)fontSize, color);
    }
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

float RenderSpellSlotGrid(
    SpellStorage& storage,
    Vector2 origin,
    float maxRowWidth,
    float slotSize,
    float padding,
    bool interactive,
    float alpha,
    int baseFontSize) {
    int totalSlots = storage.getCapacity();
    if (totalSlots <= 0 || slotSize <= 0.0f) return 0.0f;

    int columns = std::max(1, (int)std::floor((maxRowWidth + padding) / (slotSize + padding)));
    int rows = std::max(1, (int)std::ceil((float)totalSlots / (float)columns));

    const SpellDragState& drag = GetSpellDragState();

    for (int i = 0; i < totalSlots; ++i) {
        int col = i % columns;
        int row = i / columns;
        Rectangle rect{
            origin.x + col * (slotSize + padding),
            origin.y + row * (slotSize + padding),
            slotSize,
            slotSize};

        const Spell* spell = storage.getSpell(i);
        bool hiddenByDrag = interactive && IsDraggingSpellFrom(drag, &storage, i);

        Color fill = SPELL_SLOT_COLOR;
        if (spell && !hiddenByDrag) {
            fill = GetSpellColor(spell);
        } else if (spell) {
            fill = SPELL_SLOT_OCCUPIED_COLOR;
        }

        DrawRectangleRec(rect, Fade(fill, alpha));
        DrawRectangleLinesEx(rect, SPELL_SLOT_BORDER, Fade(SPELL_SLOT_OUTLINE_COLOR, alpha));
        if (spell && !hiddenByDrag) {
            DrawSpellLabelFitted(spell, rect, baseFontSize, Fade(GRAY, alpha));
        }
    }

    return std::max(0.0f, rows * (slotSize + padding) - padding);
}
