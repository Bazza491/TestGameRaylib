//
// Created by Bailey on 3/10/2025.
//

#include "../../../include/resources/guns/CastState.h"

void CastState::addSpell(std::unique_ptr<EnvSpell> spell) {
    spells.push_back(std::move(spell));
}


void CastState::modCastDelay(int mod) {
    castDelay += mod;
}

void CastState::modRechargeMod(int mod) {
    rechargeMod += mod;
}

void CastState::modDamage(int mod) {
    damage += mod;
}

void CastState::modSpread(int mod) {
    spread += mod;
}

void CastState::modKnockback(int mod) {
    knockback += mod;
}

void CastState::modAll(CastState state) {
    modCastDelay(state.castDelay);
    modRechargeMod(state.rechargeMod);
    modDamage(state.damage);
    modSpread(state.spread);
    modKnockback(state.knockback);
}