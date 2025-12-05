#pragma once

#include "guns/RaylikeTypes.h"

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
    std::string description;
    spellType type;
    CastContext     castMods;   // modifiers this spell applies
    ProjectileStats projMods;   // modifiers this spell applies
    std::unique_ptr<CastState> castState;
public:
    Spell() : name("Empty Slot"), description("Does nothing") {}
    virtual ~Spell() = default;

    [[nodiscard]] virtual std::unique_ptr<Spell> clone() const = 0;

    [[nodiscard]] const std::string& getName() const { return name; }
    [[nodiscard]] const std::string& getDescription() const { return description; }
    [[nodiscard]] spellType getType() const { return type; }


    // NOTE:
    // - deck: cloned list of spells used for this cast
    // - index: current index in that list (passed by reference so spells can skip)
    // - remainingCapacity: how many more "slots" we can consume in this cast
    // - remainingDraw: how many spells we still want to draw this shot
    // - cState: where env spells get added
    virtual void cast(const std::vector<std::unique_ptr<Spell>>& deck,
                      int& index,
                      const SpellTransform& origin,
                      CastState& cState) = 0;

protected:
    void copyBaseTo(Spell& other) const;
};

class SparkBolt : public Spell {
public:
    SparkBolt();
    [[nodiscard]] std::unique_ptr<Spell> clone() const override;
    void cast(const std::vector<std::unique_ptr<Spell>>& deck,
              int& index,
              const SpellTransform& transform,
              CastState& cState) override;
};

class SparkBoltTrigger : public Spell {
public:
    SparkBoltTrigger();
    [[nodiscard]] std::unique_ptr<Spell> clone() const override;
    void cast(const std::vector<std::unique_ptr<Spell>>& deck,
              int& index,
              const SpellTransform& transform,
              CastState& cState) override;
};

class DrawTwo : public Spell {
public:
    DrawTwo();
    [[nodiscard]] std::unique_ptr<Spell> clone() const override;
    void cast(const std::vector<std::unique_ptr<Spell>>& deck,
              int& index,
              const SpellTransform& transform,
              CastState& cState) override;
};