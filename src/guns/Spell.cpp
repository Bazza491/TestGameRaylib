#include "raylib.h"

#include <string>
#include "world/EnvItem.h"
#include "guns/Spell.h"
#include "guns/CastState.h"
#include "guns/SpellTransform.h"

#include <random>
#include <iostream>
#include <memory>
#include <vector>

SparkBolt::SparkBolt() {
    name = "Spark Bolt";
}
void SparkBolt::cast(const std::vector<std::unique_ptr<Spell>>& /*spells*/,
                     int& /*index*/,
                     const SpellTransform& transform,
                     int& remainingCapacity,
                     int& remainingDraw,
                     CastState& cState) {
    std::cout << name << " cast\n";

    // create env spell
    auto env = std::make_unique<EnvSparkBolt>();
    // env->setCastState(...) if needed
    // env->setRect / velocity later, or inside EnvSparkBolt

    cState.addSpell(std::move(env));

    // consume this spell's slot
    --remainingCapacity;
    --remainingDraw;
}

SparkBoltTrigger::SparkBoltTrigger() {
    name = "Spark Bolt Trigger";
}

void SparkBoltTrigger::cast(const std::vector<std::unique_ptr<Spell>>& spells,
                            int& index,
                            const SpellTransform& transform,
                            int& remainingCapacity,
                            int& remainingDraw,
                            CastState& cState) {
    std::cout << name << " cast (trigger)\n";

    // 1. Build a sub-cast from the *next* spells in the chain:
    CastState innerState;

//    innerState.spread = 30;

    int innerIndex     = index + 1;
    int innerCapacity  = remainingCapacity - 1;
    int innerDraw      = 1;   // only draw 1 from sub-chain

    while (innerCapacity > 0 &&
           innerDraw > 0 &&
           innerIndex < (int)spells.size()) {
        Spell* s = spells[innerIndex].get();
        if (!s) { ++innerIndex; continue; }

        s->cast(spells, innerIndex, transform, innerCapacity, innerDraw, innerState);
        ++innerIndex;
    }

    int consumed = innerIndex - (index + 1);

    // 2. Make an EnvSparkBoltTrigger that stores innerState
    auto triggerEnv = std::make_unique<EnvSparkBoltTrigger>();
    triggerEnv->setCastState(std::move(innerState));

    cState.addSpell(std::move(triggerEnv));

    // 3. Skip over the consumed spells in the main chain
    index += consumed;

    // 4. Consume this trigger's own slot
    --remainingCapacity;
    --remainingDraw;
}

DrawTwo::DrawTwo() {
    name = "Draw Two";
}

void DrawTwo::cast(const std::vector<std::unique_ptr<Spell>>& /*spells*/,
                   int& /*index*/,
                   const SpellTransform& /*transform*/,
                   int& remainingCapacity,
                   int& remainingDraw,
                   CastState& /*cState*/) {
    std::cout << name << " cast (draw two)\n";

    // Increase the draw count by 2
    remainingDraw += 2;

    // Consume this spell's slot
    --remainingCapacity;
    --remainingDraw;
}