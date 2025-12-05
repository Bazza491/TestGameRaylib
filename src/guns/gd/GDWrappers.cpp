#include "guns/gd/GDWrappers.h"

#include "guns/CastState.h"
#include "guns/CastTypes.h"
#include "guns/SpellStorage.h"
#include "guns/Spell.h"
#include "guns/SpellTransform.h"
#include "guns/TinkeringMenu.h"
#include "guns/Wand.h"

namespace gd {

CastContextWrapper::CastContextWrapper() = default;

CastContextWrapper::CastContextWrapper(const CastContext& ctx)
        : context(ctx) {}

CastContext& CastContextWrapper::unwrap() { return context; }

const CastContext& CastContextWrapper::unwrap() const { return context; }

ProjectileStatsWrapper::ProjectileStatsWrapper() = default;

ProjectileStatsWrapper::ProjectileStatsWrapper(const ProjectileStats& stats)
        : projectile(stats) {}

ProjectileStats& ProjectileStatsWrapper::unwrap() { return projectile; }

const ProjectileStats& ProjectileStatsWrapper::unwrap() const { return projectile; }

SpellTransformWrapper::SpellTransformWrapper()
        : data({Vector2{0.0f, 0.0f}, 0.0f}) {}

SpellTransformWrapper::SpellTransformWrapper(const SpellTransform& transform)
        : data(transform) {}

SpellTransformWrapper::SpellTransformWrapper(Vector2 position, float rotation)
        : data({position, rotation}) {}

SpellTransform& SpellTransformWrapper::unwrap() { return data; }

const SpellTransform& SpellTransformWrapper::unwrap() const { return data; }

CastStateWrapper::CastStateWrapper() = default;

CastState& CastStateWrapper::unwrap() { return state; }

const CastState& CastStateWrapper::unwrap() const { return state; }

SpellStorageWrapper::SpellStorageWrapper(int capacity, bool castable)
        : storage(capacity, castable) {}

SpellStorage& SpellStorageWrapper::unwrap() { return storage; }

const SpellStorage& SpellStorageWrapper::unwrap() const { return storage; }

void SpellStorageWrapper::insertSpell(std::unique_ptr<Spell> spell, int slot) {
    storage.insertSpell(std::move(spell), slot);
}

bool SpellStorageWrapper::insertSpell(std::unique_ptr<Spell> spell) {
    return storage.insertSpell(std::move(spell));
}

void SpellStorageWrapper::cast(const SpellTransform& transform, CastState& state) {
    storage.cast(transform, state);
}

WandWrapper::WandWrapper(const CastContext& baseCtx,
                         const ProjectileStats& baseProj,
                         Texture2D texture)
        : wand(baseCtx, baseProj, texture) {}

Wand& WandWrapper::unwrap() { return wand; }

const Wand& WandWrapper::unwrap() const { return wand; }

void WandWrapper::cast(const SpellTransform& origin) { wand.cast(origin); }

void WandWrapper::update(float delta) { wand.update(delta); }

SpellStorage& WandWrapper::getSpellStorage() { return wand.getSpellStorage(); }

const SpellStorage& WandWrapper::getSpellStorage() const { return wand.getSpellStorage(); }

TinkeringMenuWrapper::TinkeringMenuWrapper() = default;

SpellStorage& TinkeringMenuWrapper::getCylinder() { return menu.getCylinder(); }

const SpellStorage& TinkeringMenuWrapper::getCylinder() const { return menu.getCylinder(); }

SpellStorage& TinkeringMenuWrapper::getInventory() { return menu.getInventory(); }

const SpellStorage& TinkeringMenuWrapper::getInventory() const { return menu.getInventory(); }

} // namespace gd

