#include <algorithm>
#include <cmath>
#include "world/EnvItem.h"
#include "Common.h"
#include "guns/Wand.h"
#include "world/World.h"
#include <fstream>
#include "guns/CastState.h"
#include "guns/SpellTransform.h"
#include <memory>
#include <iostream>

using nlohmann::json;




EnvItem::EnvItem(Rectangle rect, bool blocking, bool hasPhysics, Texture2D texture) : rect(rect), collider(Physics::makeAABB(rect)), blocking(blocking),
hasPhysics(hasPhysics), color(RED), texture(texture), velocity((Vector2) {0, 0}), acceleration({0, 0}) {}
EnvItem::EnvItem(Rectangle rect, bool blocking, bool hasPhysics, Texture2D texture, Vector2 velocity) : rect(rect), collider(Physics::makeAABB(rect)), blocking(blocking),
hasPhysics(hasPhysics), color(RED), texture(texture), velocity(velocity), acceleration({0, 0}) {}
EnvItem::EnvItem(Rectangle rect, bool blocking, bool hasPhysics, Color color) : rect(rect), collider(Physics::makeAABB(rect)), blocking(blocking),
hasPhysics(hasPhysics), color(color), velocity((Vector2) {0, 0}), acceleration({0, 0}){}
EnvItem::EnvItem(Rectangle rect, Color color) : rect(rect), collider(Physics::makeAABB(rect)), blocking(true), hasPhysics(false), color(color),
velocity((Vector2) {0, 0}), acceleration({0, 0}) {}
void EnvItem::update() {
    if (!hasPhysics) return;

    float dt = GetFrameTime();
    Vector2 gravity{0.0f, G};
    velocity.x += (acceleration.x + gravity.x) * dt;
    velocity.y += (acceleration.y + gravity.y) * dt;

    float dragFactor = std::max(0.0f, 1.0f - drag * dt);
    velocity.x *= dragFactor;
    velocity.y *= dragFactor;

    Vector2 delta{velocity.x * dt, velocity.y * dt};

    Collider movedCollider = collider;
    Physics::translate(movedCollider, delta);

    World &world = World::getInstance();
    const auto &statics = world.getStaticColliders();

    float remaining = 1.0f;
    Vector2 applied{0.0f, 0.0f};
    Collider current = collider;

    for (const auto &obstacle : statics) {
        auto sweep = Physics::sweepAABB(current, delta, obstacle);
        if (sweep.hit && sweep.timeOfImpact < remaining) {
            remaining = sweep.timeOfImpact;
            applied = {delta.x * remaining, delta.y * remaining};
            Physics::translate(current, applied);

            // Slide along surface
            float vn = velocity.x * sweep.normal.x + velocity.y * sweep.normal.y;
            Vector2 bounce{velocity.x - (1 + restitution) * vn * sweep.normal.x,
                           velocity.y - (1 + restitution) * vn * sweep.normal.y};
            velocity = bounce;
            delta = {velocity.x * dt * (1.0f - remaining), velocity.y * dt * (1.0f - remaining)};
        }
    }

    Physics::translate(current, delta);

    // Handle any residual penetration
    for (const auto &obstacle : statics) {
        if (Physics::overlaps(current, obstacle)) {
            Vector2 correction = Physics::resolvePenetration(current, obstacle);
            Physics::translate(current, correction);
            if (correction.y != 0) velocity.y = 0;
            if (correction.x != 0) velocity.x = 0;
        }
    }

    collider = current;
    rect = Physics::computeAABB(collider);
}

bool EnvItem::isColliding(const Collider &other) const {
    return Physics::overlaps(collider, other);
}

static json colorToJson(Color c) {
    return json{
            {"r", static_cast<int>(c.r)},
            {"g", static_cast<int>(c.g)},
            {"b", static_cast<int>(c.b)},
            {"a", static_cast<int>(c.a)}
    };
}

