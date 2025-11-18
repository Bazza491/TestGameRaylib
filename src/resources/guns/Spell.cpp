//
// Created by BaileyPaior-Smith on 21/11/2022.
//

#include "raylib.h"

#include <string>
#include "../../../include/resources/world/EnvItem.h"
//#include "..\..\Common.h"
#include <iostream>
#include <memory>
#include <vector>
#include <cmath>

#include "../../../include/resources/guns/Spell.h"
#include "../../../include/resources/world/World.h"

Vector2 calcVelocity(double angle, double velocity) {
    Vector2 velocities;

    angle = angle * PI / 180.0;

    velocities.x = velocity * cos(angle);
    velocities.y = velocity * sin(angle);

    return velocities;
}


Spell::Spell() {
    name = "Empty Slot";
}
void Spell::cast(std::vector<std::unique_ptr<Spell>>::iterator me, SpellTransform transform, int capacity, int draw, CastState &cState) {
    std::cout << name << " cast (virtual) " << std::endl;
    if (draw > 0 && capacity > 1) {
        cState.addSpell(std::make_unique<EnvSpell>());

        me++;
        (*me)->cast(me, transform, capacity - 1, draw, cState);
    } else {
        std::cout << "End of chain" << std::endl;

        for (auto &spell: cState.spells) {
            spell->setRect({transform.position.x, transform.position.y, 8, 8});
            spell->setVelocity(calcVelocity(transform.rotation, 30));
            spell->setColor(RED);

            World::getInstance().addItem(std::move(spell));
        }
    }
}

SparkBolt::SparkBolt() {
    name = "Spark Bolt";
}
void SparkBolt::cast(std::vector<std::unique_ptr<Spell>>::iterator me, SpellTransform transform, int capacity, int draw, CastState &cState) {
    std::cout << name << " cast " << std::endl;
    if (draw > 0 && capacity > 1) {
        cState.addSpell(std::make_unique<EnvSparkBolt>());


        me++;
        (*me)->cast(me, transform, capacity - 1, draw - 1, cState);
    } else {
        std::cout << "End of chain" << std::endl;
        for (auto& spell : cState.spells) {
            spell->setRect({transform.position.x, transform.position.y, 8, 8});
            spell->setVelocity(calcVelocity(transform.rotation, 180.0));
            spell->setColor(YELLOW);

            World::getInstance().addItem(std::move(spell));
        }
    }

}