#include "Utils.h"
#include "Application.h"

struct HellBoxScene : Scene
{

    VDQuaternion rotation;
    void init() override
    {

    }

    void update(float dt) override
    {
        Scene::update(dt);
        rotation.rotate(VDQuaternion::fromEulerAngles(VDVector3(0, dt, 0)));
    }

    void draw(float dt) override
    {
        drawBox(VDVector3(), rotation, VDVector3::one(), VDVector3::one());
        drawBox(VDVector3(), rotation, VDVector3::one(), VDVector3(0,1,0), GL_LINES);
    }
};


int main(void)
{
    initApplication();
    HellBoxScene scene;
    runApplication(&scene);
    return 0;
}