#pragma once

#include "gui/GuiItem.h"
#include "gui/GuiConstants.h"
#include "gui/SpellSlotUtils.h"
#include "Player.h"

#include <vector>

class HeldWandGui : public GuiItem {
private:
    Player* player = nullptr;
    bool previewAllowed = false;
    float previewAlpha = 0.0f;

    struct PanelLayout {
        Rectangle panel{0, 0, 0, 0};
        Rectangle textureRect{0, 0, 0, 0};
        Rectangle spellArea{0, 0, 0, 0};
        float statsStartX = 0.0f;
        float statsStartY = 0.0f;
        float valueColumnX = 0.0f;
        float topRowHeight = 0.0f;
        std::vector<Rectangle> spellSlots;
    };

    [[nodiscard]] PanelLayout computeLayout() const;
    void drawPrimaryStats(const PanelLayout& layout, const Wand& wand) const;
    void drawSpellSlots(const PanelLayout& layout, SpellStorage& storage) const;
    void drawBackground(const PanelLayout& layout) const;
    void drawPreview(const PanelLayout& layout, const Wand& wand, SpellStorage& storage, float alpha) const;
    void drawPreviewSpells(const Rectangle& area, SpellStorage& storage, float slotSize, float alpha) const;
    Rectangle getSlotRect(const PanelLayout& layout, int index) const;
    [[nodiscard]] int slotAtPosition(const PanelLayout& layout, Vector2 pos) const;

public:
    explicit HeldWandGui(Player* player);

    void update(float dt, Vector2 virtualMousePos) override;
    void draw() const override;
};
