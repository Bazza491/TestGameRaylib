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

bool SpellStorage::insertSpell(std::unique_ptr<Spell> spell) {
    int slot = findFirstEmptySlot();
    if (slot == -1) return false;
    spells[slot] = std::move(spell);
    return true;
}

Spell* SpellStorage::getSpell(int slot) {
    if (slot < 0 || slot >= (int)spells.size()) return nullptr;
    return spells[slot].get();
}

const Spell* SpellStorage::getSpell(int slot) const {
    if (slot < 0 || slot >= (int)spells.size()) return nullptr;
    return spells[slot].get();
}

int SpellStorage::findFirstEmptySlot() const {
    for (int i = 0; i < (int)spells.size(); ++i) {
        if (!spells[i]) return i;
    }
    return -1;
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

    std::vector<std::unique_ptr<Spell>> castingDeck;
    castingDeck.reserve(spells.size());
    for (const auto& spell : spells) {
        castingDeck.push_back(spell ? spell->clone() : nullptr);
    }

    std::cout << capacity << std::endl;
    std::cout << state.ctx.remainingDraw << std::endl;
    std::cout << state.ctx.remainingCapacity << std::endl; // debug, ignore

    int index = 0;

    while (index >= 0 &&
           state.ctx.remainingDraw > 0 &&
           state.ctx.remainingCapacity > 0 &&
           index < (int)castingDeck.size()) {

        Spell* spell = castingDeck[index].get();
        if (!spell) {
            std::cout << "null spell cast" << std::endl;
            ++index;
            continue;
        }

        // Let the spell modify index/capacity/draw through references
        spell->cast(castingDeck, index, transform, state);

        // move to next spell unless spell changed index itself (trigger etc.)
        ++index;
    }
}