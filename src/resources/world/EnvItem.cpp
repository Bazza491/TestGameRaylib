#include "../../../include/resources/world/EnvItem.h"
#include "../../../include/Common.h"




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
        if (velocity.x <= afc) velocity.x -= afc;
        if (velocity.x >= -afc) velocity.x -= afc;
        if (velocity.x <= afc) velocity.x -= afc;
        if (velocity.x >= -afc) velocity.x -= afc;
        if (-afc >= velocity.x && velocity.x <= afc) velocity.x = 0.0f;
        if (-afc >= velocity.y && velocity.y <= afc) velocity.y = 0.0f;
    }
}



bool EnvItem::isColliding(Rectangle rec) {
    Rectangle bigger = (Rectangle){rect.x-1.0f, rect.y-1.0f, rect.width+2.0f, rect.height+2.0f};
    return CheckCollisionRecs(rec, bigger);
}



// Getters and setters
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


EnvSpell::EnvSpell() : EnvItem() {}
void EnvSpell::update() {
    rect.x += velocity.x;
    rect.y += velocity.y;
    velocity.x *= AIR_FRICTION_F;
    velocity.y *= AIR_FRICTION_F;
    float afc = AIR_FRICTION_C;
    if (velocity.x <= afc) velocity.x -= afc;
    if (velocity.x >= -afc) velocity.x -= afc;
    if (velocity.x <= afc) velocity.x -= afc;
    if (velocity.x >= -afc) velocity.x -= afc;
    if (-afc >= velocity.x && velocity.x <= afc) velocity.x = 0.0f;
    if (-afc >= velocity.y && velocity.y <= afc) velocity.y = 0.0f;

    if (velocity.x == 0.0f && velocity.y == 0.0f) {
        dead = true;
    }
}

//void EnvSpell::setCastState(CastState state) {
//    castState = state;
//}

EnvSparkBolt::EnvSparkBolt(){}

void EnvSparkBolt::update() {
    rect.x += velocity.x;
    rect.y += velocity.y;
    velocity.x *= AIR_FRICTION_F;
    velocity.y *= AIR_FRICTION_F;
    float afc = AIR_FRICTION_C;
    if (velocity.x <= afc) velocity.x -= afc;
    if (velocity.x >= -afc) velocity.x -= afc;
    if (velocity.x <= afc) velocity.x -= afc;
    if (velocity.x >= -afc) velocity.x -= afc;
    if (-afc >= velocity.x && velocity.x <= afc) velocity.x = 0.0f;
    if (-afc >= velocity.y && velocity.y <= afc) velocity.y = 0.0f;

    if (velocity.x == 0.0f && velocity.y == 0.0f) {
        dead = true;
    }
}

