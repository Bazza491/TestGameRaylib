#include "guns/EnvSpells.h"

#include "guns/CastState.h"

namespace guns {

EnvSpell::EnvSpell() = default;
EnvSpell::~EnvSpell() = default;

Rectangle EnvSpell::getRect() const { return rect; }

void EnvSpell::setRect(Rectangle rectIn) { rect = rectIn; }

const ProjectileStats& EnvSpell::getBaseStats() const { return baseStats; }

void EnvSpell::setBaseStats(const ProjectileStats& stats) { baseStats = stats; }

Vector2 EnvSpell::getVelocity() const { return velocity; }

void EnvSpell::setVelocity(Vector2 velocityIn) { velocity = velocityIn; }

Color EnvSpell::getColor() const { return color; }

void EnvSpell::setColor(Color colorIn) { color = colorIn; }

const CastState* EnvSpell::getCastState() const { return castState.get(); }

void EnvSpell::setCastState(CastState&& state) {
    castState = std::make_unique<CastState>(std::move(state));
}

EnvSparkBolt::EnvSparkBolt() {
    baseStats = ProjectileStats{
            .damage = 3.0f,
            .speed = 30.0f,
            .spread = 0.0f,
            .knockback = 0.0f,
            .lifetime = 5.0f,
            .size = 8.0f,
            .tint = GREEN,
    };
}

EnvSparkBoltTrigger::EnvSparkBoltTrigger() {
    baseStats = ProjectileStats{
            .damage = 3.0f,
            .speed = 30.0f,
            .spread = 0.0f,
            .knockback = 0.0f,
            .lifetime = 5.0f,
            .size = 8.0f,
            .tint = RED,
    };
}

EnvWand::EnvWand() = default;

} // namespace guns
