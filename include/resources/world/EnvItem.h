
#pragma once

#include <memory>
#include "raylib.h"
//#include "../guns/CastState.h"

struct CastState;

class EnvItem {
//    virtual ~EnvItem() = default;
//    virtual std::unique_ptr<EnvItem> clone() const = 0;
protected:
    Rectangle rect;
    bool blocking;
    bool hasPhysics;
    bool dead = false;
    Color color;
    Texture2D texture;
    Vector2 velocity;
public:
    EnvItem() {}
    EnvItem(Rectangle rect, bool blocking, bool hasPhysics, Texture2D texture);
    EnvItem(Rectangle rect, bool blocking, bool hasPhysics, Texture2D texture, Vector2 velocity);
    EnvItem(Rectangle rect, bool blocking, bool hasPhysics, Color color);
    EnvItem(Rectangle rect, Color color);
    virtual void update();
    bool isColliding(Rectangle rec);

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
//    CastState castState*;
public:
    EnvSpell();
    void setCastState(CastState state);
    void update() override;
};

class EnvSparkBolt : public EnvSpell {
public:
    EnvSparkBolt();
    void update() override;
};