// ----- EnvItem base -----
json EnvItem::toJson() const {
    json j;
    j["type"] = "EnvItem";
    j["rect"] = {
            {"x", rect.x},
            {"y", rect.y},
            {"w", rect.width},
            {"h", rect.height}
    };
    j["blocking"]    = blocking;
    j["hasPhysics"]  = hasPhysics;
    j["dead"]        = dead;
    j["color"]       = colorToJson(color);

    // Textures can’t really be serialized directly; you’d typically store
    // a texture path or ID instead. For now, just note whether it has one.
    j["hasTexture"] = (texture.id != 0);

    j["velocity"] = {
            {"x", velocity.x},
            {"y", velocity.y}
    };

    j["acceleration"] = {
            {"x", acceleration.x},
            {"y", acceleration.y}
    };

    j["mass"] = mass;
    j["restitution"] = restitution;
    j["drag"] = drag;

    switch (collider.type) {
        case ColliderType::AABB:
            j["collider"] = {
                    {"type", "aabb"},
                    {"rect", {{"x", collider.rect.x}, {"y", collider.rect.y}, {"w", collider.rect.width}, {"h", collider.rect.height}}}
            };
            break;
        case ColliderType::Circle:
            j["collider"] = {
                    {"type", "circle"},
                    {"center", {{"x", collider.center.x}, {"y", collider.center.y}}},
                    {"radius", collider.radius}
            };
            break;
        case ColliderType::Polygon: {
            nlohmann::json pts = nlohmann::json::array();
            for (const auto &p : collider.points) {
                pts.push_back({{"x", p.x}, {"y", p.y}});
            }
            j["collider"] = {
                    {"type", "polygon"},
                    {"points", pts}
            };
            break;
        }
    }

    return j;
}

void EnvItem::saveToFile(const std::string& path) const {
    std::ofstream out(path);
    if (!out.is_open()) return;
    out << toJson().dump(4);
}

//region Getters and setters
Rectangle EnvItem::getRect() const {
    return rect;
}
void EnvItem::setRect(Rectangle rect) {
    EnvItem::rect = rect;
    setColliderFromRect(rect);
}
const Collider &EnvItem::getCollider() const {
    return collider;
}
void EnvItem::setCollider(const Collider &collider) {
    EnvItem::collider = collider;
    rect = Physics::computeAABB(collider);
}
void EnvItem::setColliderFromRect(Rectangle rect) {
    setCollider(Physics::makeAABB(rect));
}
bool EnvItem::isBlocking() const {
    return blocking;
}
void EnvItem::setBlocking(bool blocking) {
    EnvItem::blocking = blocking;
}
bool EnvItem::isPhysics() const {
    return hasPhysics;
}
void EnvItem::setHasPhysics(bool hasPhysics) {
    EnvItem::hasPhysics = hasPhysics;
}
Color EnvItem::getColor() const {
    return color;
}
void EnvItem::setColor(Color color) {
    EnvItem::color = color;
}
Texture2D EnvItem::getTexture() const {
    return texture;
}
void EnvItem::setTexture(Texture2D texture) {
    EnvItem::texture = texture;
}
bool EnvItem::isDead() const {
    return dead;
}

const Vector2 &EnvItem::getVelocity() const {
    return velocity;
}

void EnvItem::setVelocity(const Vector2 &velocity) {
    EnvItem::velocity = velocity;
}
const Vector2 &EnvItem::getAcceleration() const {
    return acceleration;
}

void EnvItem::setAcceleration(const Vector2 &acceleration) {
    EnvItem::acceleration = acceleration;
}

float EnvItem::getMass() const {
    return mass;
}

void EnvItem::setMass(float mass) {
    EnvItem::mass = mass;
}

float EnvItem::getRestitution() const {
    return restitution;
}

void EnvItem::setRestitution(float restitution) {
    EnvItem::restitution = restitution;
}

float EnvItem::getDrag() const {
    return drag;
}

void EnvItem::setDrag(float drag) {
    EnvItem::drag = drag;
}
//endregion

EnvSpell::EnvSpell() : EnvItem(), castState(std::make_unique<CastState>()) {
    blocking = false;
    hasPhysics = false;
    this->onExpire = [this]() {
        this->dead = true;
    };
    color = BLUE;
    baseStats = ProjectileStats{
            .damage = 0.0f,
            .speed = 30.0f,
            .spread = 30.0f,
            .knockback = 0.0f,
            .lifetime = 0.0f,
            .size = 0.0f,
            .tint = WHITE
    };
    lifetimeRemaining = baseStats.lifetime;
    moveSpeed = baseStats.speed;
}

EnvSpell::~EnvSpell() = default;

void EnvSpell::setCastState(CastState&& state) {
    castState = std::make_unique<CastState>(std::move(state));
}

void EnvSpell::setRotation(float rotationDegrees) {
    rotation = rotationDegrees;
}

void EnvSpell::setMovementMode(SpellMovementMode mode) {
    movementMode = mode;
}

void EnvSpell::setMoveSpeed(float speed) {
    moveSpeed = speed;
}

void EnvSpell::setGravityScale(float scale) {
    gravityScale = scale;
}

void EnvSpell::setLifetimeRemaining(float lifetimeSeconds) {
    lifetimeRemaining = lifetimeSeconds;
}

