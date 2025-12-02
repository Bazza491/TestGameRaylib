#include "world/Physics.h"

#include <algorithm>
#include <cmath>

namespace Physics {

Collider makeAABB(Rectangle rect) {
    Collider c;
    c.type = ColliderType::AABB;
    c.rect = rect;
    return c;
}

Collider makeCircle(Vector2 center, float radius) {
    Collider c;
    c.type = ColliderType::Circle;
    c.center = center;
    c.radius = radius;
    return c;
}

Collider makePolygon(const std::vector<Vector2> &points) {
    Collider c;
    c.type = ColliderType::Polygon;
    c.points = points;
    return c;
}

void translate(Collider &collider, Vector2 delta) {
    switch (collider.type) {
        case ColliderType::AABB:
            collider.rect.x += delta.x;
            collider.rect.y += delta.y;
            break;
        case ColliderType::Circle:
            collider.center.x += delta.x;
            collider.center.y += delta.y;
            break;
        case ColliderType::Polygon:
            for (auto &p : collider.points) {
                p.x += delta.x;
                p.y += delta.y;
            }
            break;
    }
}

static float dot(Vector2 a, Vector2 b) {
    return a.x * b.x + a.y * b.y;
}

static Vector2 normalize(Vector2 v) {
    float len = std::sqrt(v.x * v.x + v.y * v.y);
    if (len <= 0.0001f) return {0.0f, 0.0f};
    return {v.x / len, v.y / len};
}

Rectangle computeAABB(const Collider &collider) {
    switch (collider.type) {
        case ColliderType::AABB:
            return collider.rect;
        case ColliderType::Circle:
            return {collider.center.x - collider.radius, collider.center.y - collider.radius,
                    collider.radius * 2.0f, collider.radius * 2.0f};
        case ColliderType::Polygon: {
            if (collider.points.empty()) return {0, 0, 0, 0};
            float minX = collider.points[0].x, maxX = collider.points[0].x;
            float minY = collider.points[0].y, maxY = collider.points[0].y;
            for (const auto &p : collider.points) {
                minX = std::min(minX, p.x);
                maxX = std::max(maxX, p.x);
                minY = std::min(minY, p.y);
                maxY = std::max(maxY, p.y);
            }
            return {minX, minY, maxX - minX, maxY - minY};
        }
    }
    return {0, 0, 0, 0};
}

Rectangle computeBroadphaseAABB(const Collider &collider, Vector2 delta) {
    Rectangle box = computeAABB(collider);
    Rectangle broad = box;
    if (delta.x > 0) broad.width += delta.x; else { broad.x += delta.x; broad.width -= delta.x; }
    if (delta.y > 0) broad.height += delta.y; else { broad.y += delta.y; broad.height -= delta.y; }
    return broad;
}

static bool polygonOverlap(const std::vector<Vector2> &a, const std::vector<Vector2> &b) {
    auto overlapOnAxis = [](const std::vector<Vector2> &poly1, const std::vector<Vector2> &poly2, Vector2 axis) {
        float min1 = dot(poly1[0], axis), max1 = min1;
        for (const auto &p : poly1) {
            float proj = dot(p, axis);
            min1 = std::min(min1, proj);
            max1 = std::max(max1, proj);
        }
        float min2 = dot(poly2[0], axis), max2 = min2;
        for (const auto &p : poly2) {
            float proj = dot(p, axis);
            min2 = std::min(min2, proj);
            max2 = std::max(max2, proj);
        }
        return !(max1 < min2 || max2 < min1);
    };

    auto addAxes = [](const std::vector<Vector2> &poly, std::vector<Vector2> &axes) {
        for (size_t i = 0; i < poly.size(); ++i) {
            Vector2 p1 = poly[i];
            Vector2 p2 = poly[(i + 1) % poly.size()];
            Vector2 edge{p2.x - p1.x, p2.y - p1.y};
            Vector2 axis = normalize({-edge.y, edge.x});
            axes.push_back(axis);
        }
    };

    std::vector<Vector2> axes;
    addAxes(a, axes);
    addAxes(b, axes);

    for (auto axis : axes) {
        if (!overlapOnAxis(a, b, axis)) return false;
    }
    return true;
}

static bool circlePolygonOverlap(const Collider &circle, const Collider &poly) {
    // Early-out using AABB
    if (!CheckCollisionRecs(computeAABB(circle), computeAABB(poly))) return false;

    // Check distance to edges
    for (size_t i = 0; i < poly.points.size(); ++i) {
        Vector2 p1 = poly.points[i];
        Vector2 p2 = poly.points[(i + 1) % poly.points.size()];
        Vector2 edge{p2.x - p1.x, p2.y - p1.y};
        Vector2 toCenter{circle.center.x - p1.x, circle.center.y - p1.y};
        float t = dot(toCenter, edge) / dot(edge, edge);
        t = std::clamp(t, 0.0f, 1.0f);
        Vector2 closest{p1.x + edge.x * t, p1.y + edge.y * t};
        Vector2 diff{circle.center.x - closest.x, circle.center.y - closest.y};
        if (dot(diff, diff) <= circle.radius * circle.radius) return true;
    }

    // Or inside polygon
    std::vector<Vector2> pointList{circle.center};
    return polygonOverlap(poly.points, pointList);
}

bool overlaps(const Collider &a, const Collider &b) {
    if (a.type == ColliderType::AABB && b.type == ColliderType::AABB) {
        return CheckCollisionRecs(a.rect, b.rect);
    }
    if (a.type == ColliderType::Circle && b.type == ColliderType::Circle) {
        return CheckCollisionCircles(a.center, a.radius, b.center, b.radius);
    }
    if ((a.type == ColliderType::Circle && b.type == ColliderType::AABB) ||
        (a.type == ColliderType::AABB && b.type == ColliderType::Circle)) {
        const Collider &circle = (a.type == ColliderType::Circle) ? a : b;
        const Collider &rect = (a.type == ColliderType::AABB) ? a : b;
        return CheckCollisionCircleRec(circle.center, circle.radius, rect.rect);
    }
    if (a.type == ColliderType::Polygon && b.type == ColliderType::Polygon) {
        if (a.points.empty() || b.points.empty()) return false;
        return polygonOverlap(a.points, b.points);
    }
    if ((a.type == ColliderType::Polygon && b.type == ColliderType::AABB) ||
        (a.type == ColliderType::AABB && b.type == ColliderType::Polygon)) {
        const Collider &polyColl = (a.type == ColliderType::Polygon) ? a : b;
        const Collider &rectColl = (a.type == ColliderType::AABB) ? a : b;
        Collider rectPoly = makePolygon({
                {rectColl.rect.x, rectColl.rect.y},
                {rectColl.rect.x + rectColl.rect.width, rectColl.rect.y},
                {rectColl.rect.x + rectColl.rect.width, rectColl.rect.y + rectColl.rect.height},
                {rectColl.rect.x, rectColl.rect.y + rectColl.rect.height}
        });
        return polygonOverlap(polyColl.points, rectPoly.points);
    }
    if ((a.type == ColliderType::Circle && b.type == ColliderType::Polygon) ||
        (a.type == ColliderType::Polygon && b.type == ColliderType::Circle)) {
        const Collider &circle = (a.type == ColliderType::Circle) ? a : b;
        const Collider &poly = (a.type == ColliderType::Polygon) ? a : b;
        return circlePolygonOverlap(circle, poly);
    }
    return false;
}

SweepHit sweepAABB(const Collider &moving, Vector2 delta, const Collider &other) {
    SweepHit result;
    if (moving.type != ColliderType::AABB || other.type != ColliderType::AABB) return result;

    Rectangle movingBroad = computeBroadphaseAABB(moving, delta);
    if (!CheckCollisionRecs(movingBroad, other.rect)) return result;

    float entryTimeX, entryTimeY;
    float exitTimeX, exitTimeY;

    if (delta.x > 0.0f) {
        entryTimeX = (other.rect.x - (moving.rect.x + moving.rect.width)) / delta.x;
        exitTimeX = ((other.rect.x + other.rect.width) - moving.rect.x) / delta.x;
    } else if (delta.x < 0.0f) {
        entryTimeX = ((other.rect.x + other.rect.width) - moving.rect.x) / delta.x;
        exitTimeX = (other.rect.x - (moving.rect.x + moving.rect.width)) / delta.x;
    } else {
        entryTimeX = -INFINITY;
        exitTimeX = INFINITY;
    }

    if (delta.y > 0.0f) {
        entryTimeY = (other.rect.y - (moving.rect.y + moving.rect.height)) / delta.y;
        exitTimeY = ((other.rect.y + other.rect.height) - moving.rect.y) / delta.y;
    } else if (delta.y < 0.0f) {
        entryTimeY = ((other.rect.y + other.rect.height) - moving.rect.y) / delta.y;
        exitTimeY = (other.rect.y - (moving.rect.y + moving.rect.height)) / delta.y;
    } else {
        entryTimeY = -INFINITY;
        exitTimeY = INFINITY;
    }

    float entryTime = std::max(entryTimeX, entryTimeY);
    float exitTime = std::min(exitTimeX, exitTimeY);

    if (entryTime > exitTime || (entryTimeX < 0.0f && entryTimeY < 0.0f) || entryTime > 1.0f) {
        return result;
    }

    result.hit = true;
    result.timeOfImpact = std::max(entryTime, 0.0f);
    if (entryTimeX > entryTimeY) {
        result.normal = (delta.x < 0.0f) ? Vector2{1.0f, 0.0f} : Vector2{-1.0f, 0.0f};
    } else {
        result.normal = (delta.y < 0.0f) ? Vector2{0.0f, 1.0f} : Vector2{0.0f, -1.0f};
    }
    return result;
}

Vector2 resolvePenetration(const Collider &moving, const Collider &other) {
    Rectangle a = computeAABB(moving);
    Rectangle b = computeAABB(other);
    float dx1 = (b.x + b.width) - a.x;
    float dx2 = (a.x + a.width) - b.x;
    float dy1 = (b.y + b.height) - a.y;
    float dy2 = (a.y + a.height) - b.y;

    float minX = (std::abs(dx1) < std::abs(dx2)) ? dx1 : -dx2;
    float minY = (std::abs(dy1) < std::abs(dy2)) ? dy1 : -dy2;

    if (std::abs(minX) < std::abs(minY)) return {minX, 0.0f};
    return {0.0f, minY};
}

} // namespace Physics

