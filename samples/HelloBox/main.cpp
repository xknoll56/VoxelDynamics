#include "Utils.h"
#include "Application.h"

struct HellBoxScene : Scene
{


    VDOBB box;
    VDImplicitPlane plane;
    VDVector3 rayCastStart;
    bool rotate;
    void init() override
    {
        box.setHalfExtents(VDVector3(0.5, 1, 1.5));
        plane = VDImplicitPlane(VDVector3(), VDVector3(0, 1, 0), 10.0f, 10.0f, 0.0f);
        rayCastStart = VDVector3(3, 5, 2);
        rotate = true;
    }

    void update(float dt) override
    {
        Scene::update(dt);
        movePositionWithArrows(camera, rayCastStart, dt, 2.0f);
        box.setPosition(rayCastStart);

        if (keysDown[GLFW_KEY_SPACE])
        {
            rotate = !rotate;
        }
        if (rotate)
        {
            box.rotate(VDQuaternion::fromEulerAngles(VDVector3(dt * 0.1, dt, dt * 0.7)));
            box.setLowAndHigh();
            box.setVertices();
        }

        VDManifold manifold;
        VDCollisionBoxImplicitPlane(box, plane, manifold);
    }

    bool VDCollisionBoxImplicitPlane(const VDOBB& box, const VDImplicitPlane& plane, VDManifold& manifold)
    {
        VDDirection closestFaceDirection = VDVectorToFrameDirection(-plane.frame.up, box.frame);
        VDImplicitPlane face = box.directionToImplicitPlane(closestFaceDirection);
        drawImplicitPlane(face, colorRed);
        VDVector3 faceVerts[4];
        face.extractVerts(faceVerts);
        for (int i = 0; i < 4; i++)
        {
            VDContactInfo ci;
            if (VDRayCastImplicitPlane(faceVerts[i], plane.frame.up, plane, ci))
            {
                manifold.insertContact(ci);
            }
        }

        return true;
    }

    void draw(float dt) override
    {
        drawBox(box, colorWhite);
        drawBox(box, colorCyan, false);
        drawAABB(box, colorGreen);
        drawBoxFrame(box, 3.0f);

        drawImplicitPlane(plane, colorWhite);

    }
};


int main(void)
{
    initApplication();
    HellBoxScene scene;
    runApplication(&scene);
    return 0;
}