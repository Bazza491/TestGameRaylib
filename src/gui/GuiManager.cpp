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


void GuiManager::update(float dt, Vector2 virtualMousePos) {
    for (auto& item : items)
        if (item->visible) item->update(dt, virtualMousePos);
}

void GuiManager::draw() const {
    for (auto& item : items)
        if (item->visible) item->draw();

// ============================================================================
// GUI LAYOUT DEBUG VISUALIZER (FOR PLANNING PURPOSES ONLY, DO NOT REMOVE GUI ELEMENTS UNTIL THEY HAVE BEEN CREATED)
// ============================================================================

// ---------------------------------------------------------------------------
// WAND PREVIEW BOX (Middle-left)
// ---------------------------------------------------------------------------
    Rectangle wandPreview = {
            GUI_MARGIN,
            GUI_MARGIN + WAND_SLOT_SIZE + 40,
            600,
            220
    };
    DrawRectangleLinesEx(wandPreview, 2, WHITE);

// ---------------------------------------------------------------------------
// SPELL SLOTS (Inside wand preview)
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
// DEBUG PANEL (Bottom-left)
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

    DrawText("Zoomed Wand View (when inventory open)", wandPreview.x + 10, wandPreview.y + 10, 20, WHITE);

// ============================================================================
// END VISUALIZER
// ============================================================================

}
