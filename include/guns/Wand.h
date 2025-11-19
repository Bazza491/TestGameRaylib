#pragma once

#include "SpellStorage.h"
struct WandStats {
    bool shuffle;
    int spellsPCast;
    float castDelay;
    float rechargeTime;
    int manaMax;
    int chargeSpd;
    int capacity;
    float spread;

    WandStats() : shuffle(false), spellsPCast(1), castDelay(0.5f), rechargeTime(2.0f),
                 manaMax(100), chargeSpd(10), capacity(5), spread(0.0f) {}
    WandStats(bool shuffle, int spellsPCast, float castDelay, float rechargeTime,
              int manaMax, int chargeSpd, int capacity, float spread) :
              shuffle(shuffle), spellsPCast(spellsPCast), castDelay(castDelay),
              rechargeTime(rechargeTime), manaMax(manaMax), chargeSpd(chargeSpd),
              capacity(capacity), spread(spread) {}
};

class SpellTransform;


class Wand {
private:
    WandStats stats;
    Texture2D texture;
    SpellStorage spells;
public:
    Wand(WandStats stats, Texture2D texture);
    void cast(SpellTransform transform);
//    void cast(SpellTransform transform, int slot);
    float getCastDelay() const;
    void setCastDelay(float delay);
    float getRechargeTime() const;
    int getCapacity() const;
    void setCapacity(int size);
    Texture2D getTexture() const { return texture; }
    int getSpellsPCast() const { return stats.spellsPCast; }
    SpellStorage& getSpellStorage() { return spells; }
};