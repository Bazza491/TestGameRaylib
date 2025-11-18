//
// Created by Bailey Paior-Smith on 2/11/2022.
//

#pragma once


//#include "resources/world/World.h"
#include "resources/guns/SpellStorage.h"
#include "resources/guns/Wand.h"

class Player {
private:
    Vector2 pos;
    Rectangle hitBox;
    Vector2 velocity;
//    std::array<Wand, 4> wands;
    SpellStorage spells;
    int selectedWandSlot;
    PlayerState state;
    SpriteSheet sprite;
    float timeElapsed;
    int frameSpeed;


public:
    Player(Texture2D spriteSheet);
    Player(Texture2D spriteSheet, Vector2 startPos);
    void setState(PlayerState newState);
    void update (float delta);
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

};

