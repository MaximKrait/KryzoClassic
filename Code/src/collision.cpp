#include "collision.h"

bool circleTriangleCollision(
    float cx, float cy, float radius,
    float x1, float y1,
    float x2, float y2,
    float x3, float y3
) {
    auto pointInCircle = [&](float px, float py) {
        float dx = px - cx;
        float dy = py - cy;
        return dx*dx + dy*dy <= radius*radius;
    };

    if (pointInCircle(x1, y1)) return true;
    if (pointInCircle(x2, y2)) return true;
    if (pointInCircle(x3, y3)) return true;

    auto distToSegment = [&](float px, float py, float ax, float ay, float bx, float by) {
        float dx = bx - ax;
        float dy = by - ay;
        float t = ((px - ax)*dx + (py - ay)*dy) / (dx*dx + dy*dy);
        t = std::clamp(t, 0.0f, 1.0f);
        float closestX = ax + t * dx;
        float closestY = ay + t * dy;
        float distX = px - closestX;
        float distY = py - closestY;
        return distX*distX + distY*distY;
    };

    if (distToSegment(cx, cy, x1, y1, x2, y2) <= radius*radius) return true;
    if (distToSegment(cx, cy, x2, y2, x3, y3) <= radius*radius) return true;
    if (distToSegment(cx, cy, x3, y3, x1, y1) <= radius*radius) return true;

    return false;
}