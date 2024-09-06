#include "Assignment9.h"

#include "WorldList.h" //This is where we place all of our WOs
#include "ManagerOpenGLState.h" //We can change OpenGL State attributes with this
#include "Axes.h" //We can set Axes to on/off with this
#include "PhysicsEngineODE.h"

//Different WO used by this module
#include "WO.h"
#include "WOStatic.h"
#include "WONVPhysX.h"
#include "WOStaticPlane.h"
#include "WOStaticTrimesh.h"
#include "WOTrimesh.h"
#include "WOHumanCyborg.h"
#include "WOHumanCal3DPaladin.h"
#include "WOWayPointSpherical.h"
#include "WOLight.h"
#include "WOSkyBox.h"
#include "WOCar1970sBeater.h"
#include "Camera.h"
#include "CameraStandard.h"
#include "CameraChaseActorSmooth.h"
#include "CameraChaseActorAbsNormal.h"
#include "CameraChaseActorRelNormal.h"
#include "Model.h"
#include "ModelDataShared.h"
#include "ModelMesh.h"
#include "ModelMeshDataShared.h"
#include "ModelMeshSkin.h"
#include "WONVStaticPlane.h"
#include "WONVPhysX.h"
#include "WONVDynSphere.h"
#include "WOImGui.h" //GUI Demos also need to #include "AftrImGuiIncludes.h"
#include "AftrImGuiIncludes.h"
#include "AftrGLRendererBase.h"
#include "Mat4.h"
#include "WOCar1970sBeater.h"
#include "WONVPhysX.h"
#include <WONVPhysX.h>
#include <irrKlang.h>
#include <iostream>
//#include "imgui.h"

#include "Frustum.h"

#include <iostream>
#include <cstring>
#include "winsock2.h"     // Winsock header for Windows
#include "ws2tcpip.h"     // Additional header for IP address functions

#pragma comment(lib, "Ws2_32.lib")  // Link against Winsock library

using namespace Aftr;

GLViewNewModule* GLViewNewModule::New(const std::vector<std::string>& args)
{
    GLViewNewModule* glv = new GLViewNewModule();
    glv->init(Aftr::GRAVITY, Vector(0, 0, -1.0f), "aftr.conf", PHYSICS_ENGINE_TYPE::petODE);
    glv->onCreate();
    return glv;
}

GLViewNewModule::GLViewNewModule() : GLView(args)
{
}

bool GLViewNewModule::hasReachedEndpoint()
{
    // Basic collision detection: check if the distance between the player box and end point is within a threshold
    return (playerBox->getPosition() - endPos).magnitude() < 0.5;
}

void GLViewNewModule::onCreate()
{
    // Create a new world object list and initialize the physics engine
    std::string shinyRedPlasticCube(ManagerEnvironmentConfiguration::getSMM() + "/models/cube4x4x4redShinyPlastic_pp.wrl");
    std::string sphereModel(ManagerEnvironmentConfiguration::getSMM() + "/models/sphereR5Earth.wrl");

    //startPoint = WO::New(shinyRedPlasticCube, Vector(1, 1, 1), MESH_SHADING_TYPE::mstFLAT);
    //startPoint->setPosition(startPos);
    //startPoint->setLabel("Start Point");
    //worldLst->push_back(startPoint);

    //endPoint = WO::New(shinyRedPlasticCube, Vector(1, 1, 1), MESH_SHADING_TYPE::mstFLAT);
    //endPoint->setPosition(endPos);
    //endPoint->setLabel("End Point");
    //worldLst->push_back(endPoint);

    playerBox = WO::New(shinyRedPlasticCube, Vector(1, 1, 1), MESH_SHADING_TYPE::mstFLAT);
    playerBox->setPosition(Vector(-10, 1, 1));
    playerBox->setLabel("box");
    worldLst->push_back(playerBox);

    sphere = WO::New(shinyRedPlasticCube, Vector(1, 1, 1), MESH_SHADING_TYPE::mstFLAT);
    sphere->setPosition(Vector(1, 10, 1));  // Position the sphere
    sphere->setLabel("earth");
    worldLst->push_back(sphere);

    sphere = WO::New(shinyRedPlasticCube, Vector(1, 1, 1), MESH_SHADING_TYPE::mstFLAT);
    sphere->setPosition(Vector(1, 20, 1));  // Position the sphere
    sphere->setLabel("earth1");
    worldLst->push_back(sphere);

    sphere = WO::New(shinyRedPlasticCube, Vector(1, 1, 1), MESH_SHADING_TYPE::mstFLAT);
    sphere->setPosition(Vector(1, 30, 1));  // Position the sphere
    sphere->setLabel("earth2");
    worldLst->push_back(sphere);

    sphere = WO::New(shinyRedPlasticCube, Vector(1, 1, 1), MESH_SHADING_TYPE::mstFLAT);
    sphere->setPosition(Vector(1, 40, 1));  // Position the sphere
    sphere->setLabel("earth3");
    worldLst->push_back(sphere);

    createNewModuleWayPoints();
}


