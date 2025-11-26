#pragma once

#include <array>
#include <memory>

#include "guns/SpellStorage.h"
#include "guns/Wand.h"
#include "Common.h"

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
    int maxHealth = 100;
    int health = 90;
    float maxStamina = 100.0f;
    float stamina = 75.0f;

public:
    Player(Texture2D spriteSheet);
    Player(Texture2D spriteSheet, Vector2 startPos);
    void setState(PlayerState newState);
    void update (float delta, Vector2 mouseWorldPos);
    void draw (float scaleX, float scaleY, float delta);

    [[nodiscard]] Vector2 getPos() const { return pos;}
    void setPos(Vector2 newPos);
    [[nodiscard]] float getVelY() const { return velocity.y;}
    void setVelY(float velY);
    [[nodiscard]] float getVelX() const { return velocity.x;}
    void setVelX(float velX);
    [[nodiscard]] Vector2 getVelocity() const { return velocity;}
    void setVelocity(Vector2 newVelocity);
    [[nodiscard]] Rectangle getHitBox() const { return hitBox;}
    void cast();
    [[nodiscard]] float getWandRotation() const;
    [[nodiscard]] Vector2 getWandTip() const;

    [[nodiscard]] int getMaxHealth() const { return maxHealth; }
    [[nodiscard]] int getHealth() const { return health;}
    [[nodiscard]] float getMaxStamina() const { return maxStamina; }
    [[nodiscard]] float getStamina() const { return stamina; }
    [[nodiscard]] float getMaxMana() const;
    [[nodiscard]] float getMana() const;
    [[nodiscard]] int getSelectedWandSlot() const { return selectedWandSlot; }
    [[nodiscard]] const std::array<std::unique_ptr<Wand>, 4>& getWandSlots() const { return wands; }

    bool selectWandSlot(int index);
    bool cycleWandSlot(int delta);
    bool swapWands(int first, int second);

};

