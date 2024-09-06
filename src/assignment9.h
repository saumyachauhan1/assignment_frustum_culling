#pragma once

#include "GLView.h"
#include <irrKlang.h>
#include <string>
#include <vector>
#include "Vector.h"
#include "Mat4.h"
#include "Frustum.h"  // Include the Frustum class

using namespace irrklang;

namespace Aftr
{
    class Camera;

    class GLViewNewModule : public GLView
    {
    public:
        static GLViewNewModule* New(const std::vector<std::string>& outArgs);
        virtual ~GLViewNewModule();
        virtual void updateWorld(); ///< Called once per frame
        virtual void loadMap(); ///< Called once at startup to build this module's scene
        virtual void createNewModuleWayPoints();
        virtual void onResizeWindow(GLsizei width, GLsizei height);
        virtual void onMouseDown(const SDL_MouseButtonEvent& e);
        virtual void onMouseUp(const SDL_MouseButtonEvent& e);
        virtual void onMouseMove(const SDL_MouseMotionEvent& e);
        virtual void onKeyDown(const SDL_KeyboardEvent& key);
        virtual void onKeyUp(const SDL_KeyboardEvent& key);
        virtual bool hasReachedEndpoint();
        virtual void performSecondaryFrustumCulling();
        virtual VectorT<float> getPosition(WO* obj);
        virtual VectorT<float> getForward(WO* obj);
        virtual VectorT<float> getUp(WO* obj);
        virtual VectorT<float> getRight(WO* obj);
        Mat4 customPerspective(float fov, float aspectRatio, float nearPlane, float farPlane);
        Mat4 customLookAt(const VectorT<float>& eye, const VectorT<float>& target, const VectorT<float>& up);
    
    protected:
        GLViewNewModule();
        virtual void onCreate();

        WO* startPoint;  // Point A
        WO* endPoint;    // Point B
        WO* playerBox;   // The box the player controls
        WO* sphere;      // A sphere added to the scene

        Vector startPos; // Starting position of the player box
        Vector endPos;   // End position (point B)

        Frustum frustum; // Main camera frustum for culling
        bool isboxmovig = false;

    };

} // namespace Aftr
