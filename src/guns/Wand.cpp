#include <iostream>
#include "raylib.h"
#include "guns/SpellStorage.h"
#include "guns/Wand.h"
#include "guns/SpellTransform.h"
#include "guns/CastState.h"
#include "guns/CastTypes.h"


Wand::Wand(const CastContext& ctx,
           const ProjectileStats& proj,
           Texture2D tex)
        : baseCtx(ctx),
          baseProj(proj),
          texture(tex) {}
//Wand::Wand() {}

void Wand::update(float delta) {
    if (rechargeTimer > 0.0f) {
        rechargeTimer -= delta;
        if (rechargeTimer < 0.0f) rechargeTimer = 0.0f;
    }
}

void Wand::cast(const SpellTransform& origin) {
    // Still recharging? Do nothing
//    if (rechargeTimer > 0.0f) {
//        return;
//    }

    CastState state;

    // Start each cast from the wand’s base values
    state.ctx  = baseCtx;
    state.proj = baseProj;

    // Set up partial-chain fields for future behavior
    state.ctx.currentIndex = nextIndex;
    state.ctx.reachedEnd   = true;  // SpellStorage::cast can flip this

    // Cast spellsPCast spells from storage
    spells.cast(origin, state);

    // Future-ready partial-chain handling:
    // If we didn't reach end of storage, we want to resume from currentIndex next time.
    if (state.ctx.reachedEnd) {
        nextIndex = 0;
    } else {
        nextIndex = state.ctx.currentIndex;
    }

    // Recharge: base + whatever modifiers accumulated in state.ctx
    rechargeTimer = state.ctx.rechargeTime + state.ctx.rechargeBonus;



    // Finally spawn all envSpells using the final ProjectileStats
    state.spawnAll(origin, state);
}


float Wand::getCastDelay() const {
    return baseCtx.castDelay;
}
void Wand::setCastDelay(float delay) {
    baseCtx.castDelay = delay;
}
float Wand::getRechargeTime() const {
    return baseCtx.rechargeTime;
}
int Wand::getCapacity() const {
    return baseCtx.remainingCapacity;
}
void Wand::setCapacity(int size) {
    baseCtx.remainingCapacity = size;
}