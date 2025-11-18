//
// Created by BaileyPaior-Smith on 3/11/2022.
//

#pragma once

#define MAX(a, b) ((a)>(b)? (a) : (b))
#define MIN(a, b) ((a)<(b)? (a) : (b))
#define AIR_FRICTION_F 0.95f
#define AIR_FRICTION_C 1.0f
#define G 1.0f
#define SPEED 2.1f
#define LEVSPEED 3.0f
#define MAX_LEVSPEED 8.0f
#define getrandom(min, max) \
    ((rand()%(int)(((max) + 1)-(min)))+ (min))

typedef struct SpriteSheet {
    Texture2D texture; //the actual image associated with the sprite-sheet
    Vector2 frameSize; //the x and y pixel sizes of each frame image in the sprite-sheet for easy access
    int aniLength; //number of frames in the current animation
    int startFrame; // starting frame no. on sprite-sheet for current animation
    int framesWide; // no. of sub-images (frames) across in each row, number of columns of frames in sheet
    Vector2 origin; // relative center point of frame used for rotation
    int frame; // current from of the animation
} SpriteSheet;
enum PlayerState {
    walkL, walkR,
    idleL, idleR,
    jumpL, jumpR,
    fallingL, fallingR,
};


void drawSprite(SpriteSheet* sprite, float x, float y, float ang, float scale, Color c);
float SmoothDamp(float current, float target, float &currentVelocity, float smoothTime, float maxSpeed, float deltaTime);


