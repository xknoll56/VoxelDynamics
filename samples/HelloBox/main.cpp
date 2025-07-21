#include "Utils.h"
#include "Application.h"

struct HellBoxScene : Scene
{


    VDOBB box;
    VDEdge edge;
    float t;
    VDImplicitPlane plane;

    void init() override
    {
        box.setHalfExtents(VDVector3(0.5, 1, 1.5));
        box.translate(VDVector3(0, 0.5, 0));
        edge = VDEdge(VDVector3(-0.5,-0.5,-0.5), VDVector3(2, 1, 2));
        t = 0.0f; 
		plane = VDImplicitPlane(VDVector3(0, 0, 0), VDVector3(0, 1, 0), 5.0f, 5.0f, 0.5f);
		//plane = VDImplicitPlane(VDVector3(0, 0, 0), VDVector3(0, 1, 0), 0.25f, 0.25f, 0.5f);
    }

    void update(float dt) override
    {
        Scene::update(dt);
        if (keys[GLFW_KEY_E])
        {
            t += dt;
        }
		movePositionWithArrows(camera, box.position, dt, 2.0f);
		box.setRotation(VDQuaternion::fromAngleAxis(VDVector3(1, 1, 1), t));

		VDManifold manifold;
        if(VDCollisionBoxImplicitPlane(box, plane, manifold))
        {
            for (VDuint i = 0; i < manifold.count; i++)
            {
                const VDContactInfo& info = manifold.infos[i];
                drawEdge(VDEdge(info.point, info.point + info.normal * info.distance), colorMagenta);
                drawPoint(info.point, colorBlue);
            }
        }
        else
        {
            std::cout << "Box did not collide with plane" << std::endl;
		}

        if (keysDown[GLFW_KEY_SPACE])
        {
            if(manifold.count > 0)
            {
                const VDContactInfo& info = manifold.infos[0];
                box.translate(info.normal * info.distance);
				std::cout << "Box moved by " << info.distance << " in direction " << info.normal.x <<"," <<info.normal.y << "," << info.normal.z << std::endl;
			}
        }

        //VDVector3 dp = plane.center - box.position;
        //VDVector3 inward = plane.frame.up;
        //if (VDDot(dp, inward) < 0.0f)
        //{
        //    inward = -inward; // Ensure inward direction is correct
        //}

        //VDDirection boxInwardDir = VDVectorToFrameDirection(inward, box.frame);
        //VDImplicitPlane inwawrdPlane = box.directionToImplicitPlane(boxInwardDir);
        VDDirection closestFaceDirection = VDVectorToFrameDirection(-plane.frame.up, box.frame);
        VDVector3 closestFaceVector = VDDirectionToFrameVector(closestFaceDirection, box.frame);
        VDImplicitPlane face = box.directionToImplicitPlane(closestFaceDirection);
		drawPoint(face.center, colorGreen);

    }

    void draw(float dt) override
    {
        drawBox(box, colorCyan, false);
        drawEdge(edge, colorRed);
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