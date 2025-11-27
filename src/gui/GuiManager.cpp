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
    for (auto& item : items) {
        if (item->visible) item->draw();
    }
}
