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
            switch (ci.type)
            {
            case FACE:
            {
                VDVector3 axis = VDCross(VDVector3::up(), velocity);
                VDQuaternion w = VDQuaternion::fromAngleAxis(axis, dt * velocity.length());
                box.setPosition(w.rotateAround(ci.point, box.position));
                box.rotate(w);
                box.translate(ci.normal * ci.distance);
            }
            break;
            case EDGE:
            {
                drawEdge(ci.toEdge(), colorRed, 0.05f);
            }
            break;
            }
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