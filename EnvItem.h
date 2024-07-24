//
// Created by BaileyPaior-Smith on 3/11/2022.
//

#pragma once
#include "raylib.h"

class EnvItem {
protected:
    Rectangle rect;
    bool blocking;
    bool hasPhysics;
    Color color;
    Texture2D texture;
    Vector2 velocity;
public:
    EnvItem() {}
    EnvItem(Rectangle rect, bool blocking, bool hasPhysics, Texture2D texture);
    EnvItem(Rectangle rect, bool blocking, bool hasPhysics, Texture2D texture, Vector2 velocity);
    EnvItem(Rectangle rect, bool blocking, bool hasPhysics, Color color);
    EnvItem(Rectangle rect, Color color);
    virtual void update();
    bool isColliding(Rectangle rec);

    // Getters and setters
    Rectangle getRect() const;
    void setRect(Rectangle rect);
    bool isBlocking() const;
    void setBlocking(bool blocking);
    bool isPhysics() const;
    void setHasPhysics(bool hasPhysics);
    Color getColor() const;
    void setColor(Color color);
    Texture2D getTexture() const;
    void setTexture(Texture2D texture);
};