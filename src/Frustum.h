#pragma once

#include "Mat4.h"
#include "Vector.h"
#include "WO.h"
#include "GLView.h"

using namespace Aftr;
namespace Aftr
{
    class Frustum
    {
    public:
        // Holds the six frustum planes
        enum Plane { LEFT, RIGHT, TOP, BOTTOM, NEAR, FAR };
        struct PlaneData {
            Aftr::VectorT<float> normal;  // x, y, z (normal to the plane)
            float w;                      // w term (distance from the origin)
        };
        PlaneData planes[6];  // The normals of the six planes (A, B, C, D coefficients)

        // Extract planes from a projection-view matrix
        void extractPlanes(const Mat4& projViewMatrix);

        // Check if a point is inside the frustum
        bool isPointInFrustum(const Vector& point);

        // Check if an object (bounding box or sphere) is inside the frustum
        bool isWOInFrustum(const WO* wo);
    };


}