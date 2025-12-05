#pragma once

#include <memory>

#include "guns/CastTypes.h"
#include "guns/RaylikeTypes.h"

struct CastState;

namespace guns {

// Placeholder environment spell types that mirror the data the guns system
// needs to pass to a Godot-side instantiation layer.
class EnvSpell {
public:
    EnvSpell();
    virtual ~EnvSpell();

    Rectangle getRect() const;
    void setRect(Rectangle rect);

    const ProjectileStats& getBaseStats() const;
    void setBaseStats(const ProjectileStats& stats);

    Vector2 getVelocity() const;
    void setVelocity(Vector2 velocity);

    Color getColor() const;
    void setColor(Color color);

    const CastState* getCastState() const;
    void setCastState(CastState&& state);

protected:
    Rectangle rect{};
    Vector2 velocity{};
    Color color{};
    ProjectileStats baseStats{};
    std::unique_ptr<CastState> castState;
};

class EnvSparkBolt : public EnvSpell {
public:
    EnvSparkBolt();
};

class EnvSparkBoltTrigger : public EnvSpell {
public:
    EnvSparkBoltTrigger();
};

class EnvWand : public EnvSpell {
public:
    EnvWand();
};

} // namespace guns
