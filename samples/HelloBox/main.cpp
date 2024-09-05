#include "Utils.h"
#include "Application.h"

struct HellBoxScene : Scene
{

    VDOBB box;
    void init() override
    {
        box.setHalfExtents(VDVector3(0.5, 1, 1.5));
        box.setPosition({ 4,4,4 });
    }

    void update(float dt) override
    {
        Scene::update(dt);
        box.rotate(VDQuaternion::fromEulerAngles(VDVector3(dt * 0.1, dt, dt * 0.7)));
  
        box.setLowAndHigh();
        box.setVertices();
    }

    void draw(float dt) override
    {
        drawBox(box, colorWhite);
        drawBox(box, colorCyan, false);
        drawAABB(box, colorGreen);
        drawBoxFrame(box, 3.0f);

        VDContactPoint point;
        if (VDRayCastPlane(VDVector3(0, 10, 0), VDNormalize(VDVector3(cosf(elapsedTime)*0.5f, -1, sinf(elapsedTime)*0.5f)),
            VDVector3::up(), VDVector3(), point))
        {
            drawLine(VDVector3(0, 10, 0), point.point, colorGreen);
        }
    }
};


int main(void)
{
    initApplication();
    HellBoxScene scene;
    runApplication(&scene);
    return 0;
}