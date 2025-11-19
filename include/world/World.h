#pragma once

#include <vector>
#include <memory>
#include <string>

#include "raylib.h"
#include "EnvItem.h"

class World {
private:
    World();                                // private constructor
    ~World();                               // private destructor

    std::vector<std::unique_ptr<EnvItem>> items;

public:
    World(const World&) = delete;           // no copy
    World& operator=(const World&) = delete; // no assignment
    static World& getInstance();            // singleton accessor

    // Manage Levels
    void loadWorld(const std::string& path);
    void saveWorld(const std::string& path) const;

    // Manage items
    void addItem(std::unique_ptr<EnvItem> item);
    [[nodiscard]] const std::vector<std::unique_ptr<EnvItem>>& getItems() const;
    void clear();

    // Game loop functions
    void update();
    void draw() const;
};