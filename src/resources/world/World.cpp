
#include <algorithm>
#include "../../../include/resources/world/World.h"

// --------------------
// Singleton accessor
// --------------------
World& World::getInstance() {
    static World instance; // created once
    return instance;
}

// --------------------
// Constructor/Destructor
// --------------------
World::World() = default;
World::~World() = default;

// --------------------
// Item management
// --------------------
void World::addItem(std::unique_ptr<EnvItem> item) {
    items.push_back(std::move(item));
}

const std::vector<std::unique_ptr<EnvItem>>& World::getItems() const {
    return items;
}

void World::clear() {
    items.clear();
}

// --------------------
// Update / Draw
// --------------------
void World::update() {
    for (auto& item : items) {
        item->update();
    }
    // Remove items that are marked for deletion
    items.erase(
            std::remove_if(items.begin(), items.end(),
                           [](const std::unique_ptr<EnvItem>& e) {
                               return e->isDead();
                           }),
            items.end()
    );
}

void World::draw() const {
    for (const auto& item : items) {
        Rectangle r = item->getRect();
        Color c = item->getColor();
        DrawRectangleRec(r, c);

        Texture2D tex = item->getTexture();
        if (tex.id != 0) {
            DrawTexture(tex, static_cast<int>(r.x), static_cast<int>(r.y), WHITE);
        }
    }
}
