//
// Created by BaileyPaior-Smith on 2/11/2022.
//

#pragma once

#include "raylib.h"

#include <memory>
#include <vector>
#include <string>
#include "CastState.h"
#include "SpellTransform.h"

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
public:
    virtual ~Spell() = default;
    Spell();
    virtual void cast(std::vector<std::unique_ptr<Spell>>::iterator me,
                      SpellTransform transform,
                      int capacity,
                      int draw,
                      CastState &cState);
};

class SparkBolt : public Spell {
public:
    SparkBolt();
    void cast(std::vector<std::unique_ptr<Spell>>::iterator me,
              SpellTransform transform,
              int capacity,
              int draw,
              CastState &cState) override;
};