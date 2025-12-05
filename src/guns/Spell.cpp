#include <string>

#include "guns/CastState.h"
#include "guns/CastTypes.h"
#include "guns/EnvSpells.h"
#include "guns/Spell.h"
#include "guns/SpellTransform.h"

#include <random>
#include <iostream>
#include <memory>
#include <vector>

void Spell::copyBaseTo(Spell& other) const {
    other.name     = name;
    other.type     = type;
    other.castMods = castMods;
    other.projMods = projMods;

    if (castState) {
        auto stateCopy = std::make_unique<CastState>();
        stateCopy->ctx  = castState->ctx;
        stateCopy->proj = castState->proj;
        other.castState = std::move(stateCopy);
    } else {
        other.castState.reset();
    }
}

SparkBolt::SparkBolt() {
    name = "Spark Bolt";
    type = PROJECTILE;
    castMods = {
            .remainingDraw = 0,
            .castDelay = 0.08f,
            .rechargeBonus = 0.0f
    };
    projMods = {
            .damage = 3.0f,
            .speed = 1.0f,
            .spread = -2.0f,
            .knockback = 3.0f,
            .lifetime = 5.0f,
            .size = 1.0f,
            .tint = WHITE
    };
}

std::unique_ptr<Spell> SparkBolt::clone() const {
    auto copy = std::make_unique<SparkBolt>();
    copyBaseTo(*copy);
    return copy;
}

void SparkBolt::cast(const std::vector<std::unique_ptr<Spell>>& deck,
                     int& index,
                     const SpellTransform& transform,
                     CastState& cState) {
    std::cout << name << " cast\n";

    // Apply this spell’s modifiers to the running state
    cState.applyModifiers(castMods, projMods);

    // create env spell
    auto env = std::make_unique<guns::EnvSparkBolt>();


    cState.addSpell(std::move(env));

    // consume this spell's slot
    cState.ctx.remainingDraw    -= 1;
    cState.ctx.remainingCapacity -= 1;
    cState.ctx.currentIndex      = index;
}

SparkBoltTrigger::SparkBoltTrigger() {
    name = "Spark Bolt Trigger";
    type = PROJECTILE;
    castMods = {
            .remainingDraw = 0,
            .castDelay = 0.08f,
            .rechargeBonus = 0.0f
    };
    projMods = {
            .damage = 3.0f,
            .speed = 1.0f,
            .spread = -2.0f,
            .knockback = 3.0f,
            .lifetime = 5.0f,
            .size = 1.0f,
            .tint = WHITE
    };
}

std::unique_ptr<Spell> SparkBoltTrigger::clone() const {
    auto copy = std::make_unique<SparkBoltTrigger>();
    copyBaseTo(*copy);
    return copy;
}

void SparkBoltTrigger::cast(const std::vector<std::unique_ptr<Spell>>& deck,
                            int& index,
                            const SpellTransform& transform,
                            CastState& cState) {
    std::cout << name << " cast\n";

    // 1. Build a sub-cast from the *next* spells in the chain:
    CastState innerState;
    innerState.proj.spread = 30.0f;

    int innerIndex = index + 1;
    innerState.ctx.currentIndex     = innerIndex;
    innerState.ctx.remainingCapacity  = cState.ctx.remainingCapacity - 1;
    innerState.ctx.remainingDraw      = 1;   // start with 1 draw from sub-chain

    while (innerIndex > 0 &&
            innerState.ctx.remainingDraw > 0 &&
            innerState.ctx.remainingCapacity > 0 &&
            innerIndex < (int)deck.size()) {
        Spell* s = deck[innerIndex].get();
        if (!s) { ++innerIndex; continue; }

        s->cast(deck, innerIndex, transform, innerState);
        ++innerIndex;
    }

    int consumed = innerIndex - (index + 1);

    // Apply this spell’s modifiers to the running state
    cState.applyModifiers(castMods, projMods);

    // 2. Make an EnvSparkBoltTrigger that stores innerState
    auto triggerEnv = std::make_unique<guns::EnvSparkBoltTrigger>();
    triggerEnv->setCastState(std::move(innerState));

    cState.addSpell(std::move(triggerEnv));

    // 3. Skip over the consumed spells in the main chain
    index += consumed;

    // 4. Consume this trigger's own slot
    cState.ctx.remainingDraw    -= 1;
    cState.ctx.remainingCapacity -= 1;
    cState.ctx.currentIndex      = index;
}

DrawTwo::DrawTwo() {
    name = "Draw Two";
    type = DRAW_MANY;
    castMods = {
            .remainingDraw = 2,
            .castDelay = 0.04f,
            .rechargeBonus = 0.0f
    };
    projMods = {
            .damage = 0.0f,
            .speed = 1.0f,
            .spread = 0.0f,
            .knockback = 0.0f,
            .lifetime = 0.0f,
            .size = 1.0f,
            .tint = { 130, 130, 130, 60 } // Tint cast Gray slightly
    };
}

std::unique_ptr<Spell> DrawTwo::clone() const {
    auto copy = std::make_unique<DrawTwo>();
    copyBaseTo(*copy);
    return copy;
}

void DrawTwo::cast(const std::vector<std::unique_ptr<Spell>>& /*deck*/,
                   int& index,
                   const SpellTransform& transform,
                   CastState& cState) {
    std::cout << name << " cast\n";

    cState.applyModifiers(castMods, projMods);

    // Consume this spell's slot
    cState.ctx.remainingDraw    -= 1;
    cState.ctx.remainingCapacity -= 1;
    cState.ctx.currentIndex      = index;
}