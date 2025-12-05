#pragma once

#include <functional>
#include <memory>
#include <vector>

#include "guns/CastTypes.h"
#include "guns/EnvSpells.h"

struct SpellTransform;

struct CastState {
    CastContext ctx;
    ProjectileStats proj;

    std::vector<std::unique_ptr<guns::EnvSpell>> envSpells;

    // Spells resolved during spawnAll are handed to this callback so a Godot
    // integration layer can instantiate PackedScenes or emit signals. If no
    // callback is provided, the spells are stored in spawnedSpells for manual
    // retrieval.
    std::function<void(std::unique_ptr<guns::EnvSpell>)> spawnCallback;
    std::vector<std::unique_ptr<guns::EnvSpell>> spawnedSpells;

    CastState() = default;
    ~CastState();

    // no copy (because unique_ptr)
    CastState(const CastState&) = delete;
    CastState& operator=(const CastState&) = delete;

    // yes move
    CastState(CastState&&) noexcept = default;
    CastState& operator=(CastState&&) noexcept = default;

    void spawnAll(const SpellTransform& origin, CastState& state);

    // Move any processed spawns out when no callback is set.
    std::vector<std::unique_ptr<guns::EnvSpell>> takeSpawnedSpells();

    void addSpell(std::unique_ptr<guns::EnvSpell> s);

    // Spells use this to apply their "modifiers"
    void applyModifiers(const CastContext& deltaCtx,
                        const ProjectileStats& deltaProj);


};
