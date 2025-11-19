#pragma once

#include "guns/SpellStorage.h"
#include "guns/Wand.h"

class Player {
private:
    Vector2 pos;
    Rectangle hitBox;
    Vector2 velocity;
    std::array<std::unique_ptr<Wand>, 4> wands;
    int selectedWandSlot;
    SpellStorage spells;
    SpriteSheet sprite;
    PlayerState state;
    float timeElapsed;
    int frameSpeed;
    float wandRotation;

public:
    Player(Texture2D spriteSheet);
    Player(Texture2D spriteSheet, Vector2 startPos);
    void setState(PlayerState newState);
    void update (float delta, Vector2 mouseWorldPos);
    void draw (float scaleX, float scaleY, float delta);

    Vector2 getPos() const;
    void setPos(Vector2 newPos);
    float getVelY() const;
    void setVelY(float velY);
    float getVelX() const;
    void setVelX(float velX);
    Vector2 getVelocity() const;
    void setVelocity(Vector2 newVelocity);
    Rectangle getHitBox() const;
    void cast();
    float getWandRotation() const;
    Vector2 getWandTip() const;

};

