#include <utility>
#include <memory>
#include "guns/SpellStorage.h"
#include "guns/Spell.h"
#include "guns/CastState.h"
#include <iostream>

#include "guns/SpellTransform.h"


SpellStorage::SpellStorage(int capacity, bool castable)
        : capacity(capacity),
          spells(capacity),  // vector of size `capacity`, all nullptr
          castable(castable)
{
}

void SpellStorage::insertSpell(std::unique_ptr<Spell> spell, int slot) {
    if (slot < 0) return;
    if (slot >= (int)spells.size()) return;
    spells[slot] = std::move(spell);
}

Spell* SpellStorage::getSpell(int slot) {
    if (slot < 0 || slot >= (int)spells.size()) return nullptr;
    return spells[slot].get();
}

const Spell* SpellStorage::getSpell(int slot) const {
    if (slot < 0 || slot >= (int)spells.size()) return nullptr;
    return spells[slot].get();
}

bool SpellStorage::swapSpells(int first, int second) {
    return swapSpells(*this, first, second);
}

bool SpellStorage::swapSpells(SpellStorage& other, int first, int second) {
    if (first < 0 || second < 0) return false;
    if (first >= (int)spells.size() || second >= (int)other.spells.size()) return false;
    if (this == &other && first == second) return false;
    std::swap(spells[first], other.spells[second]);
    return true;
}

void SpellStorage::cast(const SpellTransform& transform, CastState& state) {
    if (!castable || spells.empty()) return;

    int remainingCapacity = capacity;
    int remainingDraw     = state.ctx.remainingDraw;
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
        spell->cast(spells, index, transform, state);

        // move to next spell unless spell changed index itself (trigger etc.)
        ++index;
    }
}