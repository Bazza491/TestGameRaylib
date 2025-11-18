//
// Created by Bailey on 3/10/2025.
//

#pragma once

#include <vector>
#include <memory>
#include "../world/EnvItem.h"


struct CastState {
    int castDelay;
    int rechargeMod;
    int damage;
    int spread;
    int knockback;
    std::vector<std::unique_ptr<EnvSpell>> spells;

    void modCastDelay(int mod);
    void modRechargeMod(int mod);
    void modDamage(int mod);
    void modSpread(int mod);
    void modKnockback(int mod);
    void addSpell(std::unique_ptr<EnvSpell> spell);

    void modAll(CastState state);

    CastState() : castDelay(0), rechargeMod(0), damage(0), spread(0), knockback(0) {}
};
