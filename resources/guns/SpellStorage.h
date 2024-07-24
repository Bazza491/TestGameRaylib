//
// Created by BaileyPaior-Smith on 3/11/2022.
//

#pragma once

#include "Spell.h"
#include <vector>
#include <memory>
#include <vector>

class SpellStorage {
private:
    int capacity;
    std::vector<std::shared_ptr<Spell>> spells;
    bool castable;
public:
    SpellStorage();
    explicit SpellStorage(int capacity);
    void insertSpell(std::shared_ptr<Spell> spell, int slot);
    void cast();
};