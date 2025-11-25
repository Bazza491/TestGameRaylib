#include "gui/GuiManager.h"
#include "gui/GuiConstants.h"

// --------------------
// Singleton accessor
// --------------------
GuiManager& GuiManager::getInstance() {
    static GuiManager instance; // created once
    return instance;
}

// --------------------
// Constructor/Destructor
// --------------------
GuiManager::GuiManager() = default;
GuiManager::~GuiManager() = default;


void GuiManager::update(float dt) {
    for (auto& item : items)
        if (item->visible) item->update(dt);
}

void GuiManager::draw() const {
    for (auto& item : items)
        if (item->visible) item->draw();

// ============================================================================
// GUI LAYOUT DEBUG VISUALIZER  (paste directly inside GuiManager::draw())
// ============================================================================

// ---------------------------------------------------------------------------
// 1. WAND SLOTS (Top-left)
// ---------------------------------------------------------------------------
    Rectangle wandSlots[4];
    for (int i = 0; i < 4; i++) {
        wandSlots[i] = {
                GUI_MARGIN + i * (WAND_SLOT_SIZE + WAND_SLOT_SPACING),
                GUI_MARGIN,
                WAND_SLOT_SIZE,
                WAND_SLOT_SIZE
        };
        DrawRectangleLinesEx(wandSlots[i], 2, WHITE);
    }

// ---------------------------------------------------------------------------
// 2. WAND PREVIEW BOX (Middle-left)
// ---------------------------------------------------------------------------
    Rectangle wandPreview = {
            GUI_MARGIN,
            GUI_MARGIN + WAND_SLOT_SIZE + 40,
            600,
            220
    };
    DrawRectangleLinesEx(wandPreview, 2, WHITE);

// ---------------------------------------------------------------------------
// 3. SPELL SLOTS (Inside wand preview)
// ---------------------------------------------------------------------------
    Rectangle spellSlots[7];
    for (int i = 0; i < 7; i++) {
        spellSlots[i] = {
                wandPreview.x + 20 + i * (SPELL_SLOT_SIZE + SPELL_SLOT_SPACING),
                wandPreview.y + 60,
                SPELL_SLOT_SIZE,
                SPELL_SLOT_SIZE
        };
        DrawRectangleLinesEx(spellSlots[i], 2, WHITE);
    }

// ---------------------------------------------------------------------------
// 4. DEBUG PANEL (Bottom-left)
// ---------------------------------------------------------------------------

    Rectangle debugPanel = {
            GUI_MARGIN,
            SCREEN_H - GUI_MARGIN - DEBUG_PANEL_HEIGHT,
            DEBUG_PANEL_WIDTH,
            DEBUG_PANEL_HEIGHT
    };
    DrawRectangleLinesEx(debugPanel, 2, GRAY);

// ---------------------------------------------------------------------------
// OPTIONAL LABELS FOR VISUAL DEBUGGING
// ---------------------------------------------------------------------------
    DrawText("Wands", wandSlots[0].x, wandSlots[0].y + WAND_SLOT_SIZE + 5, 20, WHITE);
    DrawText("Zoomed Wand View (when inventory open)", wandPreview.x + 10, wandPreview.y + 10, 20, WHITE);

// ============================================================================
// END VISUALIZER
// ============================================================================

}