void EnvSpell::update() {
    float dt = GetFrameTime();

    lifetimeRemaining -= dt;
    if (lifetimeRemaining <= 0.0f) {
        onExpire();
        return;
    }

    World &world = World::getInstance();
    auto hitsBlocking = [&](const Collider &target) {
        for (const auto &obstacle : world.getStaticColliders()) {
            if (Physics::overlaps(target, obstacle)) return true;
        }
        for (const auto &item : world.getItems()) {
            if (item.get() == this) continue;
            if (!item->isBlocking()) continue;
            if (Physics::overlaps(target, item->getCollider())) return true;
        }
        return false;
    };

    if (movementMode == SpellMovementMode::Physics) {
        hasPhysics = true;
        EnvItem::update();
        if (hitsBlocking(collider)) {
            onExpire();
            return;
        }
        rotation = atan2f(velocity.y, velocity.x) * RAD2DEG;
    } else {
        Vector2 forward{cosf(rotation * DEG2RAD), sinf(rotation * DEG2RAD)};
        if (moveSpeed == 0.0f) {
            float velMag = sqrtf(velocity.x * velocity.x + velocity.y * velocity.y);
            moveSpeed = velMag > 0.0f ? velMag : baseStats.speed;
        }
        moveSpeed += G * gravityScale * dt;
        Vector2 delta{forward.x * moveSpeed * dt, forward.y * moveSpeed * dt};

        Collider moved = collider;
        Physics::translate(moved, delta);

        if (hitsBlocking(moved)) {
            onExpire();
            return;
        }

        collider = moved;
        rect = Physics::computeAABB(collider);
    }
}

json EnvSpell::toJson() const {
    json j = EnvItem::toJson();
    j["type"] = "EnvSpell";
    j["rotation"] = rotation;
    j["lifetimeRemaining"] = lifetimeRemaining;
    j["movementMode"] = movementMode == SpellMovementMode::Physics ? "physics" : "arrow";
    j["moveSpeed"] = moveSpeed;
    j["gravityScale"] = gravityScale;
    return j;
}

int EnvSpell::getCastStateSize() const {
    return castState->envSpells.size();
}

ProjectileStats EnvSpell::getBaseStats() const {
    return baseStats;
}

float EnvSpell::getLifetimeRemaining() const {
    return lifetimeRemaining;
}

float EnvSpell::getGravityScale() const {
    return gravityScale;
}

float EnvSpell::getMoveSpeed() const {
    return moveSpeed;
}

SpellMovementMode EnvSpell::getMovementMode() const {
    return movementMode;
}

EnvSparkBolt::EnvSparkBolt(){
    this->onExpire = [this]() {
        this->dead = true;
    };
    baseStats = ProjectileStats{
            .damage = 3.0f,
            .speed = 30.0f,
            .lifetime = 5.0f,
            .size = 8.0f,
            .tint = GREEN
    };
    lifetimeRemaining = baseStats.lifetime;
    moveSpeed = baseStats.speed;
}

void EnvSparkBolt::update() {
    EnvSpell::update();
}

json EnvSparkBolt::toJson() const {
    json j = EnvSpell::toJson();
    j["type"] = "EnvSparkBolt";
    return j;
}

EnvWand::EnvWand() {}

json EnvWand::toJson() const {
    json j = EnvItem::toJson();
    j["type"] = "EnvWand";

    // If Wand has its own toJson(), you can nest it:
    // if (wandData) j["wand"] = wandData->toJson();
    // else          j["wand"] = nullptr;

    return j;
}

EnvSparkBoltTrigger::EnvSparkBoltTrigger() {
    this->onExpire = [this]() {
        if (this->castState) {

            float rot = this->rotation;
            if (this->velocity.x != 0.0f || this->velocity.y != 0.0f) {
                rot = atan2f(this->velocity.y, this->velocity.x) * RAD2DEG;
            }
            // TODO: Payload should cast in opposite direction if spell has hit wall

            // Pass the transform and the CastState metadata
            this->castState->spawnAll(
                    SpellTransform({ rect.x, rect.y }, rot),
                    *this->castState
            );
        }
        this->dead = true;
    };
    baseStats = ProjectileStats{
            .damage = 3.0f,
            .speed = 30.0f,
            .lifetime = 5.0f,
            .size = 8.0f,
            .tint = RED
    };
    lifetimeRemaining = baseStats.lifetime;
    moveSpeed = baseStats.speed;
}

void EnvSparkBoltTrigger::update() {
    EnvSpell::update();
}

json EnvSparkBoltTrigger::toJson() const {
    json j = EnvSpell::toJson();
    j["type"] = "EnvSparkBoltTrigger";
    return j;
}