GLViewNewModule::~GLViewNewModule()
{
}

void GLViewNewModule::updateWorld()
{
    GLView::updateWorld();

    // Extract frustum planes from the main camera's projection-view matrix

    if (!this->isboxmovig)
    {
        Mat4 projViewMatrix = this->getCamera()->getCameraProjectionMatrix() * this->getCamera()->getCameraViewMatrix();
        std::cout << projViewMatrix << std::endl;
        frustum.extractPlanes(projViewMatrix);
        for (auto it = this->worldLst->begin(); it != this->worldLst->end(); ++it)
        {
            WO* wo = *it;
            std::cout << wo->getLabel() << std::endl;
            if ((!frustum.isWOInFrustum(wo)) && (wo->getLabel().compare("box") == 1 or wo->getLabel().compare("earth") == 1 or wo->getLabel().compare("earth1") == 1 or wo->getLabel().compare("earth2") == 1 or wo->getLabel().compare("earth3") == 1))
            {
                // Render the object since it's inside the frustum
                wo->isVisible = false;
                std::cout << wo->getLabel() << " is outside the frustum and will not be rendered.\n";


            }
            else
            {
                wo->render(*(this->getCamera()));
                wo->isVisible = true;
            }
        }
    }
    else {
        this->performSecondaryFrustumCulling();
    }
}


