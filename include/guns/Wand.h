#pragma once

#include "SpellStorage.h"

class SpellTransform;


class Wand {
private:
    CastContext baseCtx;
    ProjectileStats baseProj;
    int spellsPCast = 1; // how many spells are cast per cast action

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

    float getCastDelay() const;
    void setCastDelay(float delay);
    float getRechargeTime() const;
    int getCapacity() const;
    void setCapacity(int size);
    int getSpellsPCast() const { return spellsPCast; }
};