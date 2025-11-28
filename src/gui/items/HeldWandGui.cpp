#include "gui/items/HeldWandGui.h"

#include "guns/Wand.h"
#include "guns/CastTypes.h"
#include "gui/items/WandInventory.h"
#include <algorithm>
#include <cmath>
#include <string>

HeldWandGui::HeldWandGui(Player* player)
    : player(player), slotsGui(nullptr), previewSlotsGui(nullptr) {}

HeldWandGui::PanelLayout HeldWandGui::computeLayout() const {
    PanelLayout layout{};
    if (!player) return layout;
    const Wand* wand = player->getSelectedWand();
    if (!wand) return layout;

    const float padding = HELD_WAND_PANEL_PADDING;
    const float statsSpacing = HELD_WAND_STATS_SPACING;
    const float valueSpacing = HELD_WAND_VALUE_SPACING;
    const float betweenRows = HELD_WAND_BETWEEN_ROWS;
    const float maxWidth = SCREEN_W - 2 * GUI_MARGIN;

    Texture2D tex = wand->getTexture();
    const float textureWidth = HELD_WAND_TEXTURE_SIZE;
    const float textureHeight = textureWidth * ((float)tex.height / std::max(1.0f, (float)tex.width));

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
    float topRowHeight = std::max(textureHeight, statsHeight);
    float topRowWidth = textureWidth + HELD_WAND_TEXTURE_GAP + statsWidth;

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

    float spellInvHeight = 2 * SPELL_SLOT_SIZE + SPELL_SLOT_SPACING;
    float baseY = GUI_MARGIN + std::max(WAND_SLOT_SIZE, spellInvHeight) + HELD_WAND_PANEL_OFFSET_Y;

    layout.panel = {GUI_MARGIN, baseY, panelWidth, panelHeight};

    layout.textureRect = {layout.panel.x + padding,
                          layout.panel.y + padding,
                          textureWidth, topRowHeight};
    layout.statsStartX = layout.textureRect.x + textureWidth + HELD_WAND_TEXTURE_GAP;
    layout.statsStartY = layout.panel.y + padding + (topRowHeight - statsHeight) * 0.5f;
    layout.valueColumnX = layout.statsStartX + (float)maxLabel + valueSpacing;
    layout.topRowHeight = topRowHeight;

    layout.spellSlots.reserve(totalSlots);
    float startX = layout.panel.x + padding;
    float startY = layout.panel.y + padding + topRowHeight + betweenRows;
    layout.spellArea = {startX, startY, slotsWidth, slotsHeight};
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
    Vector2 origin{layout.spellArea.x, layout.spellArea.y};
    slotsGui.setStorage(&storage);
    slotsGui.setSlotSize(SPELL_SLOT_SIZE);
    slotsGui.setPadding(SPELL_SLOT_SPACING);
    slotsGui.setInteractive(true);
    slotsGui.setAlpha(1.0f);
    slotsGui.setBaseFontSize(16);
    slotsGui.renderGrid(origin, layout.spellArea.width);
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
    if (!player) return;
    Wand* wand = player->getSelectedWand();
    if (!wand) return;

    PanelLayout layout = computeLayout();
    SpellStorage& storage = wand->getSpellStorage();

    slotsGui.setStorage(&storage);
    slotsGui.update(dt, virtualMousePos);

    const SpellDragState* drag = GuiSpellStorage::getActiveDrag();

    if ((!drag || !drag->active) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        int hit = slotAtPosition(layout, virtualMousePos);
        if (hit >= 0 && storage.getSpell(hit)) {
            Rectangle slot = getSlotRect(layout, hit);
            slotsGui.beginSlotDrag(hit, slot, virtualMousePos);
        }
    }

    drag = GuiSpellStorage::getActiveDrag();
    if (drag && drag->active && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
        int target = slotAtPosition(layout, virtualMousePos);
        if (target >= 0) {
            SpellStorage* source = drag->source;
            if (source == &storage) {
                storage.swapSpells(drag->slot, target);
            } else if (source) {
                source->swapSpells(storage, drag->slot, target);
            }
        }
        slotsGui.endDrag();
    }

    bool overSpells = CheckCollisionPointRec(virtualMousePos, layout.spellArea);

    const SpellStorage& inv = player->getSpellInventory();
    int invColumns = 8;
    int invRows = std::max(1, (int)std::ceil((float)inv.getCapacity() / (float)invColumns));
    float invWidth = invColumns * SPELL_SLOT_SIZE + SPELL_SLOT_SPACING * (invColumns - 1);
    float invHeight = invRows * (SPELL_SLOT_SIZE + SPELL_SLOT_SPACING) - SPELL_SLOT_SPACING;

    float wandCount = (float)player->getWandSlots().size();
    float wandWidth = (wandCount > 0)
                          ? wandCount * (WAND_SLOT_SIZE + WAND_SLOT_SPACING) - WAND_SLOT_SPACING
                          : 0.0f;
    Rectangle invBounds = {GUI_MARGIN + wandWidth + GUI_MARGIN, GUI_MARGIN, invWidth, invHeight};

    bool overInventorySlots = CheckCollisionPointRec(virtualMousePos, invBounds);
    bool draggingAnything = (drag && drag->active) || IsAnyWandDragging();

    previewAllowed = CheckCollisionPointRec(virtualMousePos, layout.panel) && !overSpells && !overInventorySlots && !draggingAnything;
    if (previewAllowed) {
        previewAlpha = std::min(1.0f, previewAlpha + dt * HELD_WAND_PREVIEW_FADE_SPEED);
    } else {
        previewAlpha = 0.0f;
    }
}

