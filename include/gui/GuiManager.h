#pragma once

#include <vector>
#include <memory>

#include "gui/GuiItem.h"

class GuiManager {
private:
    GuiManager();                                // private constructor
    ~GuiManager();                               // private destructor

    std::vector<std::unique_ptr<GuiItem>> items;
public:
    template<typename T, typename... Args>
    T* add(Args&&... args) {
        auto ptr = std::make_unique<T>(std::forward<Args>(args)...);
        T* raw = ptr.get();
        items.emplace_back(std::move(ptr));
        return raw;
    }

    GuiManager(const GuiManager&) = delete;           // no copy
    GuiManager& operator=(const GuiManager&) = delete; // no assignment
    static GuiManager& getInstance();            // singleton accessor

    void update(float dt);
    void draw() const;
};