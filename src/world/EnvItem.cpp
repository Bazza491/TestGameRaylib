#include <cmath>
#include "world/EnvItem.h"
#include "Common.h"
#include "guns/Wand.h"
#include <fstream>
#include "guns/CastState.h"
#include "guns/SpellTransform.h"
#include <memory>
#include <iostream>

using nlohmann::json;




EnvItem::EnvItem(Rectangle rect, bool blocking, bool hasPhysics, Texture2D texture) : rect(rect), blocking(blocking),
hasPhysics(hasPhysics), color(RED), texture(texture), velocity((Vector2) {0, 0}) {}
EnvItem::EnvItem(Rectangle rect, bool blocking, bool hasPhysics, Texture2D texture, Vector2 velocity) : rect(rect), blocking(blocking),
hasPhysics(hasPhysics), color(RED), texture(texture), velocity(velocity) {}
EnvItem::EnvItem(Rectangle rect, bool blocking, bool hasPhysics, Color color) : rect(rect), blocking(blocking),
hasPhysics(hasPhysics), color(color), velocity((Vector2) {0, 0}){}
EnvItem::EnvItem(Rectangle rect, Color color) : rect(rect), blocking(true), hasPhysics(false), color(color),
velocity((Vector2) {0, 0}) {}
void EnvItem::update() {
    if (hasPhysics) {
        rect.x += velocity.x;
        rect.y += velocity.y;
        velocity.x *= AIR_FRICTION_F;
        velocity.y *= AIR_FRICTION_F;
        float afc = AIR_FRICTION_C;
//        if ((fabsf(velocity.x) < afc) && fabsf(velocity.y) < afc) dead = true;
    }
}

bool EnvItem::isColliding(Rectangle rec) {
    Rectangle bigger = (Rectangle){rect.x-1.0f, rect.y-1.0f, rect.width+2.0f, rect.height+2.0f};
    return CheckCollisionRecs(rec, bigger);
}

static json colorToJson(Color c) {
    return json{
            {"r", static_cast<int>(c.r)},
            {"g", static_cast<int>(c.g)},
            {"b", static_cast<int>(c.b)},
            {"a", static_cast<int>(c.a)}
    };
}

// ----- EnvItem base -----
json EnvItem::toJson() const {
    json j;
    j["type"] = "EnvItem";
    j["rect"] = {
            {"x", rect.x},
            {"y", rect.y},
            {"w", rect.width},
            {"h", rect.height}
    };
    j["blocking"]    = blocking;
    j["hasPhysics"]  = hasPhysics;
    j["dead"]        = dead;
    j["color"]       = colorToJson(color);

    // Textures can’t really be serialized directly; you’d typically store
    // a texture path or ID instead. For now, just note whether it has one.
    j["hasTexture"] = (texture.id != 0);

    j["velocity"] = {
            {"x", velocity.x},
            {"y", velocity.y}
    };

    return j;
}

void EnvItem::saveToFile(const std::string& path) const {
    std::ofstream out(path);
    if (!out.is_open()) return;
    out << toJson().dump(4);
}

//region Getters and setters
Rectangle EnvItem::getRect() const {
    return rect;
}
void EnvItem::setRect(Rectangle rect) {
    EnvItem::rect = rect;
}
bool EnvItem::isBlocking() const {
    return blocking;
}
void EnvItem::setBlocking(bool blocking) {
    EnvItem::blocking = blocking;
}
bool EnvItem::isPhysics() const {
    return hasPhysics;
}
void EnvItem::setHasPhysics(bool hasPhysics) {
    EnvItem::hasPhysics = hasPhysics;
}
Color EnvItem::getColor() const {
    return color;
}
void EnvItem::setColor(Color color) {
    EnvItem::color = color;
}
Texture2D EnvItem::getTexture() const {
    return texture;
}
void EnvItem::setTexture(Texture2D texture) {
    EnvItem::texture = texture;
}
bool EnvItem::isDead() const {
    return dead;
}

const Vector2 &EnvItem::getVelocity() const {
    return velocity;
}

void EnvItem::setVelocity(const Vector2 &velocity) {
    EnvItem::velocity = velocity;
}
//endregion

EnvSpell::EnvSpell() : EnvItem(), castState(std::make_unique<CastState>()) {
    this->onExpire = [this]() {
        this->dead = true;
    };
    color = BLUE;
}

EnvSpell::~EnvSpell() = default;

void EnvSpell::setCastState(CastState&& state) {
    castState = std::make_unique<CastState>(std::move(state));
}

void EnvSpell::update() {
    rect.x += velocity.x;
    rect.y += velocity.y;
    velocity.x *= AIR_FRICTION_F;
    velocity.y *= AIR_FRICTION_F;
    float afc = AIR_FRICTION_C;
    if ((fabsf(velocity.x) < afc) && fabsf(velocity.y) < afc) onExpire();
}

json EnvSpell::toJson() const {
    json j = EnvItem::toJson();
    j["type"] = "EnvSpell";
    return j;
}

int EnvSpell::getCastStateSize() const {
    return castState->envSpells.size();
}


EnvSparkBolt::EnvSparkBolt(){
    this->onExpire = [this]() {
        this->dead = true;
    };
    color = GREEN;
}

void EnvSparkBolt::update() {
    rect.x += velocity.x;
    rect.y += velocity.y;
    velocity.x *= AIR_FRICTION_F;
    velocity.y *= AIR_FRICTION_F;
    float afc = AIR_FRICTION_C;
    if ((fabsf(velocity.x) < afc) && fabsf(velocity.y) < afc) onExpire();
}

json EnvSparkBolt::toJson() const {
    json j = EnvSpell::toJson();
    j["type"] = "EnvSparkBolt";
    return j;
}



EnvWand::EnvWand() {}

json EnvWand::toJson() const {
    json j = EnvItem::toJson();
    j["type"] = "EnvWand";

    // If Wand has its own toJson(), you can nest it:
    // if (wandData) j["wand"] = wandData->toJson();
    // else          j["wand"] = nullptr;

    return j;
}

EnvSparkBoltTrigger::EnvSparkBoltTrigger() {
    this->onExpire = [this]() {
        if (this->castState) {
            // Use whatever API you want on CastState
            // If CastState has a free function castToWorld or similar:
            this->castState->spawnAll(
                    SpellTransform({ rect.x, rect.y }, atan2f(this->velocity.y, this->velocity.x) * (180.0f / PI)),
                    *this->castState
                    );
        }
        this->dead = true;
    };
    color = RED;
}

void EnvSparkBoltTrigger::update() {
    rect.x += velocity.x;
    rect.y += velocity.y;
    velocity.x *= AIR_FRICTION_F;
    velocity.y *= AIR_FRICTION_F;
    float afc = AIR_FRICTION_C;
    if ((fabsf(velocity.x) < afc) && fabsf(velocity.y) < afc) onExpire();
}

json EnvSparkBoltTrigger::toJson() const {
    json j = EnvSpell::toJson();
    j["type"] = "EnvSparkBoltTrigger";
    return j;
}