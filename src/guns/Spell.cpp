#include "raylib.h"

#include <string>
#include "world/EnvItem.h"
#include "guns/Spell.h"
#include "guns/CastState.h"
#include "guns/SpellTransform.h"
#include "guns/CastTypes.h"

#include <random>
#include <iostream>
#include <memory>
#include <vector>

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
void SparkBolt::cast(const std::vector<std::unique_ptr<Spell>>& spells,
                     int& index,
                     const SpellTransform& transform,
                     CastState& cState) {
    std::cout << name << " cast\n";

    // Apply this spell’s modifiers to the running state
    cState.applyModifiers(castMods, projMods);

    // create env spell
    auto env = std::make_unique<EnvSparkBolt>();


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

void SparkBoltTrigger::cast(const std::vector<std::unique_ptr<Spell>>& spells,
                            int& index,
                            const SpellTransform& transform,
                            CastState& cState) {
    std::cout << name << " cast (trigger)\n";

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
            innerIndex < (int)spells.size()) {
        Spell* s = spells[innerIndex].get();
        if (!s) { ++innerIndex; continue; }

        s->cast(spells, innerIndex, transform, innerState);
        ++innerIndex;
    }

    int consumed = innerIndex - (index + 1);

    // Apply this spell’s modifiers to the running state
    cState.applyModifiers(castMods, projMods);

    // 2. Make an EnvSparkBoltTrigger that stores innerState
    auto triggerEnv = std::make_unique<EnvSparkBoltTrigger>();
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

void DrawTwo::cast(const std::vector<std::unique_ptr<Spell>>& /*spells*/,
                   int& index,
                   const SpellTransform& transform,
                   CastState& cState) {
    std::cout << name << " cast (draw two)\n";

    cState.applyModifiers(castMods, projMods);

    // Consume this spell's slot
    cState.ctx.remainingDraw    -= 1;
    cState.ctx.remainingCapacity -= 1;
    cState.ctx.currentIndex      = index;
}