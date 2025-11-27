#pragma once

#include "SpellStorage.h"
#include <string>

class SpellTransform;


class Wand {
private:
    CastContext baseCtx;
    ProjectileStats baseProj;

    float maxMana = 80.0f;
    float currentMana = 40.0f;
    float manaChargeSpeed = 10.0f;
    std::string name = "Wand";

    Texture2D texture;
    SpellStorage spells;

    // For recharge & partial casts (future)
    int   nextIndex     = 0;    // where next cast starts in spell list
    float rechargeTimer = 0.0f; // time left until cast is allowed
public:
    Wand(const CastContext& baseCtx,
         const ProjectileStats& baseProj,
         Texture2D texture);

    void cast(const SpellTransform& origin);
//    void cast(SpellTransform transform, int slot); // Wand tracks it's own current slot, this should not exist
    void update(float delta);

    SpellStorage&       getSpellStorage()       { return spells; }
    const SpellStorage& getSpellStorage() const { return spells; }

    Texture2D getTexture() const { return texture; }
    const std::string& getName() const { return name; }
    void setName(const std::string& newName) { name = newName; }

    const CastContext& getBaseContext() const { return baseCtx; }
    const ProjectileStats& getBaseProjectile() const { return baseProj; }
    float getCastDelay() const;
    void setCastDelay(float delay);
    float getRechargeTime() const;
    int getCapacity() const;
    void setCapacity(int size);
    int getSpellsPCast() const { return baseCtx.remainingDraw; }
    bool isShuffle() const { return baseCtx.shuffle; }
    float getMana() const { return currentMana; }
    float getMaxMana() const { return maxMana; }
    float getManaChargeSpeed() const { return manaChargeSpeed; }
};