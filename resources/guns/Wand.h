//
// Created by BaileyPaior-Smith on 2/11/2022.
//

#pragma once
//#include "raylib.h"

//#include "SpellStorage.h"

class Wand {
private:
    bool shuffle;
    int spellsPCast;
    float castDelay;
    float rechargeTime;
    int manaMax;
    int chargeSpd;
    int capacity;
    float spread;
    Texture2D texture;
    SpellStorage spells;
public:
    Wand(float castDelay, float rechargeTime, int capacity, Texture2D texture);
    Wand();
    float getCastDelay() const;
    void setCastDelay(float delay);
    float getRechargeTime() const;
    int getCapacity() const;
    void setCapacity(int size);
};