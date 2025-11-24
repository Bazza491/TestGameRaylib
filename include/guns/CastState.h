#pragma once

#include <vector>
#include <memory>
#include "world/EnvItem.h"
//#include "SpellTransform.h"
#include "guns/CastTypes.h"

class EnvSpell;   // forward-declare
struct SpellTransform;

struct CastState {
    CastContext ctx;
    ProjectileStats proj;

    std::vector<std::unique_ptr<EnvSpell>> envSpells;

    CastState() = default;

    // no copy (because unique_ptr)
    CastState(const CastState&) = delete;
    CastState& operator=(const CastState&) = delete;

    // yes move
    CastState(CastState&&) noexcept = default;
    CastState& operator=(CastState&&) noexcept = default;

    void spawnAll(const SpellTransform& origin, CastState& state);

    void addSpell(std::unique_ptr<EnvSpell> s);

    // Spells use this to apply their "modifiers"
    void applyModifiers(const CastContext& deltaCtx,
                        const ProjectileStats& deltaProj);


};
