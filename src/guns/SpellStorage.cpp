#include <utility>
#include <memory>
#include "guns/SpellStorage.h"
#include "guns/Spell.h"
#include "guns/CastState.h"
#include <iostream>

#include "guns/SpellTransform.h"


SpellStorage::SpellStorage(int capacity)
        : capacity(capacity),
          spells(capacity)   // vector of size `capacity`, all nullptr
{
}

void SpellStorage::insertSpell(std::unique_ptr<Spell> spell, int slot) {
    if (slot < 0) return;
    if (slot >= (int)spells.size()) {
        spells.resize(slot + 1);
    }
    spells[slot] = std::move(spell);
}

CastState SpellStorage::cast(const SpellTransform& transform, CastState& state) {
    if (spells.empty()) return CastState{};


    CastState cState;

    int remainingCapacity = capacity;
    int remainingDraw     = state.draw;
    int index             = 0;

    while (remainingCapacity > 0 &&
           remainingDraw > 0 &&
           index < (int)spells.size()) {

        Spell* spell = spells[index].get();
        if (!spell) {
            ++index;
            continue;
        }

        // Let the spell modify index/capacity/draw through references
        spell->cast(spells, index, transform, remainingCapacity, remainingDraw, cState);

        // move to next spell unless spell changed index itself (trigger etc.)
        ++index;
    }

    return cState;  // moved out by value – OK, CastState is move-only
}