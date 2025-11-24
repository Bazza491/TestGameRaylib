#pragma once

#include "Spell.h"
#include <vector>
#include <memory>

class Spell;
struct SpellTransform;
struct CastState;

class SpellStorage {
private:
    int capacity;
    std::vector<std::unique_ptr<Spell>> spells;
    bool castable;
public:
    explicit SpellStorage(int capacity = 16);
    void insertSpell(std::unique_ptr<Spell> spell, int slot);

    // casts the wand, populates CastState, and returns it
    void cast(const SpellTransform& transform, CastState &state);

    int getCapacity() const { return capacity; }

//    void cast(SpellTransform transform, CastState &cState, int draw);
//    void cast(SpellTransform transform, CastState &cState, int draw, int slot);
};