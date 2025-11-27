#include "gui/items/HeldWandGui.h"

#include "guns/Wand.h"
#include "guns/CastTypes.h"
#include <algorithm>
#include <cmath>
#include <string>

HeldWandGui::HeldWandGui(Player* player) : player(player) {}

HeldWandGui::PanelLayout HeldWandGui::computeLayout() const {
    PanelLayout layout{};
    if (!player) return layout;
    const Wand* wand = player->getSelectedWand();
    if (!wand) return layout;

    const float padding = HELD_WAND_PANEL_PADDING;
    const float statsSpacing = HELD_WAND_STATS_SPACING;
    const float valueSpacing = HELD_WAND_VALUE_SPACING;
    const float betweenRows = HELD_WAND_BETWEEN_ROWS;
    const float textureSize = HELD_WAND_TEXTURE_SIZE;
    const float maxWidth = SCREEN_W - 2 * GUI_MARGIN;

    std::vector<std::pair<std::string, std::string>> primaryStats = {
        {"Shuffle", wand->isShuffle() ? "Yes" : "No"},
        {"Spells/Cast", TextFormat("%d", wand->getSpellsPCast())}
    };

    int maxLabel = 0;
    int maxValue = 0;
    int fontSize = HELD_WAND_FONT_SIZE;
    for (const auto& [name, value] : primaryStats) {
        maxLabel = std::max(maxLabel, MeasureText(name.c_str(), fontSize));
        maxValue = std::max(maxValue, MeasureText(value.c_str(), fontSize));
    }

    float statsWidth = (float)maxLabel + valueSpacing + (float)maxValue;
    float statsHeight = primaryStats.size() * (fontSize + statsSpacing) - statsSpacing;
    float topRowHeight = std::max(textureSize, statsHeight);
    float topRowWidth = textureSize + HELD_WAND_TEXTURE_GAP + statsWidth;

    SpellStorage& storage = player->getSelectedWand()->getSpellStorage();
    int totalSlots = storage.getCapacity();
    float slotSize = SPELL_SLOT_SIZE;
    float slotSpacing = SPELL_SLOT_SPACING;

    int columns = std::max(1, (int)std::floor((maxWidth - 2 * padding + slotSpacing) / (slotSize + slotSpacing)));
    int rows = std::max(1, (int)std::ceil((float)totalSlots / (float)columns));
    float slotsWidth = columns * (slotSize + slotSpacing) - slotSpacing;
    float slotsHeight = rows * (slotSize + slotSpacing) - slotSpacing;

    float panelWidth = std::max(topRowWidth, slotsWidth) + 2 * padding;
    panelWidth = std::min(panelWidth, maxWidth);
    float panelHeight = padding + topRowHeight + betweenRows + slotsHeight + padding;

    layout.panel = {GUI_MARGIN, GUI_MARGIN + WAND_SLOT_SIZE + HELD_WAND_PANEL_OFFSET_Y, panelWidth, panelHeight};

    layout.textureRect = {layout.panel.x + padding,
                          layout.panel.y + padding + (topRowHeight - textureSize) * 0.5f,
                          textureSize, textureSize};
    layout.statsStartX = layout.textureRect.x + textureSize + HELD_WAND_TEXTURE_GAP;
    layout.statsStartY = layout.panel.y + padding;
    layout.valueColumnX = layout.statsStartX + (float)maxLabel + valueSpacing;

    layout.spellSlots.reserve(totalSlots);
    float startX = layout.panel.x + padding;
    float startY = layout.panel.y + padding + topRowHeight + betweenRows;
    for (int i = 0; i < totalSlots; ++i) {
        int col = i % columns;
        int row = i / columns;
        Rectangle rect{startX + col * (slotSize + slotSpacing), startY + row * (slotSize + slotSpacing), slotSize, slotSize};
        layout.spellSlots.push_back(rect);
    }

    return layout;
}

void HeldWandGui::drawBackground(const PanelLayout& layout) const {
    DrawRectangleRec(layout.panel, HELD_WAND_PANEL_BG);
    DrawRectangleLinesEx(layout.panel, HELD_WAND_PANEL_BORDER, HELD_WAND_PANEL_OUTLINE);
}

void HeldWandGui::drawPrimaryStats(const PanelLayout& layout, const Wand& wand) const {
    int fontSize = HELD_WAND_FONT_SIZE;
    Color fontColor = HELD_WAND_FONT_COLOR;
    std::vector<std::pair<std::string, std::string>> primaryStats = {
        {"Shuffle", wand.isShuffle() ? "Yes" : "No"},
        {"Spells/Cast", TextFormat("%d", wand.getSpellsPCast())}
    };

    float y = layout.statsStartY;
    for (const auto& [name, value] : primaryStats) {
        DrawText(name.c_str(), (int)layout.statsStartX, (int)y, fontSize, fontColor);
        DrawText(value.c_str(), (int)layout.valueColumnX, (int)y, fontSize, fontColor);
        y += fontSize + HELD_WAND_STATS_SPACING;
    }
}

