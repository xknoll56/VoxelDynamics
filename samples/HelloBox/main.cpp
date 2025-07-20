#include "Utils.h"
#include "Application.h"

struct HellBoxScene : Scene
{


    VDOBB box;
    VDEdge edge;

    void init() override
    {
        box.setHalfExtents(VDVector3(0.5, 1, 1.5));
		edge = VDEdge(VDVector3(-2,-1, -2), VDVector3(2, 2, 2));
    }

    void update(float dt) override
    {
        Scene::update(dt);
        if(keys[GLFW_KEY_SPACE])
		    box.setRotation(VDQuaternion::fromAngleAxis(VDVector3(0, 1, 0), elapsedTime));

   //     VDContactInfo info;
   //     VDImplicitPlane contactPlane;
   //     if(VDRayCastOBB(edge.pointFrom, edge.dir, box, info, contactPlane))
   //     {
			//drawPoint(info.point, colorYellow);
			//drawPoint(contactPlane.center, colorBlue);
   //         VDEdge closest = contactPlane.closestEdgeToPoint(info.point);
			//drawEdge(closest, colorGreen);
   //         VDEdge edgeGap;
   //         if(edge.closestEdgeToEdgeNoClamp(closest, edgeGap))
   //         {
   //             drawPoint(edgeGap.pointFrom, colorBlue);
   //             drawPoint(edgeGap.pointTo, colorGreen);
   //             if(box.isPointInOBB(edgeGap.pointFrom))
   //                 drawEdge(edgeGap, colorMagenta);
			//}
   //     }
   //     if (VDRayCastOBB(edge.pointTo, -edge.dir, box, info, contactPlane))
   //     {
   //         drawPoint(info.point, colorYellow);
   //         drawPoint(contactPlane.center, colorBlue);
   //         VDEdge closest = contactPlane.closestEdgeToPoint(info.point);
   //         drawEdge(closest, colorGreen);
   //         VDEdge edgeGap;
   //         if (edge.closestEdgeToEdgeNoClamp(closest, edgeGap))
   //         {
   //             drawPoint(edgeGap.pointFrom, colorBlue);
   //             drawPoint(edgeGap.pointTo, colorGreen);
   //             if (box.isPointInOBB(edgeGap.pointFrom))
   //                 drawEdge(edgeGap, colorMagenta);
   //         }
   //     }
        VDEdge edges[12];
        box.getEdges(edges);
		VDEdge smallestEdge = edges[0];
        smallestEdge.distance = FLT_MAX;
        for (int i = 0; i < 12; i++)
        {
            VDEdge& e = edges[i];
            VDEdge edgeGap;
            if (e.closestEdgeToEdgeNoClamp(edge, edgeGap))
            {
                if (box.isPointInOBB(edgeGap.pointTo) && edgeGap!=e)
                {
                    if(edgeGap.distance < smallestEdge.distance)
                    {
                        smallestEdge = edgeGap;
					}
                }
            }
		}
        if (smallestEdge.distance > 0.0f && smallestEdge.distance != FLT_MAX)
        {
            drawEdge(smallestEdge, colorMagenta);
            drawPoint(smallestEdge.pointFrom, colorBlue);
            drawPoint(smallestEdge.pointTo, colorGreen);
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