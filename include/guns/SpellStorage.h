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
    explicit SpellStorage(int capacity = 16, bool castable = true);
    void insertSpell(std::unique_ptr<Spell> spell, int slot);
    [[nodiscard]] const std::vector<std::unique_ptr<Spell>>& getSpells() const { return spells; }
    [[nodiscard]] std::vector<std::unique_ptr<Spell>>& getSpells() { return spells; }
    [[nodiscard]] Spell* getSpell(int slot);
    [[nodiscard]] const Spell* getSpell(int slot) const;
    [[nodiscard]] int findFirstEmptySlot() const;
    bool swapSpells(int first, int second);
    bool swapSpells(SpellStorage& other, int first, int second);

    // casts the wand, populates CastState, and returns it
    void cast(const SpellTransform& transform, CastState &state);

    int getCapacity() const { return capacity; }
    [[nodiscard]] bool isCastable() const { return castable; }

//    void cast(SpellTransform transform, CastState &cState, int draw);
//    void cast(SpellTransform transform, CastState &cState, int draw, int slot);
};