void HeldWandGui::drawSpellSlots(const PanelLayout& layout, SpellStorage& storage) const {
    const SpellDragState& drag = GetSpellDragState();
    for (int i = 0; i < (int)layout.spellSlots.size(); ++i) {
        Rectangle rect = layout.spellSlots[i];
        const Spell* spell = storage.getSpell(i);
        bool hiddenByDrag = IsDraggingSpellFrom(drag, &storage, i);

        Color fill = SPELL_SLOT_COLOR;
        if (spell && !hiddenByDrag) {
            fill = GetSpellColor(spell);
        } else if (spell) {
            fill = SPELL_SLOT_OCCUPIED_COLOR;
        }

        DrawRectangleRec(rect, fill);
        DrawRectangleLinesEx(rect, SPELL_SLOT_BORDER, SPELL_SLOT_OUTLINE_COLOR);
        if (spell && !hiddenByDrag) {
            DrawSpellLabelFitted(spell, rect, 16, GRAY);
        }
    }
}

Rectangle HeldWandGui::getSlotRect(const PanelLayout& layout, int index) const {
    if (index < 0 || index >= (int)layout.spellSlots.size()) return {0, 0, 0, 0};
    return layout.spellSlots[index];
}

int HeldWandGui::slotAtPosition(const PanelLayout& layout, Vector2 pos) const {
    for (int i = 0; i < (int)layout.spellSlots.size(); ++i) {
        if (CheckCollisionPointRec(pos, layout.spellSlots[i])) return i;
    }
    return -1;
}

void HeldWandGui::update(float dt, Vector2 virtualMousePos) {
    (void)dt;
    if (!player) return;
    Wand* wand = player->getSelectedWand();
    if (!wand) return;

    PanelLayout layout = computeLayout();
    SpellStorage& storage = wand->getSpellStorage();

    UpdateDragMouse(virtualMousePos);
    SpellDragState& drag = GetSpellDragState();
    lastMouse = virtualMousePos;

    if (!drag.active && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        int hit = slotAtPosition(layout, virtualMousePos);
        if (hit >= 0 && storage.getSpell(hit)) {
            Rectangle slot = getSlotRect(layout, hit);
            StartSpellDrag(&storage, hit, slot, virtualMousePos, slot.width);
        }
    }

    if (drag.active && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
        int target = slotAtPosition(layout, virtualMousePos);
        if (target >= 0) {
            SpellStorage* source = drag.source;
            if (source == &storage) {
                storage.swapSpells(drag.slot, target);
            } else if (source) {
                source->swapSpells(storage, drag.slot, target);
            }
        }
        EndSpellDrag();
    }
}

static std::string formatFloat(float value) {
    return TextFormat("%.2f", value);
}

void HeldWandGui::drawPreviewStats(Rectangle area, const Wand& wand) const {
    int fontSize = HELD_WAND_PREVIEW_FONT_SIZE;
    Color color = HELD_WAND_FONT_COLOR;
    float lineHeight = fontSize + HELD_WAND_STATS_SPACING;

    CastContext ctx = wand.getBaseContext();
    ProjectileStats proj = wand.getBaseProjectile();

    std::vector<std::pair<std::string, std::string>> lines = {
        {"Name", wand.getName()},
        {"Shuffle", ctx.shuffle ? "Yes" : "No"},
        {"Spells/Cast", TextFormat("%d", ctx.remainingDraw)},
        {"Cast delay", TextFormat("%s s", formatFloat(ctx.castDelay).c_str())},
        {"Recharge Time", TextFormat("%s s", formatFloat(ctx.rechargeTime).c_str())},
        {"Mana Max", TextFormat("%d", (int)wand.getMaxMana())},
        {"Mana Charge Speed", TextFormat("%d", (int)wand.getManaChargeSpeed())},
        {"Capacity", TextFormat("%d", ctx.remainingCapacity)}
    };

    if (std::abs(proj.spread - ProjectileStats{}.spread) > 0.0001f)
        lines.emplace_back("Spread", TextFormat("%s °", formatFloat(proj.spread).c_str()));
    if (std::abs(proj.damage - ProjectileStats{}.damage) > 0.0001f)
        lines.emplace_back("Damage", TextFormat("+ %s", formatFloat(proj.damage).c_str()));
    if (std::abs(proj.speed - ProjectileStats{}.speed) > 0.0001f)
        lines.emplace_back("Speed", TextFormat("x %s", formatFloat(proj.speed).c_str()));
    if (std::abs(proj.knockback - ProjectileStats{}.knockback) > 0.0001f)
        lines.emplace_back("Knockback", TextFormat("%s", formatFloat(proj.knockback).c_str()));
    if (std::abs(proj.lifetime - ProjectileStats{}.lifetime) > 0.0001f)
        lines.emplace_back("lifetime", TextFormat("+ %s s", formatFloat(proj.lifetime).c_str()));
    if (std::abs(proj.size - ProjectileStats{}.size) > 0.0001f)
        lines.emplace_back("size", TextFormat("x %s", formatFloat(proj.size).c_str()));

    float labelMax = 0.0f;
    for (const auto& [label, _] : lines) {
        labelMax = std::max(labelMax, (float)MeasureText(label.c_str(), fontSize));
    }

    float labelX = area.x + HELD_WAND_PANEL_PADDING;
    float valueX = labelX + labelMax + HELD_WAND_VALUE_SPACING;
    float y = area.y + HELD_WAND_PANEL_PADDING;

    for (const auto& [label, value] : lines) {
        DrawText(label.c_str(), (int)labelX, (int)y, fontSize, color);
        DrawText(value.c_str(), (int)valueX, (int)y, fontSize, color);
        y += lineHeight;
    }
}

