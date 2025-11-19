#include <cmath>
#include "raylib.h"
#include "Common.h"
#include "world/EnvItem.h"
#include "guns/SpellStorage.h"
#include "guns/Wand.h"
#include "Player.h"
#include "raymath.h"
#include "guns/SpellTransform.h"
#include "world/World.h"
#include "guns/Spell.h"

const int HAND_OFFSET_X = 40; // offset right/50
const int HAND_OFFSET_Y = 35; // offset down/100

Player::Player(Texture2D spriteSheet) : Player(spriteSheet, {0, 0}) {}
Player::Player(Texture2D spriteSheet, Vector2 startPos) : pos(startPos), spells(16), velocity({0, 0}) {
    hitBox = {pos.x, pos.y, 50.0f, 100.0f};
    selectedWandSlot = 0;

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

    //region initialise wands
    for (int i = 0; i < 4; i++) {
        //TODO: Finalise starting wand stats
        WandStats wandStats = WandStats(
                false,
                1,
                0.5f,
                2.0f,
                100,
                10,
                6,
                30.0f);


        // create a placeholder colored texture
        Color placeholderColor;
        switch (i) {
            case 0: placeholderColor = BLACK; break;
            case 1: placeholderColor = GREEN; break;
            case 2: placeholderColor = BLUE; break;
            case 3: placeholderColor = YELLOW; break;
            default: placeholderColor = WHITE; break;
        }

        Image wandImage = GenImageColor(64, 16, placeholderColor); // 32x8 rectangle
        Texture2D wandTexture = LoadTextureFromImage(wandImage);
        UnloadImage(wandImage); // free the CPU-side image

        wands[i] = std::make_unique<Wand>(wandStats, wandTexture);
        wands[i]->getSpellStorage().insertSpell(std::make_unique<SparkBoltTrigger>(), 0);
        wands[i]->getSpellStorage().insertSpell(std::make_unique<DrawTwo>(), 1);
        wands[i]->getSpellStorage().insertSpell(std::make_unique<SparkBolt>(), 2);
        wands[i]->getSpellStorage().insertSpell(std::make_unique<SparkBolt>(), 3);
    }
    selectedWandSlot = 0;
    //endregion
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

void Player::update(float delta, Vector2 mouseWorldPos) {
    float move = IsKeyDown(KEY_A) && IsKeyDown(KEY_D) ? 0.0f :
                 IsKeyDown(KEY_A) ? -1.0f :
                 IsKeyDown(KEY_D) ?  1.0f : 0.0f;

    // Apply friction and gravity
    velocity.x *= AIR_FRICTION_F;
    velocity.y *= AIR_FRICTION_F;

    float afc = AIR_FRICTION_C;
    if (velocity.x > -afc && velocity.x < afc) velocity.x = 0.0f; // prevent overshooting
    if (velocity.x >  afc) velocity.x -= afc;
    if (velocity.x < -afc) velocity.x += afc;

    velocity.y += G;

    // --- Collision detection using World singleton ---
    const auto& envItems = World::getInstance().getItems();
    for (const auto& env : envItems) {
        const Rectangle& rect = env->getRect();

        if (!CheckCollisionRecs(hitBox, rect)) continue; // if not colliding, skip

        Rectangle overlap = GetCollisionRec(hitBox, rect);
        if (overlap.height < overlap.width) {
            // Vertical collision
            if (overlap.y == rect.y) {
                // Landed on top
                pos.y = rect.y - hitBox.height + 1;
                if (!IsKeyDown(KEY_W)) {
                    velocity.y = 0;
                } else {
                    velocity.y = -20.0f;
                }
            } else {
                // Hit from below
                pos.y = rect.y + rect.height;
                velocity.y = 0;
            }
        } else {
            // Horizontal collision
            pos.x = (overlap.x == rect.x)
                    ? rect.x - hitBox.width
                    : rect.x + rect.width;
            velocity.x = 0;
        }
    }

    // Levitation mechanic (W key)
    if (IsKeyDown(KEY_W) && velocity.y > -MAX_LEVSPEED) {
        if (velocity.y > 10.0f) velocity.y = 10.0f; // cancel most falling velocity
        velocity.y -= LEVSPEED;
        if (velocity.y < -MAX_LEVSPEED) velocity.y = -MAX_LEVSPEED;
    }

    // Horizontal movement
    velocity.x += move * SPEED;

    // Update animation state
    if (velocity.x > 0 && state != walkL) {
        setState(walkL);
    } else if (velocity.x < 0 && state != walkR) {
        setState(walkR);
    }

    // Apply velocity
    pos = Vector2Add(pos, velocity);
    hitBox.x = pos.x;
    hitBox.y = pos.y;

    // Update wand rotation
    Vector2 handPos = { pos.x + HAND_OFFSET_X, pos.y + HAND_OFFSET_Y };
    Vector2 diff = { mouseWorldPos.x - handPos.x, mouseWorldPos.y - handPos.y };
    wandRotation = atan2f(diff.y, diff.x) * RAD2DEG;
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

    // draw held wand if it exists
    if (wands[selectedWandSlot]) {
        Texture2D wandTex = wands[selectedWandSlot]->getTexture();
        Vector2 origin = { 0, (float)wandTex.height / 2.0f };
        Vector2 handPos = { pos.x + HAND_OFFSET_X, pos.y + HAND_OFFSET_Y};

        DrawTexturePro(
                wandTex,
                { 0, 0, (float)wandTex.width, (float)wandTex.height },
                { handPos.x, handPos.y, (float)wandTex.width, (float)wandTex.height },
                origin,
                wandRotation,
                WHITE
        );
    }
}

Vector2 Player::getWandTip() const {
    float rad = wandRotation * DEG2RAD;
    Texture2D wandTex = wands[selectedWandSlot]->getTexture();
    Vector2 origin = {0, (float)wandTex.height / 2.0f}; // same as DrawTexturePro

    // Tip offset relative to origin
    Vector2 tipOffset = { (float)wandTex.width - origin.x, 0 };

    // Rotate tip offset
    Vector2 rotatedTip = {
            tipOffset.x * cosf(rad) - tipOffset.y * sinf(rad),
            tipOffset.x * sinf(rad) + tipOffset.y * cosf(rad)
    };

    // Tip world position
    Vector2 tipPos = { pos.x + HAND_OFFSET_X + rotatedTip.x, pos.y + HAND_OFFSET_Y + rotatedTip.y };

    // Adjust for spell origin
    tipPos.x -= 4.0f;
    tipPos.y -= 4.0f;

    return tipPos;
}

void Player::cast() {
    if (!wands[selectedWandSlot]) return;

    Vector2 tipPos = getWandTip();

    // Create spell transform
    SpellTransform theTipOfTheWand(tipPos, wandRotation);

    // Cast spell
    wands[selectedWandSlot]->cast(theTipOfTheWand);
}

//region Getters and Setters
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
float Player::getWandRotation() const {
    return wandRotation;
}
//endregion