#pragma once

#include <optional>
#include <vector>

#include "raylib.h"

// Collider abstraction for world geometry and dynamic entities.
enum class ColliderType {
    AABB,
    Circle,
    Polygon
};

struct Collider {
    ColliderType type = ColliderType::AABB;
    // AABB data
    Rectangle rect{};
    // Circle data
    Vector2 center{};
    float radius = 0.0f;
    // Polygon data
    std::vector<Vector2> points{}; // World-space points in winding order
};

namespace Physics {

Rectangle computeAABB(const Collider &collider);
Rectangle computeBroadphaseAABB(const Collider &collider, Vector2 delta);
Collider makeAABB(Rectangle rect);
Collider makeCircle(Vector2 center, float radius);
Collider makePolygon(const std::vector<Vector2> &points);
void translate(Collider &collider, Vector2 delta);

bool overlaps(const Collider &a, const Collider &b);

struct SweepHit {
    bool hit = false;
    float timeOfImpact = 1.0f; // fraction of delta applied
    Vector2 normal{0.0f, 0.0f};
};

SweepHit sweepAABB(const Collider &moving, Vector2 delta, const Collider &other);

Vector2 resolvePenetration(const Collider &moving, const Collider &other);

} // namespace Physics

