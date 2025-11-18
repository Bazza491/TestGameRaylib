//
// Created by BaileyPaior-Smith on 21/11/2022.
//
#include <utility>
#include <memory>
#include "../../../include/resources/guns/SpellStorage.h"

SpellStorage::SpellStorage() : SpellStorage(16) {}

SpellStorage::SpellStorage(int capacity) {
    SpellStorage::capacity = capacity;
    for (int i = 0; i < capacity; i++) {
        spells.emplace_back(new Spell());
    }
}
void SpellStorage::insertSpell(std::unique_ptr<Spell> spell, int slot) {
    if (slot >= spells.size()) spells.resize(slot + 1);
    spells[slot] = std::move(spell);

//    spells.at(slot - 1) = std::move(spell);
}
void SpellStorage::cast(SpellTransform transform) {
    CastState state;
    spells.at(0)->cast(spells.begin(), transform, capacity, 5, state);
}