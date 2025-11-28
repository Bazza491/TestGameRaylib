#pragma once

#include "gui/GuiItem.h"
#include "gui/GuiConstants.h"
#include "gui/SpellSlotUtils.h"
#include "guns/SpellStorage.h"

struct SpellDragState {
    bool active = false;
    SpellStorage* source = nullptr;
    int slot = -1;
    Vector2 offset{0.0f, 0.0f};
    Vector2 mousePos{0.0f, 0.0f};
    float slotSize = SPELL_SLOT_SIZE;
};

class GuiSpellStorage : public GuiItem {
public:
    struct Config {
        Config() = default;
        float slotSize = SPELL_SLOT_SIZE;
        float padding = SPELL_SLOT_SPACING;
        float alpha = 1.0f;
        bool interactive = true;
        int baseFontSize = 16;
    };

private:
    SpellStorage* storage = nullptr;
    Config config{};
    SpellDragState dragState{};

    static GuiSpellStorage* activeDragOwner;

    [[nodiscard]] bool isDragOwner() const;
    [[nodiscard]] bool isDraggingSpellFrom(const SpellStorage* sourceStorage, int slot) const;

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

    float renderGrid(Vector2 origin, float maxRowWidth) const;
    void drawDraggedSpellSprite() const;

    static const SpellDragState* getActiveDrag();
    static GuiSpellStorage* getActiveDragOwner();
};