void GLViewNewModule::onKeyDown(const SDL_KeyboardEvent& key)
{
    GLView::onKeyDown(key);
    auto lookDirection = this->getCamera()->getLookDirection();
    auto normalDirection = this->getCamera()->getNormalDirection();
    // Calculate the right direction using the cross product
    auto rightDirection = lookDirection.crossProduct(normalDirection);
    
    //WO* objToMove = this->worldLst->at(1); // Assuming object to move is at index 1
    Vector movementVector;

    switch (key.keysym.sym) {
    case SDLK_u: // Move up (along the normal direction)
        this->getCamera()->moveRelative(normalDirection * 0.1f);
        this->isboxmovig = false;
        break;
    case SDLK_d: // Move down (opposite to the normal direction)
        this->getCamera()->moveRelative(normalDirection * -0.1f);
        this->isboxmovig = false;
        break;
    case SDLK_l: // Move left (opposite to the right direction)
        this->getCamera()->moveRelative(rightDirection * -0.1f);
        this->isboxmovig = false;
        break;
    case SDLK_r: // Move right (along the right direction)
        this->getCamera()->moveRelative(rightDirection * 0.1f);
        this->isboxmovig = false;
        break;
    case SDLK_f: // Move forward (along the look direction)
        this->getCamera()->moveRelative(lookDirection * 0.1f);
        this->isboxmovig = false;
        break;
    case SDLK_b: // Move backward (opposite to the look direction)
        this->getCamera()->moveRelative(lookDirection * -0.1f);
        this->isboxmovig = false;
        break;
    case SDLK_UP:    // Move forward
        this->isboxmovig = true;
        movementVector = Vector(0.1f, 0, 0);
        playerBox->moveRelative(movementVector);
        this->performSecondaryFrustumCulling();
        break;
    case SDLK_DOWN:  // Move backward
        this->isboxmovig = true;
        movementVector = Vector(-0.1f, 0, 0);
        playerBox->moveRelative(movementVector);
        this->performSecondaryFrustumCulling();
        break;
    case SDLK_LEFT:  // Rotate left
        this->isboxmovig = true;
        movementVector = Vector(0, -0.1f,0);
        playerBox->moveRelative(movementVector);
        this->performSecondaryFrustumCulling();
        break;
    case SDLK_RIGHT: // Rotate right
        this->isboxmovig = true;
        movementVector = Vector(0, 0.1f, 0);
        playerBox->moveRelative(movementVector);
        this->performSecondaryFrustumCulling();
        break;
    default:
        this->setNumPhysicsStepsPerRender(1); // Call base class method for other keys
        break;
    }

}
void GLViewNewModule::performSecondaryFrustumCulling()
{
    Frustum secondaryFrustum;

    // Get the box object's position, forward direction, and up direction
    VectorT<float> boxPosition = playerBox->getPosition();  // Use the playerBox as the "camera" object
    VectorT<float> boxForward = getForward(playerBox);    // Get the forward direction of the box
    VectorT<float> boxUp = getUp(playerBox);              // Get the up direction of the box

    // Create a "look at" point by adding the forward vector to the position
    VectorT<float> boxTarget = boxPosition + boxForward;

    // Create the secondary view matrix using the box's position, target, and up vector
    Mat4 secondaryViewMatrix = customLookAt(boxPosition, boxTarget, boxUp);

    // Create a standard perspective projection matrix (adjust FOV, aspect ratio, etc. as needed)
    float fov = 45.0f * (M_PI / 180.0f);  // Field of view in radians
    float aspectRatio = 1.7333f;// static_cast<float>(this->getCamera()->getViewportWidth()) / this->getCamera()->getViewportHeight();
    float nearPlane = 0.1f;
    float farPlane = 100.0f;
    Mat4 secondaryProjectionMatrix = customPerspective(fov, aspectRatio, nearPlane, farPlane);

    // Combine the secondary view and projection matrices into a view-projection matrix
    Mat4 secondaryProjViewMatrix = secondaryProjectionMatrix * secondaryViewMatrix;

    // Extract frustum planes from the view-projection matrix
    secondaryFrustum.extractPlanes(secondaryProjViewMatrix);

    // Perform frustum culling on all objects in the world list using the new frustum
    for (auto it = this->worldLst->begin(); it != this->worldLst->end(); ++it)
    {
        WO* wo = *it;
        VectorT<float> objectPosition = wo->getPosition();
        float distanceToPlayerBox = (objectPosition - boxPosition).magnitude();

        std::cout << wo->getLabel() << std::endl;
        std::cout << wo->getLabel()<< distanceToPlayerBox << std::endl;
        if ((distanceToPlayerBox > 15.0) && (wo->getLabel().compare("earth") == 1 or wo->getLabel().compare("earth1") == 1 or wo->getLabel().compare("earth2") == 1 or wo->getLabel().compare("earth3") == 1))
        {
            // Render the object since it's inside the frustum
            wo->isVisible = false;
            std::cout << wo->getLabel() << " is outside the frustum and will not be rendered.\n";


        }
        else
        {
            wo->render(*(this->getCamera()));
            wo->isVisible = true;
        }
    }
}
Mat4 GLViewNewModule::customLookAt(const VectorT<float>& eye, const VectorT<float>& target, const VectorT<float>& up) {
    // Calculate the forward (z) axis: the direction from the eye to the target
    VectorT<float> forward = (target - eye).normalizeMe();

    // Calculate the right (x) axis: the perpendicular vector to the forward and up directions
    VectorT<float> right = forward.crossProduct(up).normalizeMe();

    // Recalculate the up (y) axis: it's perpendicular to the right and forward vectors
    VectorT<float> newUp = right.crossProduct(forward).normalizeMe();

    // Create the view matrix (rotation and translation components)
    Mat4 viewMatrix;

    // Set rotation (upper-left 3x3 matrix) part
    viewMatrix[0] = right.x;
    viewMatrix[1] = newUp.x;
    viewMatrix[2] = -forward.x;
    viewMatrix[4] = right.y;
    viewMatrix[5] = newUp.y;
    viewMatrix[6] = -forward.y;
    viewMatrix[8] = right.z;
    viewMatrix[9] = newUp.z;
    viewMatrix[10] = -forward.z;

    // Set translation part (last column of the 4x4 matrix)
    viewMatrix[12] = -right.dotProduct(eye);
    viewMatrix[13] = -newUp.dotProduct(eye);
    viewMatrix[14] = forward.dotProduct(eye); // Reverse direction

    return viewMatrix;
}
Mat4 GLViewNewModule::customPerspective(float fov, float aspectRatio, float nearPlane, float farPlane) {
    Mat4 perspectiveMatrix;

    float tanHalfFov = tan(fov / 2.0f);
    float range = nearPlane - farPlane;

    // Set the perspective matrix values
    perspectiveMatrix[0] = 1.0f / (aspectRatio * tanHalfFov); // X-axis scaling (perspective)
    perspectiveMatrix[5] = 1.0f / tanHalfFov;                 // Y-axis scaling (perspective)
    perspectiveMatrix[10] = -(farPlane + nearPlane) / range;   // Z-axis (depth) scaling
    perspectiveMatrix[11] = -1.0f;                            // Perspective division
    perspectiveMatrix[14] = (2.0f * farPlane * nearPlane) / range; // Z-axis translation (depth)
    perspectiveMatrix[15] = 0.0f;                             // W-axis (homogeneous coord.)

    return perspectiveMatrix;
}

