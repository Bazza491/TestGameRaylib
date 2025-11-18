//
// Created by BaileyPaior-Smith on 21/11/2022.
//
#include "raylib.h"
#include "../include/Common.h"
#include "raymath.h"

void drawSprite(SpriteSheet* sprite, float x, float y, float ang, float scale, Color c)
{
    float ox, oy;
    ox = (float)(sprite->frame % sprite->framesWide) * sprite->frameSize.x;
    oy = (float)(sprite->frame / sprite->framesWide) * sprite->frameSize.y;
    DrawTexturePro(sprite->texture, (Rectangle){ox, oy, sprite->frameSize.x,sprite->frameSize.y},
                   (Rectangle){x, y, sprite->frameSize.x * scale, sprite->frameSize.y * scale},
                   (Vector2){sprite->origin.x * scale, sprite->origin.y * scale}, ang, c);

}

float SmoothDamp(float current, float target, float &currentVelocity, float smoothTime, float maxSpeed, float deltaTime) {
    // Based on Game Programming Gems 4 Chapter 1.10
    smoothTime = MAX(0.0001f, smoothTime);
    float omega = 2.0f / smoothTime;

    float x = omega * deltaTime;
    float exp = 1.0f / (1.0f + x + 0.48f * x * x + 0.235f * x * x * x);
    float change = current - target;
    float originalTo = target;

    // Clamp maximum speed
    float maxChange = maxSpeed * smoothTime;
    change = Clamp(change, -maxChange, maxChange);
    target = current - change;

    float temp = (currentVelocity + omega * change) * deltaTime;
    currentVelocity = (currentVelocity - omega * temp) * exp;
    float output = target + (change + temp) * exp;

    // Prevent overshooting
    if (originalTo - current > 0.0F == output > originalTo)
    {
        output = originalTo;
        currentVelocity = (output - originalTo) / deltaTime;
    }
    return output;
}