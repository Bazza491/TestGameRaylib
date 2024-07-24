//
// Created by BaileyPaior-Smith on 21/11/2022.
//

#include "raylib.h"

#include <string>
#include "C:\Users\BaileyPaior-Smith\CLionProjects\SecondGame\EnvItem.h"
#include "C:\Users\BaileyPaior-Smith\CLionProjects\SecondGame\Common.h"
#include <iostream>
#include <memory>
#include <vector>
#include <cmath>

#include "Spell.h"

Vector2 calcVelocity(double angle, double velocity) {
    Vector2 velocities;

    angle = angle * PI / 180.0;

    velocities.x = velocity * cos(angle);
    velocities.y = velocity * sin(angle);

    return velocities;
}


Spell::Spell() {
    pos = { 1, 2 };
    name = "Empty Slot";
}
void Spell::cast(std::vector<std::shared_ptr<Spell>>::iterator me, Vector2 from, float angle, int capacity, int draw) {
    std::cout << name << " cast (virtual) " << std::endl;
    if (draw > 0 && capacity > 1) {
        me++;
        (*me)->cast(me, pos, 0.0f, capacity - 1, draw);
    }
}

SparkBolt::SparkBolt() {
    pos = { 0, 0 };
    name = "Spark Bolt";
}
void SparkBolt::cast(std::vector<std::shared_ptr<Spell>>::iterator me, Vector2 from, float angle, int capacity, int draw) {
    std::cout << name << " cast " << std::endl;
    if (draw > 0 && capacity > 1) {

        EnvItem newSpell({pos.x, pos.y, 10, 5}, false, true, {},
                         calcVelocity(angle, 10));
        //#TODO - Function here to add "newSpell" to world (the vector of EnvItems drawn every frame)

        me++;
        (*me)->cast(me, pos, 0.0f, capacity - 1, draw - 1);
    }
}