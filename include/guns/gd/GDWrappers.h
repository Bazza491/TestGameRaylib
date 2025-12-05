#pragma once

#include <memory>

#include "guns/CastState.h"
#include "guns/CastTypes.h"
#include "guns/RaylikeTypes.h"
#include "guns/Spell.h"
#include "guns/SpellStorage.h"
#include "guns/SpellTransform.h"
#include "guns/TinkeringMenu.h"
#include "guns/Wand.h"

// Thin wrappers around the guns system to make it easier to expose the API to
// Godot (via GDExtension or other bindings) without pulling raylib headers
// into the integration layer.

namespace gd {

struct CastContextWrapper {
    CastContextWrapper();
    explicit CastContextWrapper(const CastContext& ctx);

    CastContext& unwrap();
    const CastContext& unwrap() const;

private:
    CastContext context;
};

struct ProjectileStatsWrapper {
    ProjectileStatsWrapper();
    explicit ProjectileStatsWrapper(const ProjectileStats& stats);

    ProjectileStats& unwrap();
    const ProjectileStats& unwrap() const;

private:
    ProjectileStats projectile;
};

struct SpellTransformWrapper {
    SpellTransformWrapper();
    explicit SpellTransformWrapper(const SpellTransform& transform);
    SpellTransformWrapper(Vector2 position, float rotation);

    SpellTransform& unwrap();
    const SpellTransform& unwrap() const;

private:
    SpellTransform data;
};

class CastStateWrapper {
public:
    CastStateWrapper();

    CastState& unwrap();
    const CastState& unwrap() const;

private:
    CastState state;
};

class SpellStorageWrapper {
public:
    explicit SpellStorageWrapper(int capacity = 16, bool castable = true);

    SpellStorage& unwrap();
    const SpellStorage& unwrap() const;

    void insertSpell(std::unique_ptr<Spell> spell, int slot);
    bool insertSpell(std::unique_ptr<Spell> spell);
    void cast(const SpellTransform& transform, CastState& state);

private:
    SpellStorage storage;
};

class WandWrapper {
public:
    WandWrapper(const CastContext& baseCtx,
                const ProjectileStats& baseProj,
                Texture2D texture);

    Wand& unwrap();
    const Wand& unwrap() const;

    void cast(const SpellTransform& origin);
    void update(float delta);
    SpellStorage& getSpellStorage();
    const SpellStorage& getSpellStorage() const;

private:
    Wand wand;
};

class TinkeringMenuWrapper {
public:
    TinkeringMenuWrapper();

    SpellStorage& getCylinder();
    const SpellStorage& getCylinder() const;

    SpellStorage& getInventory();
    const SpellStorage& getInventory() const;

private:
    TinkeringMenu menu;
};

} // namespace gd

