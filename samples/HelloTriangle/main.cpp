#include "Utils.h"
#include "Application.h"

struct HelloTriangle : Scene
{
    VDSimulation sim;
    VDTriangle triangle;
    VDAABB aabb;

    void init() override
    {
        triangle = VDTriangle({ 0,0,0 }, { 3,0,0 }, { 0,1,3 });
        aabb = VDAABB();
    }

    void update(float dt) override
    {
        Scene::update(dt);
        movePositionWithArrows(camera, aabb.position, dt, 3.0f);
        aabb.setPosition(aabb.position);
    }

    bool VDCollisionAABBEdge(const VDAABB& aabb, const VDEdge& edge, VDManifold& manifold)
    {
        VDImplicitPlane contactPlane;
        VDContactInfo info;
        if (VDRayCastAABB(edge.pointFrom, edge.dir, aabb, info, contactPlane))
        {
            if (info.type == VDContactType::FACE)
            {
                VDEdge closestEdge = contactPlane.closestEdgeToPoint(info.point);
                VDEdge penetrationEdge = closestEdge.closestEdgeToEdge(edge);
                manifold.insertEdgeContact(penetrationEdge);
            }
        }

        if (VDRayCastAABB(edge.pointTo, -edge.dir, aabb, info, contactPlane))
        {
            if (info.type == VDContactType::FACE)
            {
                VDEdge closestEdge = contactPlane.closestEdgeToPoint(info.point);
                VDEdge penetrationEdge = closestEdge.closestEdgeToEdge(edge);
                manifold.insertEdgeContact(penetrationEdge);
            }
        }
        return true;
    }

    bool VDCollisionAABBTriangle(const VDAABB& aabb, const VDTriangle& triangle, VDManifold& manifold)
    {
        VDAABB triangleAABB = triangle.toAABB();
        if (aabb.isIntersecting(triangleAABB))
        {
            VDVector3 dp = aabb.position - triangle.vertices[0];
            float dot = VDDot(dp, triangle.normal);
            VDVector3 normal = triangle.normal;
            if (dot > 0.0f)
                normal = -normal;

            VDDirection normalDir = VDVectorToDirection(normal);
            VDImplicitPlane closestPlane = aabb.directionToImplicitPlane(normalDir);
            VDContactInfo rayInfo;
            for (int i = 0; i < 4; i++)
            {
                if (VDRayCastTriangle(closestPlane.getVertexByIndex(i), -closestPlane.normal(), triangle, rayInfo))
                {
                    manifold.insertContact(rayInfo);
                }
            }

            VDEdge triangleEdge = triangle.getEdgeByIndex(0);
            if (VDRayCastAABB(triangleEdge.pointFrom, triangleEdge.dir, aabb, rayInfo))
            {
                return true;
            }
            triangleEdge = triangle.getEdgeByIndex(1);
            if (VDRayCastAABB(triangleEdge.pointFrom, triangleEdge.dir, aabb, rayInfo))
            {
                return true;
            }
            triangleEdge = triangle.getEdgeByIndex(2);
            if (VDRayCastAABB(triangleEdge.pointFrom, triangleEdge.dir, aabb, rayInfo))
            {
                return true;
            }
        }

        return false;
    }


    void draw(float dt) override
    {
        //drawTranslatedBox(VDVector3(2,2,2), {1,1,1});
        drawTriangle(triangle, VDVector3::right());
        drawAABB(aabb, colorCyan);

        drawEdge(triangle.getEdgeByIndex(1), colorRed);
        VDManifold manifold;
        if (VDCollisionAABBEdge(aabb, triangle.getEdgeByIndex(1), manifold))
        {
            for (int i = 0; i < manifold.count; i++)
            {
                drawTranslatedBox(manifold.infos[i].point, colorGreen, VDVector3::uniformScale(0.25f));
            }
        }
        //VDManifold manifold;
        //if(VDCollisionAABBTriangle(aabb, triangle, manifold))
        //    drawAABB(triangle.toAABB(), colorGreen);
        //else
        //    drawAABB(triangle.toAABB(), colorRed);
        //for (int i = 0; i < manifold.count; i++)
        //{
        //    drawTranslatedBox(manifold.infos[i].point, colorGreen, VDVector3::uniformScale(0.24f));
        //}
    }
};


int main(void)
{
    initApplication();
    HelloTriangle scene;
    runApplication(&scene);
    return 0;
}