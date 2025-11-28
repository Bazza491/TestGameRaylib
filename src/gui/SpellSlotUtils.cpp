#include "gui/SpellSlotUtils.h"

#include "guns/Spell.h"
#include <algorithm>
#include <cmath>
#include <sstream>
#include <string>
#include <vector>

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
