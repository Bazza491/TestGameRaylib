//
// Created by BaileyPaior-Smith on 21/11/2022.
//

#include <cmath>
#include "raylib.h"
#include "Common.h"
#include "EnvItem.h"
#include "resources/guns/SpellStorage.h"
#include "resources/guns/Wand.h"
#include "Player.h"
#include "raymath.h"
#include <iostream>

Player::Player(Texture2D spriteSheet) : Player(spriteSheet, {0, 0}) {}
Player::Player(Texture2D spriteSheet, Vector2 startPos) : pos(startPos), spells(16), velocity({0, 0}) {
    hitBox = {pos.x, pos.y, 50.0f, 100.0f};
    selectedWandSlot = 1;

    // initialise sprite
    sprite = { 0 };
    sprite.texture = spriteSheet;
    sprite.frameSize = {(float)spriteSheet.width/3, (float)spriteSheet.height/4};
    sprite.framesWide = 3;
    sprite.origin = {sprite.frameSize.x/2.0f, sprite.frameSize.y/2.0f};
    frameSpeed = 3.0f;
    timeElapsed = 0.0f;

    // default animation data
    sprite.startFrame = 0;
    sprite.aniLength = 10;
}
void Player::setState(PlayerState newState) {
    state = newState;
    switch (state) {
        case walkL:
            sprite.startFrame = 0;
            sprite.frame = 0;
            sprite.aniLength = 3;
            break;
        case walkR:
            sprite.startFrame = 3;
            sprite.frame = 3;
            sprite.aniLength = 3;
            break;
        case idleL:
            sprite.startFrame = 20;
            sprite.aniLength = 2;
            break;
        case idleR:
            sprite.startFrame = 0;
            sprite.aniLength = 10;
            break;
        case jumpL:
            sprite.startFrame = 0;
            sprite.aniLength = 10;
            break;
        case jumpR:
            sprite.startFrame = 0;
            sprite.aniLength = 10;
            break;
        case fallingL:
            sprite.startFrame = 0;
            sprite.aniLength = 10;
            break;
        case fallingR:
            sprite.startFrame = 0;
            sprite.aniLength = 10;
            break;
        default:
            TraceLog(LOG_DEBUG, TextFormat("setState() unexpected state: [%i]", state));
            break;
    }
}
void Player::update (EnvItem *envItems, int envItemsLength, float delta) {
    float move = IsKeyDown(KEY_A) && IsKeyDown(KEY_D) ? 0.0f :
            IsKeyDown(KEY_A) ? -1.0f :
            IsKeyDown(KEY_D) ? 1.0f : 0.0f;

    // apply friction and gravity
    velocity.x *= AIR_FRICTION_F;
    velocity.y *= AIR_FRICTION_F;
    float afc = AIR_FRICTION_C;
    if (velocity.x > -afc && velocity.x < afc) velocity.x = 0.0f; // prevent overshooting
    if (velocity.x > afc) velocity.x -= afc;
    if (velocity.x < -afc) velocity.x += afc;
    velocity.y += G;

    // update velocity
    EnvItem *p = envItems;
    Rectangle rect;
    for (int i = 0; i < envItemsLength; i++) {
        rect = (p+i)->getRect();
        if (!CheckCollisionRecs(hitBox, rect)) continue; //if colliding with an environment item
        Rectangle overlap = GetCollisionRec(hitBox, rect);
        if (overlap.height < overlap.width) {
            if (overlap.y == rect.y) {
                pos.y = rect.y - hitBox.height + 1;
                if (!IsKeyDown(KEY_W)) {
                    velocity.y = 0;
                } else {
                    velocity.y = -20.0f;
                }
            } else {
                pos.y = rect.y + rect.height;
                velocity.y = 0;
            }
        } else {
            pos.x = (overlap.x == rect.x) ? rect.x - hitBox.width :  rect.x + rect.width;
            velocity.x = 0;
        }
    }
    if (IsKeyDown(KEY_W) && velocity.y > -MAX_LEVSPEED) { //if holding W and not already moving faster than max levitation speed
        if (velocity.y > 10.0f) velocity.y = 10.0f; // if falling remove all but 10 velocity
            velocity.y -= LEVSPEED;
        if (velocity.y < -MAX_LEVSPEED) velocity.y = -MAX_LEVSPEED; //clamp to max levitation speed
    }
    velocity.x += move * SPEED;

    if (velocity.x > 0 && state != walkL) {
        setState(walkL);
    } else if (velocity.x < 0 && state != walkR) {
        setState(walkR);
    }


//    velocity.x = SmoothDamp(velocity.x, (float)move*10.0f, velocity.x, 1.0f, 3.0f, delta);

    // apply velocity
    pos = Vector2Add(pos, velocity);
    hitBox.x = pos.x;
    hitBox.y = pos.y;

}

void Player::draw (float scaleX, float scaleY, float delta) {
    float ox, oy;
    ox = (float)(sprite.frame % sprite.framesWide) * sprite.frameSize.x;
    oy = (float)(sprite.frame / sprite.framesWide) * sprite.frameSize.y;
    DrawRectangleRec(hitBox, RED);
    DrawTexturePro(sprite.texture, {ox, oy, sprite.frameSize.x,sprite.frameSize.y},
                   hitBox,
                   {0, 0}, 0.0f, WHITE);
    timeElapsed += delta;
    if (timeElapsed >= 1.0f/frameSpeed) {
        sprite.frame++;
        timeElapsed = fmod(timeElapsed, 1.0f/(float)frameSpeed);
        if (sprite.frame >= sprite.aniLength + sprite.startFrame) {
            sprite.frame = sprite.startFrame;
        }
    }
}

Vector2 Player::getPos() const {
    return pos;
}
void Player::setPos(Vector2 newPos) {
    pos = newPos;
}
float Player::getVelY() const {
    return velocity.y;
}
void Player::setVelY(float velY) {
    velocity.y = velY;
}
float Player::getVelX() const {
    return velocity.x;
}
void Player::setVelX(float velX) {
    velocity.x = velX;
}
Vector2 Player::getVelocity() const {
    return velocity;
}
void Player::setVelocity(Vector2 newVelocity) {
    velocity = newVelocity;
}
Rectangle Player::getHitBox() const {
    return hitBox;
}
void Player::cast() {
    spells.cast();
}

//    Wand getWand(int slot) {
//        switch (slot) {
//            case 1:
//                return wandSlot1;
//            case 2:
//                return wandSlot2;
//            case 3:
//                return wandSlot3;
//            case 4:
//                return wandSlot4;
//            default:
//                TraceLog(LOG_DEBUG, "getWand() out of bounds");
//                return wandSlot1;
//        }
//    }
//    void setWand (Wand wand, int slot) {
//        switch (slot) {
//            case 1:
//                wandSlot1 = wand;
//                break;
//            case 2:
//                wandSlot2 = wand;
//                break;
//            case 3:
//                wandSlot3 = wand;
//                break;
//            case 4:
//                wandSlot4 = wand;
//                break;
//            default:
//                TraceLog(LOG_DEBUG, "setWand() out of bounds");
//                break;
//        }
//    }