void HeldWandGui::drawPreviewSpells(const Rectangle& area, SpellStorage& storage, float slotSize) const {
    float padding = HELD_WAND_PANEL_PADDING;
    float spacing = SPELL_SLOT_SPACING;
    int total = storage.getCapacity();

    int columns = std::max(1, (int)std::floor((area.width - 2 * padding + spacing) / (slotSize + spacing)));
    float startX = area.x + padding;
    float startY = area.y;

    for (int i = 0; i < total; ++i) {
        int row = i / columns;
        int col = i % columns;
        Rectangle rect{startX + col * (slotSize + spacing), startY + row * (slotSize + spacing), slotSize, slotSize};
        const Spell* spell = storage.getSpell(i);

        Color fill = SPELL_SLOT_COLOR;
        if (spell) fill = GetSpellColor(spell);

        DrawRectangleRec(rect, fill);
        DrawRectangleLinesEx(rect, SPELL_SLOT_BORDER, SPELL_SLOT_OUTLINE_COLOR);
        if (spell) {
            DrawSpellLabelFitted(spell, rect, 14, GRAY);
        }
    }
}

void HeldWandGui::drawPreview(const PanelLayout& layout, const Wand& wand, SpellStorage& storage) const {
    float previewX = layout.panel.x + layout.panel.width + HELD_WAND_PREVIEW_OFFSET_X;
    float previewY = layout.panel.y;
    previewX = std::min(previewX, SCREEN_W - HELD_WAND_PREVIEW_WIDTH - GUI_MARGIN);
    previewX = std::max(previewX, layout.panel.x + HELD_WAND_PANEL_PADDING);

    Rectangle preview = {
        previewX,
        previewY,
        HELD_WAND_PREVIEW_WIDTH,
        HELD_WAND_PREVIEW_HEIGHT
    };

    DrawRectangleRec(preview, HELD_WAND_PANEL_BG);
    DrawRectangleLinesEx(preview, HELD_WAND_PANEL_BORDER, HELD_WAND_PANEL_OUTLINE);

    float textureAreaWidth = HELD_WAND_TEXTURE_SIZE;
    Rectangle statsArea = {
        preview.x,
        preview.y,
        preview.width - textureAreaWidth - HELD_WAND_TEXTURE_GAP,
        preview.height * 0.6f
    };
    drawPreviewStats(statsArea, wand);

    Texture2D tex = wand.getTexture();
    float maxH = statsArea.height;
    float maxW = textureAreaWidth;
    float scale = std::min(maxW / tex.width, maxH / tex.height);
    Rectangle dest = {
        statsArea.x + statsArea.width + HELD_WAND_TEXTURE_GAP + maxW * 0.5f,
        statsArea.y + maxH * 0.5f,
        tex.width * scale,
        tex.height * scale
    };
    DrawTexturePro(tex, {0, 0, (float)tex.width, (float)tex.height}, dest, {dest.width * 0.5f, dest.height * 0.5f}, -90.0f, WHITE);

    Rectangle spellsArea = {
        preview.x,
        statsArea.y + statsArea.height + HELD_WAND_PREVIEW_SPELL_GAP,
        preview.width,
        preview.height - statsArea.height - HELD_WAND_PREVIEW_SPELL_GAP
    };
    drawPreviewSpells(spellsArea, storage, SPELL_SLOT_SIZE * HELD_WAND_PREVIEW_SLOT_SCALE);
}

void HeldWandGui::draw() const {
    if (!player) return;
    Wand* wand = player->getSelectedWand();
    if (!wand) return;

    PanelLayout layout = computeLayout();
    SpellStorage& storage = wand->getSpellStorage();

    drawBackground(layout);

    Texture2D tex = wand->getTexture();
    float scale = std::min(layout.textureRect.width / tex.width, layout.textureRect.height / tex.height);
    Rectangle dest = {layout.textureRect.x, layout.textureRect.y, tex.width * scale, tex.height * scale};
    DrawTexturePro(tex, {0, 0, (float)tex.width, (float)tex.height}, dest, {0, 0}, 0.0f, WHITE);

    drawPrimaryStats(layout, *wand);
    drawSpellSlots(layout, storage);

    if (CheckCollisionPointRec(lastMouse, layout.panel)) {
        drawPreview(layout, *wand, storage);
    }

    DrawDraggedSpellSprite(GetSpellDragState());
}
