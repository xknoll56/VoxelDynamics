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

    void VDCollisionBoxImplicitPlaneEdgeTest(const VDEdge& edge, const VDImplicitPlane& plane, VDManifold& manifold)
    {
        VDDirection dir1 = plane.closestEdgeDirectionFromPoint(edge.pointFrom);
        VDVector3 dir1Vec = VDDirectionToFrameVector(dir1, plane.frame);
        VDDirection dir2 = plane.closestEdgeDirectionFromPoint(edge.pointTo);
        VDVector3 dir2Vec = VDDirectionToFrameVector(dir2, plane.frame);
        VDEdge gap1;
        VDEdge gap2;
        if (edge.closestEdgeToEdgeNoClamp(plane.getEdgeByDirection(dir1), gap1))
        {
            if(VDDot(gap1.dir, plane.frame.up) >= 0.0f  && VDDot(gap1.dir, dir1Vec) >= 0.0f)
                drawEdge(gap1, colorRed);
        }
        if (dir1!=dir2 && edge.closestEdgeToEdgeNoClamp(plane.getEdgeByDirection(dir2), gap2))
        {
            if (VDDot(gap2.dir, plane.frame.up) >= 0.0f && VDDot(gap2.dir, dir2Vec) >= 0.0f)
                drawEdge(gap2, colorRed);
        }
    }

    bool VDCollisionBoxImplicitPlane(const VDOBB& box, const VDImplicitPlane& plane, VDManifold& manifold)
    {
        VDDirection closestFaceDirection = VDVectorToFrameDirection(-plane.frame.up, box.frame);
        VDImplicitPlane face = box.directionToImplicitPlane(closestFaceDirection);
        VDVector3 faceVerts[4];
        face.extractVerts(faceVerts);
        for (int i = 0; i < 4; i++)
        {
            VDContactInfo ci;
            if (VDRayCastImplicitPlane(faceVerts[i], plane.frame.up, plane, ci))
            {
                manifold.insertContact(ci);
                drawTranslatedBox(ci.point, colorBlue, VDVector3::uniformScale(0.1f));
            }
        }

        VDVector3 dp = plane.center - box.position;
        VDVector3 inward = VDTangentialComponent(dp, plane.frame.up);
        VDDirection inwardDir = VDVectorToFrameDirection(inward, box.frame);
        VDImplicitPlane inwardFace = box.directionToImplicitPlane(inwardDir);

        VDDirection edgeDirs[] = { VDDirection::RIGHT, VDDirection::LEFT, VDDirection::FORWARD, VDDirection::BACK };
        if (VDDot(face.frame.up, inward) >= 0.0f)
        {
            for (int i = 0; i < 4; i++)
            {
                VDEdge edge = face.getEdgeByDirection(edgeDirs[i]);
                VDCollisionBoxImplicitPlaneEdgeTest(edge, plane, manifold);
            }
        }
        else
        {
            for (int i = 0; i < 4; i++)
            {
                VDEdge edge = inwardFace.getEdgeByDirection(edgeDirs[i]);
                VDCollisionBoxImplicitPlaneEdgeTest(edge, plane, manifold);
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