void GLViewNewModule::createNewModuleWayPoints()
{
    WayPointParametersBase params(this);
    params.frequency = 5000;
    params.useCamera = true;
    params.visible = true;
    WOWayPointSpherical* wayPt = WOWayPointSpherical::New(params, 3);
    wayPt->setPosition(Vector(50, 0, 3));
    worldLst->push_back(wayPt);
}


void GLViewNewModule::onResizeWindow(GLsizei width, GLsizei height)
{
    GLView::onResizeWindow(width, height);
}

void GLViewNewModule::onMouseDown(const SDL_MouseButtonEvent& e)
{
    GLView::onMouseDown(e);
}

void GLViewNewModule::onMouseUp(const SDL_MouseButtonEvent& e)
{
    GLView::onMouseUp(e);
}

void GLViewNewModule::onMouseMove(const SDL_MouseMotionEvent& e)
{
    GLView::onMouseMove(e);
}

void GLViewNewModule::onKeyUp(const SDL_KeyboardEvent& key)
{
    GLView::onKeyUp(key);
}

void GLViewNewModule::loadMap()
{
    this->worldLst = new WorldList();
    this->actorLst = new WorldList();
    this->netLst = new WorldList();

    ManagerOpenGLState::GL_CLIPPING_PLANE = 1000.0;
    ManagerOpenGLState::GL_NEAR_PLANE = 0.1f;
    ManagerOpenGLState::enableFrustumCulling = false;
    Axes::isVisible = true;
    this->glRenderer->isUsingShadowMapping(false);
    this->cam->setPosition(15, 15, 10);

    std::string shinyRedPlasticCube(ManagerEnvironmentConfiguration::getSMM() + "/models/cube4x4x4redShinyPlastic_pp.wrl");
    std::string grass(ManagerEnvironmentConfiguration::getSMM() + "/models/grassFloor400x400_pp.wrl");

    std::vector<std::string> skyBoxImageNames;
    skyBoxImageNames.push_back(ManagerEnvironmentConfiguration::getSMM() + "/images/skyboxes/sky_mountains+6.jpg");

    // Add skybox and light
    WO* wo = WOSkyBox::New(skyBoxImageNames.at(0), this->getCameraPtrPtr());
    wo->setPosition(Vector(0, 0, 0));
    worldLst->push_back(wo);
    WOLight* light = WOLight::New();
    light->isDirectionalLight(true);
    light->setPosition(Vector(0, 0, 100));
    worldLst->push_back(light);
    wo->getDisplayMatrix();
    // Add ground plane
    WO* ground = WO::New(grass, Vector(1, 1, 1), MESH_SHADING_TYPE::mstFLAT);
    ground->setPosition(Vector(0, 0, 0));
    worldLst->push_back(ground);

    createNewModuleWayPoints();
}
// Assuming you have a display matrix, retrieve the position of the object
VectorT<float> GLViewNewModule::getPosition(WO* obj) 
{
    Mat4 displayMatrix = obj->getDisplayMatrix();
    return VectorT<float>(displayMatrix[12], displayMatrix[13], displayMatrix[14]);
}

// Retrieve the forward vector of the object
VectorT<float> GLViewNewModule::getForward(WO* obj)
{
    Mat4 displayMatrix = obj->getDisplayMatrix();
    return VectorT<float>(displayMatrix[8], displayMatrix[9], displayMatrix[10]);
}

// Retrieve the up vector of the object
VectorT<float> GLViewNewModule::getUp(WO* obj)
{
    Mat4 displayMatrix = obj->getDisplayMatrix();
    return VectorT<float>(displayMatrix[4], displayMatrix[5], displayMatrix[6]);
}

VectorT<float> GLViewNewModule::getRight(WO* obj)
{
    Mat4 displayMatrix = obj->getDisplayMatrix();
    return VectorT<float>(displayMatrix[0], displayMatrix[1], displayMatrix[2]);
}



