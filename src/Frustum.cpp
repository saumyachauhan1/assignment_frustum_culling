#include "Frustum.h"
#include <cmath>

// Extracts the six frustum planes from the combined projection-view matrix
void Frustum::extractPlanes(const Mat4& projViewMatrix)
{
    // Extract the left plane
    planes[LEFT].x = projViewMatrix[3] + projViewMatrix[0];
    planes[LEFT].y = projViewMatrix[7] + projViewMatrix[4];
    planes[LEFT].z = projViewMatrix[11] + projViewMatrix[8];
    planes[LEFT].w = projViewMatrix[15] + projViewMatrix[12];

    // Extract the right plane
    planes[RIGHT].x = projViewMatrix[3] - projViewMatrix[0];
    planes[RIGHT].y = projViewMatrix[7] - projViewMatrix[4];
    planes[RIGHT].z = projViewMatrix[11] - projViewMatrix[8];
    planes[RIGHT].w = projViewMatrix[15] - projViewMatrix[12];

    // Extract the top plane
    planes[TOP].x = projViewMatrix[3] - projViewMatrix[1];
    planes[TOP].y = projViewMatrix[7] - projViewMatrix[5];
    planes[TOP].z = projViewMatrix[11] - projViewMatrix[9];
    planes[TOP].w = projViewMatrix[15] - projViewMatrix[13];

    // Extract the bottom plane
    planes[BOTTOM].x = projViewMatrix[3] + projViewMatrix[1];
    planes[BOTTOM].y = projViewMatrix[7] + projViewMatrix[5];
    planes[BOTTOM].z = projViewMatrix[11] + projViewMatrix[9];
    planes[BOTTOM].w = projViewMatrix[15] + projViewMatrix[13];

    // Extract the near plane
    planes[NEAR].x = projViewMatrix[3] + projViewMatrix[2];
    planes[NEAR].y = projViewMatrix[7] + projViewMatrix[6];
    planes[NEAR].z = projViewMatrix[11] + projViewMatrix[10];
    planes[NEAR].w = projViewMatrix[15] + projViewMatrix[14];

    // Extract the far plane
    planes[FAR].x = projViewMatrix[3] - projViewMatrix[2];
    planes[FAR].y = projViewMatrix[7] - projViewMatrix[6];
    planes[FAR].z = projViewMatrix[11] - projViewMatrix[10];
    planes[FAR].w = projViewMatrix[15] - projViewMatrix[14];

    // Normalize the planes to ensure correct distance calculation
    for (int i = 0; i < 6; ++i)
    {
        float length = std::sqrt(planes[i].x * planes[i].x + planes[i].y * planes[i].y + planes[i].z * planes[i].z);
        planes[i].x /= length;
        planes[i].y /= length;
        planes[i].z /= length;
        planes[i].w /= length;
    }
}

// Checks if a point is inside the frustum by testing against all six frustum planes
bool Frustum::isPointInFrustum(const Vector& point)
{
    // For the point to be inside the frustum, it must be in front of all six planes
    for (int i = 0; i < 6; ++i)
    {
        if (planes[i].dot(point) + planes[i].w < 0)
        {
            // The point is outside this plane, hence outside the frustum
            return false;
        }
    }
    return true;
}

// Checks if a world object (WO) is inside the frustum by checking its position
bool Frustum::isWOInFrustum(const WO* wo)
{
    Vector position = wo->getPosition();  // Get the object's position
    return isPointInFrustum(position);    // Check if the object's position is inside the frustum
}

