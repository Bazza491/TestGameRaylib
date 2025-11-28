#pragma once

#include "gui/GuiItem.h"
#include "gui/GuiConstants.h"
#include "gui/SpellSlotUtils.h"
#include "guns/SpellStorage.h"

#include <vector>

struct SpellDragState {
    bool active = false;
    SpellStorage* source = nullptr;
    int slot = -1;
    Vector2 offset{0.0f, 0.0f};
    Vector2 mousePos{0.0f, 0.0f};
    float slotSize = SPELL_SLOT_SIZE;
};
struct Config {
    float slotSize = SPELL_SLOT_SIZE;
    float padding = SPELL_SLOT_SPACING;
    float alpha = 1.0f;
    bool interactive = true;
    int baseFontSize = 16;
};

class GuiSpellStorage : public GuiItem {
private:
    SpellStorage* storage = nullptr;
    Config config{};
    SpellDragState dragState{};
    mutable std::vector<Rectangle> cachedSlotRects{};
    mutable Rectangle cachedGridArea{0, 0, 0, 0};

    static GuiSpellStorage* activeDragOwner;

    [[nodiscard]] bool isDragOwner() const;
    [[nodiscard]] bool isDraggingSpellFrom(const SpellStorage* sourceStorage, int slot) const;
    float updateGridLayout(Vector2 origin, float maxRowWidth) const;

public:
    GuiSpellStorage(SpellStorage* storage, Config cfg = Config{});

    void update(float dt, Vector2 virtualMousePos) override;
    void draw() const override;

    [[nodiscard]] SpellStorage* getStorage() const;
    void setStorage(SpellStorage* newStorage);

    void setSlotSize(float size);
    void setPadding(float pad);
    void setAlpha(float newAlpha);
    void setInteractive(bool enabled);
    void setBaseFontSize(int size);

    void beginSlotDrag(int slotIndex, const Rectangle& slotRect, Vector2 mousePos);
    void endDrag();
    void updateDragMouse(Vector2 mousePos);

    [[nodiscard]] Rectangle computeGridArea(Vector2 origin, float maxRowWidth) const;
    [[nodiscard]] Rectangle getSlotRect(Vector2 origin, float maxRowWidth, int index) const;
    [[nodiscard]] int slotAtPosition(Vector2 origin, float maxRowWidth, Vector2 pos) const;
    float renderGrid(Vector2 origin, float maxRowWidth) const;
    void drawDraggedSpellSprite() const;

    static const SpellDragState* getActiveDrag();
    static GuiSpellStorage* getActiveDragOwner();
};

