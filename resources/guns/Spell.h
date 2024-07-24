//
// Created by BaileyPaior-Smith on 2/11/2022.
//

#pragma once

#include "raylib.h"

#include <memory>
#include <vector>
#include <string>

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
    Vector2 pos;
    std::string name;
public:
    Spell();
    virtual void cast(std::vector<std::shared_ptr<Spell>>::iterator me, Vector2 from, float angle, int capacity, int draw);
//    ~Spell();
};

class SparkBolt : public Spell {
public:
    SparkBolt();
    void cast(std::vector<std::shared_ptr<Spell>>::iterator me, Vector2 from, float angle, int capacity, int draw);
};