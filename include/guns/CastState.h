#pragma once

#include <vector>
#include <memory>
#include "world/EnvItem.h"
//#include "SpellTransform.h"

class EnvSpell;   // forward-declare
struct SpellTransform;

struct CastState {
    bool shuffle;
    int draw;
    float castDelay;
    float rechargeMod;
    int damage;
    float spread;
    float knockback;

    std::vector<std::unique_ptr<EnvSpell>> envSpells;

    CastState() = default;

    // no copy (because unique_ptr)
    CastState(const CastState&) = delete;
    CastState& operator=(const CastState&) = delete;

    // yes move
    CastState(CastState&&) noexcept = default;
    CastState& operator=(CastState&&) noexcept = default;

    // Later: a method to push this into the world
    void spawnAll(const SpellTransform& transform, CastState& state);

    void modCastDelay(int mod);
    void modRechargeMod(int mod);
    void modDamage(int mod);
    void modSpread(int mod);
    void modKnockback(int mod);
    void addSpell(std::unique_ptr<EnvSpell> s);

    void modAll(CastState state);


};
