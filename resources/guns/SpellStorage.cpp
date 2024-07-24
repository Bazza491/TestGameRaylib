//
// Created by BaileyPaior-Smith on 21/11/2022.
//
#include <utility>
#include <vector>
#include "Spell.h"
#include <memory>
#include <vector>
#include "SpellStorage.h"

SpellStorage::SpellStorage() : SpellStorage(16) {}

SpellStorage::SpellStorage(int capacity) {
    SpellStorage::capacity = capacity;
    for (int i = 0; i < capacity; i++) {
        spells.emplace_back(new Spell());
    }
}
void SpellStorage::insertSpell(std::shared_ptr<Spell> spell, int slot) {
    spells.at(slot - 1) = std::move(spell);
}
void SpellStorage::cast() {
    spells.at(0)->cast(spells.begin(), { 3.0f, 4.0f }, 0.0f, capacity, 5);
}