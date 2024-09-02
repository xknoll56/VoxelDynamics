#include "Utils.h"
#include "Application.h"

struct HellBoxScene : Scene
{

    void init() override
    {

    }

    void update(float dt) override
    {
        Scene::update(dt);

    }

    void draw(float dt) override
    {
        drawBox(VDVector3(), VDVector3(0, elapsedTime, 0), VDVector3::one(), VDVector3::one());
        drawBox(VDVector3(), VDVector3(0, elapsedTime, 0), VDVector3::one(), VDVector3(0,1,0), GL_LINES);
        
    }
};


int main(void)
{
    initApplication();
    HellBoxScene scene;
    runApplication(&scene);
    return 0;
}