#pragma once

#include <memory>
#include "raylib.h"
#include "guns/CastTypes.h"

#include <nlohmann/json.hpp>

struct CastState;
class Wand;

class EnvItem {
protected:
    Rectangle rect;
    bool blocking;
    bool hasPhysics;
    bool dead = false;
    Color color;
    Texture2D texture; // TODO: use a texture manager and make this a texture ID or Texture2D&
    Vector2 velocity;
public:
    EnvItem() {}
    EnvItem(Rectangle rect, bool blocking, bool hasPhysics, Texture2D texture);
    EnvItem(Rectangle rect, bool blocking, bool hasPhysics, Texture2D texture, Vector2 velocity);
    EnvItem(Rectangle rect, bool blocking, bool hasPhysics, Color color);
    EnvItem(Rectangle rect, Color color);
    virtual ~EnvItem() = default;

    virtual void update();
    bool isColliding(Rectangle rec);

    virtual nlohmann::json toJson() const;
    void saveToFile(const std::string& path) const;

    // Getters and setters
    Rectangle getRect() const;
    void setRect(Rectangle rect);
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