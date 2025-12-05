#include "guns/CastState.h"

#include "guns/EnvSpells.h"
#include "guns/SpellTransform.h"

#include <cmath>
#include <random>
#include <iostream>

CastState::~CastState() = default;

void CastState::addSpell(std::unique_ptr<guns::EnvSpell> spell) {
    if (spell) {
        envSpells.push_back(std::move(spell));
    } else {
        std::cout << "Warning: Tried to add null spell to CastState\n";
    }
}

Vector2 calcVelocity(double angle, double velocity) {
    Vector2 velocities;

    angle = angle * PI / 180.0;

    velocities.x = velocity * cos(angle);
    velocities.y = velocity * sin(angle);

    return velocities;
}

Color ColorTintAlphaWeighted(Color base, Color tint)
{
    float severity = tint.a / 255.0f;   // 0.0 → no tint, 1.0 → full tint

    // First compute the normal multiplicative tint
    float r = (base.r * tint.r) / 255.0f;
    float g = (base.g * tint.g) / 255.0f;
    float b = (base.b * tint.b) / 255.0f;

    // Then lerp between base and multiplicative tint by severity
    Color out;
    out.r = (unsigned char)(base.r + (r - base.r) * severity);
    out.g = (unsigned char)(base.g + (g - base.g) * severity);
    out.b = (unsigned char)(base.b + (b - base.b) * severity);

    // Keep original alpha, or change it?
    // I assume keep the base alpha:
    out.a = base.a;

    return out;
}

void CastState::spawnAll(const SpellTransform& origin, CastState& state) {
    if (envSpells.empty()) return;

//    float offset = 0.0f;
//    float rotation = origin.rotation;

    // create a random float generator
    std::random_device rd;  // seed
    std::mt19937 gen(rd()); // Mersenne Twister engine
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

    for (auto &spell: envSpells) { //TODO: ask about expensiveness and if code should check if spread == 0
        if (!spell) continue;

        // Use the same ProjectileStats for all projectiles this call

        // set size and position
        float size = proj.size;
        Rectangle r = spell->getRect();
        r.width  = spell->getBaseStats().size * size;
        r.height = spell->getBaseStats().size * size;
        r.x = origin.position.x - r.width * 0.5f;
        r.y = origin.position.y - r.height * 0.5f;
        spell->setRect(r);


        // spread and velocity
        float offsetDeg  = dist(gen) * proj.spread;
        float finalDeg   = origin.rotation + offsetDeg;

        Vector2 vel = calcVelocity(finalDeg, proj.speed * spell->getBaseStats().speed);
        spell->setVelocity(vel);

        // set color (if no sprite) or tint
        Color tinted = ColorTintAlphaWeighted(spell->getBaseStats().tint, proj.tint);
        spell->setColor(tinted);

        if (spawnCallback) {
            spawnCallback(std::move(spell));
        } else {
            spawnedSpells.push_back(std::move(spell));
        }
    }
//    envSpells.clear();
}

std::vector<std::unique_ptr<guns::EnvSpell>> CastState::takeSpawnedSpells() {
    auto out = std::move(spawnedSpells);
    spawnedSpells.clear();
    return out;
}

void CastState::applyModifiers(const CastContext& deltaCtx,
                               const ProjectileStats& deltaProj) {
    // Chain-level
    ctx.remainingCapacity += deltaCtx.remainingCapacity;
    ctx.remainingDraw     += deltaCtx.remainingDraw;

    ctx.shuffle           = ctx.shuffle || deltaCtx.shuffle;

    ctx.castDelay        += deltaCtx.castDelay;
    ctx.rechargeTime     += deltaCtx.rechargeTime;
    ctx.rechargeBonus    += deltaCtx.rechargeBonus;

    // We generally let SpellStorage manage currentIndex/reachedEnd,
    // so we *don’t* modify those here.

    // Projectile-level
    proj.damage    += deltaProj.damage;
    if (deltaProj.speed != 0.0f) proj.speed     *= deltaProj.speed;
    proj.spread    += deltaProj.spread;
    proj.knockback += deltaProj.knockback;
    proj.lifetime  += deltaProj.lifetime;
    if (deltaProj.size != 0.0f) proj.size     *= deltaProj.size;

    // Apply tint modification
    if (!(deltaProj.tint.r == 255 &&
            deltaProj.tint.g == 255 &&
            deltaProj.tint.b == 255))   // Skip pure white (neutral/no tint)
    {
        float severity = deltaProj.tint.a / 255.0f;   // How strong the tint pushes

        // Step 1: normal multiplicative tint of the *current* tint
        float mulR = (proj.tint.r * deltaProj.tint.r) / 255.0f;
        float mulG = (proj.tint.g * deltaProj.tint.g) / 255.0f;
        float mulB = (proj.tint.b * deltaProj.tint.b) / 255.0f;

        // Step 2: blend between current tint and multiplicative result
        proj.tint.r = (unsigned char)(proj.tint.r + (mulR - proj.tint.r) * severity);
        proj.tint.g = (unsigned char)(proj.tint.g + (mulG - proj.tint.g) * severity);
        proj.tint.b = (unsigned char)(proj.tint.b + (mulB - proj.tint.b) * severity);

        // Increase tint severity based on how strong the modifier's alpha is
        int newAlpha = proj.tint.a + deltaProj.tint.a;

// Clamp to valid 0–255 range
        if (newAlpha > 255) newAlpha = 255;

        proj.tint.a = (unsigned char)newAlpha;
    }
}