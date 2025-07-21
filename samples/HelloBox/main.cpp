#include "Utils.h"
#include "Application.h"

struct HellBoxScene : Scene
{


    VDOBB box;
    VDEdge edge;
    float t;

    void init() override
    {
        box.setHalfExtents(VDVector3(0.5, 1, 1.5));
        edge = VDEdge(VDVector3(-0.5,-0.5,-0.5), VDVector3(2, 1, 2));
        t = 0.0f; 
    }

    void update(float dt) override
    {
        Scene::update(dt);
        if (keys[GLFW_KEY_E])
        {
            t += dt;
        }
		box.setRotation(VDQuaternion::fromAngleAxis(VDVector3(0, 1, 0), t));

		VDManifold manifold;
        VDCollisionBoxEdge(box, edge, manifold);
        for (VDuint i = 0; i < manifold.count; i++)
        {
            const VDContactInfo& info = manifold.infos[i];
            if (info.type == VDContactType::EDGE)
            {
                drawEdge(VDEdge(info.point, info.point + info.normal * info.distance), colorMagenta);
                drawPoint(info.point, colorBlue);
            }
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


    }

    void draw(float dt) override
    {
       // drawBox(box, VDVector3(1,1,10.2));
        drawBox(box, colorCyan, false);
        drawEdge(edge, colorRed);
    }
};


int main(void)
{
    initApplication();
    HellBoxScene scene;
    runApplication(&scene);
    return 0;
}