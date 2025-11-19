#include "guns/CastState.h"
#include "world/World.h"
#include "world/EnvItem.h"
#include "guns/SpellTransform.h"
#include "guns/Spell.h"

#include <random>

void CastState::addSpell(std::unique_ptr<EnvSpell> spell) {
    envSpells.push_back(std::move(spell));
}

Vector2 calcVelocity(double angle, double velocity) {
    Vector2 velocities;

    angle = angle * PI / 180.0;

    velocities.x = velocity * cos(angle);
    velocities.y = velocity * sin(angle);

    return velocities;
}

void CastState::spawnAll(const SpellTransform& transform, CastState& state) {
    float offset = 0.0f;
    float rotation = transform.rotation;

    // create a random float generator
    std::random_device rd;  // seed
    std::mt19937 gen(rd()); // Mersenne Twister engine
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

    for (auto &spell: envSpells) { //TODO: ask about expensiveness and if code should check if spread == 0
        spell->setRect({transform.position.x, transform.position.y, 8, 8});

        offset = dist(gen) * state.spread;
        rotation = transform.rotation + offset;

        spell->setVelocity(calcVelocity(rotation, 30));
        spell->setColor(spell->getColor());

        World::getInstance().addItem(std::move(spell));
    }
//    envSpells.clear();
}

//void CastState::cast(SpellTransform transform) {
//    float offset = 0.0f;
//    float rotation = transform.rotation;
//
//    // create a random float generator
//    std::random_device rd;  // seed
//    std::mt19937 gen(rd()); // Mersenne Twister engine
//    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
//
//    for (auto &spell: envSpells) { //TODO: ask about expensiveness and if code should check if spread == 0
//        spell->setRect({transform.position.x, transform.position.y, 8, 8});
//
//        offset = dist(gen) * spread;
//        rotation = transform.rotation + offset;
//
//        spell->setVelocity(calcVelocity(rotation, 30));
//        spell->setColor(spell->getColor());
//
//        World::getInstance().addItem(std::move(spell));
//    }
//}

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