#pragma once

#include "raylib.h"

#include <memory>
#include <vector>
#include <string>
#include "CastState.h"
//#include "SpellTransform.h"

struct SpellTransform;
struct CastState;

enum spellType {
    PROJECTILE,
    STATIC_PROJECTILE,
    MODIFIER,
    DRAW_MANY,
    PASSIVE,
    MATERIAL,
    UTILITY,
    OTHER,
};

class Spell {
protected:
    std::string name;
    spellType type;
    CastContext     castMods;   // modifiers this spell applies
    ProjectileStats projMods;   // modifiers this spell applies
    std::unique_ptr<CastState> castState;
public:
    Spell() : name("Empty Slot") {}
    virtual ~Spell() = default;


    // NOTE:
    // - spells: full list of spells in the wand
    // - index: current index in that list (passed by reference so spells can skip)
    // - remainingCapacity: how many more "slots" we can consume in this cast
    // - remainingDraw: how many spells we still want to draw this shot
    // - cState: where env spells get added
    virtual void cast(const std::vector<std::unique_ptr<Spell>>& spells,
                      int& index,
                      const SpellTransform& origin,
                      CastState& cState) = 0;
};

class SparkBolt : public Spell {
public:
    SparkBolt();
    void cast(const std::vector<std::unique_ptr<Spell>>& spells,
              int& index,
              const SpellTransform& transform,
              CastState& cState) override;
};

class SparkBoltTrigger : public Spell {
public:
    SparkBoltTrigger();
    void cast(const std::vector<std::unique_ptr<Spell>>& spells,
              int& index,
              const SpellTransform& transform,
              CastState& cState) override;
};

class DrawTwo : public Spell {
public:
    DrawTwo();
    void cast(const std::vector<std::unique_ptr<Spell>>& spells,
              int& index,
              const SpellTransform& transform,
              CastState& cState) override;
};