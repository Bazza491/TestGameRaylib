#include <iostream>
#include "raylib.h"
#include "guns/SpellStorage.h"
#include "guns/Wand.h"
#include "guns/SpellTransform.h"
#include "guns/CastState.h"
#include "world/World.h"

Wand::Wand(WandStats stats, Texture2D texture) : stats(stats), texture(texture), spells(stats.capacity) {}
//Wand::Wand() {}


void Wand::cast(SpellTransform transform) {
    CastState cState;

    cState.shuffle = stats.shuffle;
    cState.draw = stats.spellsPCast;
    cState.castDelay = stats.castDelay;
    cState.rechargeMod = 0.0f;
    cState.damage = 0;
    cState.spread = stats.spread;
    cState.knockback = 0.0f;

    auto results = spells.cast(transform, cState);

    results.spawnAll(transform, cState);
}


float Wand::getCastDelay() const {
    return stats.castDelay;
}
void Wand::setCastDelay(float delay) {
    stats.castDelay = delay;
}
float Wand::getRechargeTime() const {
    return stats.rechargeTime;
}
int Wand::getCapacity() const {
    return stats.capacity;
}
void Wand::setCapacity(int size) {
    stats.capacity = size;
}