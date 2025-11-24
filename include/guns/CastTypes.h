#pragma once

#include "raylib.h"

// Everything needed while walking the spell chain
// (used/modified inside Spell::cast)
struct CastContext {
    // How much of the wand we can still use this cast
    int remainingCapacity = 0;   // slots left this cast
    int remainingDraw     = 0;   // spells left to fire this cast

    bool shuffle          = false;

    // Timing
    float castDelay       = 0.0f;  // per-shot delay (once per "fire" during chain)
    float rechargeTime    = 0.0f;  // base recharge once we reach the end
    float rechargeBonus   = 0.0f;  // modifiers added by spells

    // For partial-chain behavior (future)
    int  currentIndex     = 0;     // index in spell storage
    bool reachedEnd       = true;  // did this cast hit end of storage?
};

// Everything needed to spawn projectiles in CastState::spawnAll
// All projectiles from a single spawnAll share these stats.
struct ProjectileStats {
    float damage      = 0.0f;
    float speed       = 1.0f;
    float spread      = 0.0f;   // +/- degrees from base rotation
    float knockback   = 0.0f;
    float lifetime    = 0.0f;
    float size        = 1.0f;   // width/height for rect
    Color tint        = WHITE;  // tint or color modifier
};