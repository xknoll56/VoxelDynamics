#include "Utils.h"
#include "Application.h"

struct HellBoxScene : Scene
{


    VDOBB box;
    VDImplicitPlane plane;
    VDVector3 boxPosition;
    bool rotate;
    void init() override
    {
        box.setHalfExtents(VDVector3(0.5, 1, 1.5));
        plane = VDImplicitPlane(VDVector3(), VDVector3(0, 1, 0), 10.0f, 10.0f, 0.0f);
        boxPosition = VDVector3(0, 0.99, 0);
        rotate = false;
        rotPoint = VDVector3(0, 2, 0);
    }

    void moveBoxAlongPlane(const VDImplicitPlane& plane, VDOBB& box, VDVector3 velocity, float dt)
    {
        VDManifold manifold;
        VDCollisionBoxImplicitPlane(box, plane, manifold);

        if (manifold.count > 0)
        {
            VDContactInfo ci = manifold.infos[manifold.deepestPenetrationIndex];
            VDVector3 axis = VDCross(VDVector3::up(), velocity);
            VDQuaternion w = VDQuaternion::fromAngleAxis(axis, dt * velocity.length());
            box.setPosition(w.rotateAround(ci.point, box.position));
            box.rotate(w);
            box.translate(ci.normal * ci.distance);
        }
    }

    VDVector3 rotPoint;

    void update(float dt) override
    {
        Scene::update(dt);
        movePositionWithArrows(camera, boxPosition, dt, 2.0f);
        //box.setPosition(boxPosition);

        if (keysDown[GLFW_KEY_SPACE])
        {
            rotate = !rotate;
        }
        if (rotate)
        {
            //box.rotate(VDQuaternion::fromEulerAngles(VDVector3(dt * 0.1, dt, dt * 0.7)));
            //box.setLowAndHigh();
            //box.setVertices();
            VDVector3 velVec = VDNormalize(VDVector3(1, 0, 1));
            moveBoxAlongPlane(plane, box, velVec, dt);
        }


        //box.usingVertices();
        //VDQuaternion w = VDQuaternion::fromAngleAxis(VDNormalize(VDVector3(0, 1, 1)), dt );
        //VDVector3 newPos = w.rotateAround(box.vertices[4], box.position);
        //box.rotate(w);
        //box.setPosition(newPos);


        //box.rotate(w);
        //box.setPosition(newPos);
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
            if (VDDot(gap1.dir, plane.frame.up) >= 0.0f && VDDot(gap1.dir, dir1Vec) >= 0.0f)
            {
                if(box.isPointInOBB(gap1.pointTo))
                    manifold.insertEdgeContact(gap1);
            }
        }
        if (dir1!=dir2 && edge.closestEdgeToEdgeNoClamp(plane.getEdgeByDirection(dir2), gap2))
        {
            if (VDDot(gap2.dir, plane.frame.up) >= 0.0f && VDDot(gap2.dir, dir2Vec) >= 0.0f)
            {
                if (box.isPointInOBB(gap2.pointTo))
                    manifold.insertEdgeContact(gap2);
            }
        }
    }

    bool VDCollisionBoxImplicitPlane(const VDOBB& box, const VDImplicitPlane& plane, VDManifold& manifold, float skinWidth = 0.005f)
    {
        VDDirection closestFaceDirection = VDVectorToFrameDirection(-plane.frame.up, box.frame);
        VDVector3 closestFaceVector = VDDirectionToFrameVector(closestFaceDirection, box.frame);
        VDImplicitPlane face = box.directionToImplicitPlane(closestFaceDirection);
        VDVector3 faceVerts[4];
        face.extractVerts(faceVerts);
        for (int i = 0; i < 4; i++)
        {
            VDContactInfo ci;
            if (VDRayCastImplicitPlane(faceVerts[i]+ closestFaceVector*(-skinWidth), plane.frame.up, plane, ci))
            {
                ci.normal = -ci.normal;
                ci.distance -= skinWidth;
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
       /// drawBoxFrame(box, 3.0f);

        drawImplicitPlane(plane, colorWhite);
        //drawTranslatedBox(box.vertices[4], colorGreen, VDVector3::uniformScale(0.1f));
    }
};


int main(void)
{
    initApplication();
    HellBoxScene scene;
    runApplication(&scene);
    return 0;
}