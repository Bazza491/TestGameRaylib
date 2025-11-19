
#include <algorithm>
#include "world/World.h"
#include "world/EnvItem.h"

#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <memory>

using json = nlohmann::json;



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
// Save/json management
// --------------------
static Color jsonToColor(const json& j) {
    return {
            static_cast<unsigned char>(j.value("r", 255)),
            static_cast<unsigned char>(j.value("g", 255)),
            static_cast<unsigned char>(j.value("b", 255)),
            static_cast<unsigned char>(j.value("a", 255))
    };
}

static Rectangle jsonToRect(const json& j) {
    Rectangle r{};
    r.x      = j.value("x", 0.0f);
    r.y      = j.value("y", 0.0f);
    r.width  = j.value("w", 0.0f);
    r.height = j.value("h", 0.0f);
    return r;
}

static Vector2 jsonToVec2(const json& j) {
    Vector2 v{};
    v.x = j.value("x", 0.0f);
    v.y = j.value("y", 0.0f);
    return v;
}

// Factory: create correct EnvItem subclass from JSON produced by EnvItem::toJson()
static std::unique_ptr<EnvItem> makeEnvItemFromJson(const json& j) {
    const std::string type = j.value("type", "EnvItem");

    Rectangle rect{};
    if (j.contains("rect")) rect = jsonToRect(j["rect"]);

    bool blocking   = j.value("blocking", true);
    bool hasPhysics = j.value("hasPhysics", false);

    Color color = WHITE;
    if (j.contains("color")) color = jsonToColor(j["color"]);

    Vector2 vel{};
    if (j.contains("velocity")) vel = jsonToVec2(j["velocity"]);

    std::unique_ptr<EnvItem> item;

    if (type == "EnvItem") {
        item = std::make_unique<EnvItem>(rect, color);
    } else if (type == "EnvSpell") {
        auto ptr = std::make_unique<EnvSpell>();
        ptr->setRect(rect);
        ptr->setColor(color);
        ptr->setBlocking(blocking);
        ptr->setHasPhysics(hasPhysics);
        ptr->setVelocity(vel);
        item = std::move(ptr);
    } else if (type == "EnvSparkBolt") {
        auto ptr = std::make_unique<EnvSparkBolt>();
        ptr->setRect(rect);
        ptr->setColor(color);
        ptr->setBlocking(blocking);
        ptr->setHasPhysics(hasPhysics);
        ptr->setVelocity(vel);
        item = std::move(ptr);
    } else if (type == "EnvWand") {
        // For now we don't reconstruct the actual Wand data.
        // You can extend this once Wand has its own fromJson().
        auto ptr = std::make_unique<EnvWand>();
        ptr->setColor(color);
        ptr->setBlocking(blocking);
        ptr->setHasPhysics(hasPhysics);
        ptr->setVelocity(vel);
        item = std::move(ptr);
    } else if (type == "EnvSparkBoltTrigger") {
        auto ptr = std::make_unique<EnvSparkBoltTrigger>();
        ptr->setRect(rect);
        ptr->setColor(color);
        ptr->setBlocking(blocking);
        ptr->setHasPhysics(hasPhysics);
        ptr->setVelocity(vel);
        item = std::move(ptr);
    } else {
        std::cerr << "Unknown EnvItem type in JSON: " << type << "\n";
        return nullptr;
    }

    // Note: we ignore "dead" and "hasTexture" on load for now.
    return item;
}

void World::loadWorld(const std::string& path) {
    clear();

    std::ifstream in(path);
    if (!in.is_open()) {
        std::cerr << "Failed to open world file: " << path << std::endl;
        return;
    }

    json j;
    try {
        in >> j;
    } catch (const std::exception& e) {
        std::cerr << "Error parsing JSON world file '" << path
                  << "': " << e.what() << std::endl;
        return;
    }

    // Support both:
    // 1) [ {item}, {item}, ... ]
    // 2) { "items": [ {item}, ... ] }
    json itemsJson;
    if (j.is_array()) {
        itemsJson = j;
    } else if (j.contains("items") && j["items"].is_array()) {
        itemsJson = j["items"];
    } else {
        std::cerr << "World file has no items array.\n";
        return;
    }

    for (const auto& itemJson : itemsJson) {
        auto item = makeEnvItemFromJson(itemJson);
        if (item) {
            items.push_back(std::move(item));
        }
    }

    std::cout << "Loaded world from " << path
              << " with " << items.size() << " items\n";
}

void World::saveWorld(const std::string& path) const {
    json worldJson;
    worldJson["items"] = json::array();
    for (auto& ptr : getItems()) {
        worldJson["items"].push_back(ptr->toJson());
    }

    std::ofstream out(path);
    out << worldJson.dump(4);
}

// --------------------
// Item management
// --------------------
void World::addItem(std::unique_ptr<EnvItem> item) {
    items.push_back(std::move(item));
    std::cout << "Added item. Total items: " << items.size() << "\n";
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
