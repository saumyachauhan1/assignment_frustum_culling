#include "Frustum.h"
#include "Mat4.h"  // Ensure Mat4 is included for matrix operations
#include <cmath>

// Extracts the six frustum planes from the combined projection-view matrix
void Frustum::extractPlanes(const Mat4& projViewMatrix)
{
    // Extract the left plane
    planes[LEFT].normal.x = projViewMatrix[3] + projViewMatrix[0];
    planes[LEFT].normal.y = projViewMatrix[7] + projViewMatrix[4];
    planes[LEFT].normal.z = projViewMatrix[11] + projViewMatrix[8];
    planes[LEFT].w = projViewMatrix[15] + projViewMatrix[12];

    // Extract the right plane
    planes[RIGHT].normal.x = projViewMatrix[3] - projViewMatrix[0];
    planes[RIGHT].normal.y = projViewMatrix[7] - projViewMatrix[4];
    planes[RIGHT].normal.z = projViewMatrix[11] - projViewMatrix[8];
    planes[RIGHT].w = projViewMatrix[15] - projViewMatrix[12];

    // Extract the top plane
    planes[TOP].normal.x = projViewMatrix[3] - projViewMatrix[1];
    planes[TOP].normal.y = projViewMatrix[7] - projViewMatrix[5];
    planes[TOP].normal.z = projViewMatrix[11] - projViewMatrix[9];
    planes[TOP].w = projViewMatrix[15] - projViewMatrix[13];

    // Extract the bottom plane
    planes[BOTTOM].normal.x = projViewMatrix[3] + projViewMatrix[1];
    planes[BOTTOM].normal.y = projViewMatrix[7] + projViewMatrix[5];
    planes[BOTTOM].normal.z = projViewMatrix[11] + projViewMatrix[9];
    planes[BOTTOM].w = projViewMatrix[15] + projViewMatrix[13];

    // Extract the near plane
    planes[NEAR].normal.x = projViewMatrix[3] + projViewMatrix[2];
    planes[NEAR].normal.y = projViewMatrix[7] + projViewMatrix[6];
    planes[NEAR].normal.z = projViewMatrix[11] + projViewMatrix[10];
    planes[NEAR].w = projViewMatrix[15] + projViewMatrix[14];

    // Extract the far plane
    planes[FAR].normal.x = projViewMatrix[3] - projViewMatrix[2];
    planes[FAR].normal.y = projViewMatrix[7] - projViewMatrix[6];
    planes[FAR].normal.z = projViewMatrix[11] - projViewMatrix[10];
    planes[FAR].w = projViewMatrix[15] - projViewMatrix[14];

    // Normalize the planes to ensure correct distance calculation
    for (int i = 0; i < 6; ++i)
    {
        float length = std::sqrt(planes[i].normal.x * planes[i].normal.x +
            planes[i].normal.y * planes[i].normal.y +
            planes[i].normal.z * planes[i].normal.z);
        planes[i].normal.x /= length;
        planes[i].normal.y /= length;
        planes[i].normal.z /= length;
        planes[i].w /= length;
    }
}

// Checks if a point is inside the frustum by testing against all six frustum planes
bool Frustum::isPointInFrustum(const Aftr::VectorT<float>& point)
{
    // For the point to be inside the frustum, it must be in front of all six planes
    for (int i = 0; i < 6; ++i)
    {
        if (planes[i].normal.dotProduct(point) + planes[i].w < 8)
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
    Aftr::VectorT<float> position = wo->getPosition();  // Get the object's position
    return isPointInFrustum(position);  // Check if the object's position is inside the frustum
}