static std::string formatFloat(float value) {
    return TextFormat("%.2f", value);
}

void HeldWandGui::drawPreviewSpells(const Rectangle& area, SpellStorage& storage, float slotSize, float alpha) const {
    float padding = HELD_WAND_PANEL_PADDING;
    Vector2 origin{area.x + padding, area.y};
    float availableWidth = area.width - 2 * padding;

    previewSlotsGui.setStorage(&storage);
    previewSlotsGui.setSlotSize(slotSize);
    previewSlotsGui.setPadding(SPELL_SLOT_SPACING);
    previewSlotsGui.setInteractive(false);
    previewSlotsGui.setAlpha(alpha);
    previewSlotsGui.setBaseFontSize(14);

    previewSlotsGui.renderGrid(origin, availableWidth);
}

void HeldWandGui::drawPreview(const PanelLayout& layout, const Wand& wand, SpellStorage& storage, float alpha) const {
    const float padding = HELD_WAND_PANEL_PADDING;
    const float slotSize = SPELL_SLOT_SIZE * HELD_WAND_PREVIEW_SLOT_SCALE;
    const float spacing = SPELL_SLOT_SPACING;
    const int titleSize = HELD_WAND_PREVIEW_TITLE_SIZE;
    const int fontSize = HELD_WAND_PREVIEW_FONT_SIZE;
    const float lineHeight = fontSize + HELD_WAND_STATS_SPACING;

    CastContext ctx = wand.getBaseContext();
    ProjectileStats proj = wand.getBaseProjectile();

    std::vector<std::pair<std::string, std::string>> lines = {
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
    float valueMax = 0.0f;
    for (const auto& [label, value] : lines) {
        labelMax = std::max(labelMax, (float)MeasureText(label.c_str(), fontSize));
        valueMax = std::max(valueMax, (float)MeasureText(value.c_str(), fontSize));
    }

    float nameHeight = (float)titleSize;
    float statsHeight = lines.size() * lineHeight;
    float statsBlockHeight = nameHeight + HELD_WAND_PREVIEW_NAME_GAP + statsHeight;
    Texture2D tex = wand.getTexture();
    float maxTexW = HELD_WAND_TEXTURE_SIZE;
    float maxTexH = HELD_WAND_TEXTURE_SIZE;
    float texScale = std::min(maxTexW / std::max(1.0f, (float)tex.height), maxTexH / std::max(1.0f, (float)tex.width));
    float texDrawW = tex.width * texScale;
    float texDrawH = tex.height * texScale;

    float statsWidth = labelMax + HELD_WAND_VALUE_SPACING + valueMax;
    float baseWidth = padding + statsWidth + HELD_WAND_TEXTURE_GAP + maxTexW + padding;
    float previewWidth = std::max(HELD_WAND_PREVIEW_WIDTH, baseWidth);

    float topHeight = std::max(statsBlockHeight, texDrawH);

    float slotsWidth = previewWidth - 2 * padding;
    int columns = std::max(1, (int)std::floor((slotsWidth + spacing) / (slotSize + spacing)));
    int rows = std::max(1, (int)std::ceil((float)storage.getCapacity() / (float)columns));
    float slotsHeight = rows * (slotSize + spacing) - spacing;

    float desiredHeight = padding + topHeight + HELD_WAND_PREVIEW_SPELL_GAP + slotsHeight + padding;
    float previewHeight = std::max(HELD_WAND_PREVIEW_HEIGHT, desiredHeight);

    float previewX = SCREEN_W - GUI_MARGIN - previewWidth - HELD_WAND_PREVIEW_EDGE_GAP;
    float previewY = SCREEN_H - GUI_MARGIN - previewHeight - HELD_WAND_PREVIEW_EDGE_GAP;

    previewX = std::max(GUI_MARGIN, previewX);
    previewY = std::max(GUI_MARGIN, previewY);

    Rectangle preview = {previewX, previewY, previewWidth, previewHeight};

    DrawRectangleRec(preview, Fade(HELD_WAND_PANEL_BG, alpha));
    DrawRectangleLinesEx(preview, HELD_WAND_PANEL_BORDER, Fade(HELD_WAND_PANEL_OUTLINE, alpha));

    Rectangle statsArea = {
        preview.x + padding,
        preview.y + padding + (topHeight - statsBlockHeight) * 0.5f,
        statsWidth,
        statsBlockHeight
    };

    float labelX = statsArea.x;
    float valueX = labelX + labelMax + HELD_WAND_VALUE_SPACING;
    float textY = statsArea.y;

    DrawText(wand.getName().c_str(), (int)labelX, (int)textY, titleSize, Fade(HELD_WAND_FONT_COLOR, alpha));
    textY += nameHeight + HELD_WAND_PREVIEW_NAME_GAP;

    for (const auto& [label, value] : lines) {
        DrawText(label.c_str(), (int)labelX, (int)textY, fontSize, Fade(HELD_WAND_FONT_COLOR, alpha));
        DrawText(value.c_str(), (int)valueX, (int)textY, fontSize, Fade(HELD_WAND_FONT_COLOR, alpha));
        textY += lineHeight;
    }

    float maxH = topHeight;
    float maxW = maxTexW;
    float scale = std::min(maxW / tex.height, maxH / tex.width);
    Rectangle dest = {
        statsArea.x + statsArea.width + HELD_WAND_TEXTURE_GAP + maxW * 0.5f,
        preview.y + padding + topHeight * 0.5f,
        tex.width * scale,
        tex.height * scale
    };
    DrawTexturePro(tex, {0, 0, (float)tex.width, (float)tex.height}, dest, {dest.width * 0.5f, dest.height * 0.5f}, -90.0f, Fade(WHITE, alpha));

    Rectangle spellsArea = {
        preview.x + padding,
        preview.y + padding + topHeight + HELD_WAND_PREVIEW_SPELL_GAP,
        preview.width - 2 * padding,
        preview.height - (padding + topHeight + HELD_WAND_PREVIEW_SPELL_GAP + padding)
    };
    drawPreviewSpells(spellsArea, storage, slotSize, alpha);
}

void HeldWandGui::draw() const {
    if (!player) return;
    Wand* wand = player->getSelectedWand();
    if (!wand) return;

    PanelLayout layout = computeLayout();
    SpellStorage& storage = wand->getSpellStorage();

    drawBackground(layout);

    Texture2D tex = wand->getTexture();
    float scale = layout.textureRect.width / tex.width;
    float drawHeight = tex.height * scale;
    Rectangle dest = {layout.textureRect.x,
                      layout.textureRect.y + (layout.textureRect.height - drawHeight) * 0.5f,
                      tex.width * scale,
                      drawHeight};
    DrawTexturePro(tex, {0, 0, (float)tex.width, (float)tex.height}, dest, {0, 0}, 0.0f, WHITE);

    drawPrimaryStats(layout, *wand);
    drawSpellSlots(layout, storage);

    if (previewAllowed && previewAlpha > 0.0f) {
        drawPreview(layout, *wand, storage, previewAlpha);
    }

    slotsGui.drawDraggedSpellSprite();
}
