#pragma once

#include <memory>
#include <variant>
#include <vector>
#include "raylib.h"
#include "guns/CastTypes.h"

#include "world/Physics.h"

#include <nlohmann/json.hpp>

struct CastState;
class Wand;

class EnvItem {
protected:
    Rectangle rect;
    Collider collider;
    bool blocking;
    bool hasPhysics;
    bool dead = false;
    Color color;
    Texture2D texture; // TODO: use a texture manager and make this a texture ID or Texture2D& (texture2Ds are references internally anyway so maybe not a big deal for now)
    Vector2 velocity;
    Vector2 acceleration;
    float mass = 1.0f;
    float restitution = 0.0f;
    float drag = 0.0f;
    // TODO: figure out whether to add lifetime, etc. here or in subclasses
public:
    EnvItem() {}
    EnvItem(Rectangle rect, bool blocking, bool hasPhysics, Texture2D texture);
    EnvItem(Rectangle rect, bool blocking, bool hasPhysics, Texture2D texture, Vector2 velocity);
    EnvItem(Rectangle rect, bool blocking, bool hasPhysics, Color color);
    EnvItem(Rectangle rect, Color color);
    virtual ~EnvItem() = default;

    virtual void update();

    bool isColliding(const Collider &other) const;

    virtual nlohmann::json toJson() const;
    void saveToFile(const std::string& path) const;

    // Getters and setters
    Rectangle getRect() const;
    void setRect(Rectangle rect);
    const Collider &getCollider() const;
    void setCollider(const Collider &collider);
    void setColliderFromRect(Rectangle rect);
    bool isBlocking() const;
    void setBlocking(bool blocking);
    bool isPhysics() const;
    void setHasPhysics(bool hasPhysics);
    Color getColor() const;
    void setColor(Color color);
    Texture2D getTexture() const;
    void setTexture(Texture2D texture);
    const Vector2 &getVelocity() const;
    void setVelocity(const Vector2 &velocity);
    const Vector2 &getAcceleration() const;
    void setAcceleration(const Vector2 &acceleration);
    float getMass() const;
    void setMass(float mass);
    float getRestitution() const;
    void setRestitution(float restitution);
    float getDrag() const;
    void setDrag(float drag);
    bool isDead() const;
};

class EnvSpell : public EnvItem {
protected:
    std::unique_ptr<CastState> castState;
    std::function<void()> onExpire;
    ProjectileStats baseStats;
public:
    EnvSpell();
    ~EnvSpell();
    void setCastState(CastState&& state);
    void update() override;
    int getCastStateSize() const;
    ProjectileStats getBaseStats() const;

    nlohmann::json toJson() const override;
};

class EnvSparkBolt : public EnvSpell {
public:
    EnvSparkBolt();
    void update() override;

    nlohmann::json toJson() const override;
};

class EnvWand : public EnvItem {
public:
//    std::unique_ptr<Wand> wandData;

    EnvWand();

    nlohmann::json toJson() const override;
};

class EnvSparkBoltTrigger : public EnvSpell {
public:
    EnvSparkBoltTrigger();
    void update() override;

    nlohmann::json toJson() const override;
};