#ifndef VOXEL_DYNAMICS_COLLISION_DETECTION
#define VOXEL_DYNAMICS_COLLISION_DETECTION

#include "VoxelDynamicsCollider.h"
#include <cfloat>

struct VDPenetrationField
{
	float maxPenetrations[6];
	VDPointer pVoxels[6];

	VDPenetrationField()
	{
		memset(maxPenetrations, 0.0f, 6 * sizeof(float));
		for(int i = 0; i<6; i++)
			pVoxels[i] = NULL;
	}

	~VDPenetrationField()
	{

	}

	void insertPenetration(VDDirection dir, float magnitude, VDPointer pVoxel)
	{
		if (magnitude > maxPenetrations[dir])
		{
			maxPenetrations[dir] = magnitude;
			pVoxels[dir] = pVoxel;
		}
	}
};

enum VDContactType
{
	FACE = 0,
	EDGE,
	POINT,
	INTERNAL
};

struct VDContactInfo
{
	VDVector3 normal;
	float distance;
	VDVector3 point;
	VDContactType type;
	VDDirection dir;

	VDContactInfo()
	{
		point = VDVector3();
		normal = VDVector3();
		distance = 0.0;
	}

	VDContactInfo(VDVector3 _point, VDVector3 _normal, float _distance, VDContactType _type)
	{
		normal = _normal;
		distance = _distance;
		point = _point;
		type = _type;
	}

	VDEdge toEdge()
	{
		return VDEdge(point, normal, distance);
	}
};



struct VDAABBContact
{
	VDPointer pContact;
	VDPointer pOtherContact;
	VDAABB intersectionRegion;
	VDVector3 quadrantDir;
	VDDirection minDirections[3];

	VDAABBContact()
	{
	}

	VDAABBContact(VDPointer _pContact, VDPointer _pOtherContact) :
		pContact(_pContact), pOtherContact(_pOtherContact)
	{
	}

	void setParameters(VDPointer _pContact, VDPointer _pOtherContact, VDAABB _intersectionRegion, VDVector3 _quadrantDir)
	{
		pContact = _pContact;
		pOtherContact = _pOtherContact;
		intersectionRegion = _intersectionRegion;
		quadrantDir = _quadrantDir;
	}

	VDAABBContact(VDPointer _pContact, VDPointer _pOtherContact, VDAABB _intersectionRegion, VDVector3 _quadrantDir)
	{
		setParameters(_pContact, _pOtherContact, _intersectionRegion, _quadrantDir);
	}

	VDAABBContact& operator=(const VDAABBContact& other)
	{
		if (this != &other)
		{
			pContact = other.pContact;
			pOtherContact = other.pContact;
			intersectionRegion = other.intersectionRegion;
			quadrantDir = other.quadrantDir;
			memcpy(minDirections, other.minDirections, 3 * sizeof(VDDirection));
		}
		return *this;
	}

	float getPenetrationByDirection(VDDirection dir) const
	{
		switch (dir)
		{
		case VDDirection::RIGHT:
			return 2.0f * intersectionRegion.halfExtents.x;
			break;
		case VDDirection::LEFT:
			return 2.0f * intersectionRegion.halfExtents.x;
			break;
		case VDDirection::UP:
			return 2.0f * intersectionRegion.halfExtents.y;
			break;
		case VDDirection::DOWN:
			return 2.0f * intersectionRegion.halfExtents.y;
			break;
		case VDDirection::FORWARD:
			return 2.0f * intersectionRegion.halfExtents.z;
			break;
		case VDDirection::BACK:
			return 2.0f * intersectionRegion.halfExtents.z;
			break;
		}
	}

	void setPenetrations()
	{
		if (intersectionRegion.halfExtents.x <= intersectionRegion.halfExtents.y
			&& intersectionRegion.halfExtents.x <= intersectionRegion.halfExtents.z)
		{
			if (quadrantDir.x > 0)
				minDirections[0] = VDDirection::RIGHT;
			else
				minDirections[0] = VDDirection::LEFT;
			if (intersectionRegion.halfExtents.y <= intersectionRegion.halfExtents.z)
			{
				if (quadrantDir.y > 0)
					minDirections[1] = VDDirection::UP;
				else
					minDirections[1] = VDDirection::DOWN;
				if (quadrantDir.z > 0)
					minDirections[2] = VDDirection::FORWARD;
				else
					minDirections[2] = VDDirection::BACK;
			}
			else
			{
				if (quadrantDir.y > 0)
					minDirections[2] = VDDirection::UP;
				else
					minDirections[2] = VDDirection::DOWN;
				if (quadrantDir.z > 0)
					minDirections[1] = VDDirection::FORWARD;
				else
					minDirections[1] = VDDirection::BACK;
			}
		}
		else if (intersectionRegion.halfExtents.y <= intersectionRegion.halfExtents.z)
		{
			if (quadrantDir.y > 0)
				minDirections[0] = VDDirection::UP;
			else
				minDirections[0] = VDDirection::DOWN;
			if (intersectionRegion.halfExtents.x <= intersectionRegion.halfExtents.z)
			{
				if (quadrantDir.x > 0)
					minDirections[1] = VDDirection::RIGHT;
				else
					minDirections[1] = VDDirection::LEFT;
				if (quadrantDir.z > 0)
					minDirections[2] = VDDirection::FORWARD;
				else
					minDirections[2] = VDDirection::BACK;
			}
			else
			{
				if (quadrantDir.x > 0)
					minDirections[2] = VDDirection::RIGHT;
				else
					minDirections[2] = VDDirection::LEFT;
				if (quadrantDir.z > 0)
					minDirections[1] = VDDirection::FORWARD;
				else
					minDirections[1] = VDDirection::BACK;
			}

		}
		else
		{
			if (quadrantDir.z > 0)
				minDirections[0] = VDDirection::FORWARD;
			else
				minDirections[0] = VDDirection::BACK;
			if (intersectionRegion.halfExtents.x <= intersectionRegion.halfExtents.y)
			{
				if (quadrantDir.x > 0)
					minDirections[1] = VDDirection::RIGHT;
				else
					minDirections[1] = VDDirection::LEFT;
				if (quadrantDir.y > 0)
					minDirections[2] = VDDirection::UP;
				else
					minDirections[2] = VDDirection::DOWN;
			}
			else
			{
				if (quadrantDir.x > 0)
					minDirections[2] = VDDirection::RIGHT;
				else
					minDirections[2] = VDDirection::LEFT;
				if (quadrantDir.y > 0)
					minDirections[1] = VDDirection::UP;
				else
					minDirections[1] = VDDirection::DOWN;
			}
		}
	}
};

struct VDManifold
{
	VDPointer other;
	VDContactInfo infos[8];
	VDuint deepestPenetrationIndex;
	VDuint count;
	float deepestPenetration;

	VDManifold()
	{
		other = NULL;
		deepestPenetrationIndex = 0;
		count = 0;
		deepestPenetration = 0.0f;
	}

	// Copy constructor
	VDManifold(const VDManifold& otherManifold)
	{
		other = otherManifold.other;
		deepestPenetrationIndex = otherManifold.deepestPenetrationIndex;
		count = otherManifold.count;
		deepestPenetration = otherManifold.deepestPenetration;
		for (VDuint i = 0; i < 8; ++i)
		{
			infos[i] = otherManifold.infos[i];
		}
	}

	// Assignment operator
	VDManifold& operator=(const VDManifold& otherManifold)
	{
		if (this != &otherManifold)
		{
			other = otherManifold.other;
			deepestPenetrationIndex = otherManifold.deepestPenetrationIndex;
			count = otherManifold.count;
			deepestPenetration = otherManifold.deepestPenetration;
			for (VDuint i = 0; i < 8; ++i)
			{
				infos[i] = otherManifold.infos[i];
			}
		}
		return *this;
	}

	void insertContact(const VDContactInfo info)
	{
		if (count < 8)
		{
			if (info.distance > deepestPenetration)
			{
				deepestPenetrationIndex = count;
				deepestPenetration = info.distance;
			}
			infos[count++] = info;
		}
	}

	void insertEdgeContact(const VDEdge edge)
	{
		insertContact(VDContactInfo(edge.pointFrom, edge.dir, edge.distance, VDContactType::EDGE));
	}

	void combine(const VDManifold& other)
	{
		for (int i = 0; i < other.count; i++)
		{
			insertContact(other.infos[i]);
		}
	}

	void combineOrSwapIfSimilar(const VDManifold& other, float similarityFactor = 0.95f)
	{

		// If one manifold is empty and another is not, use the non-empty manifold
		if(count == 0 && other.count > 0)
		{
			*this = other;
			return;
		}
		else if(other.count == 0 && count > 0)
		{
			return;
		}

		VDContactInfo deepestContact = infos[deepestPenetrationIndex];
		VDContactInfo otherDeepestContact = other.infos[other.deepestPenetrationIndex];

		float dotProduct = VDDot(deepestContact.normal, otherDeepestContact.normal);
		if(dotProduct > similarityFactor)
		{
			// If the normals are similar, combine the manifolds
			combine(other);
		}
		else if(otherDeepestContact.distance < deepestContact.distance)
		{
			// If the other manifold's deepest contact is deeper, replace this manifold because they are not similar
			*this = other;
		}
	}


	VDContactInfo& contactClosestToDirection(VDVector3 dir, VDVector3 position)
	{
		float maxDistance = -FLT_MAX;
		VDuint maxIndex = 0;
		for (VDuint i = 0; i < count; i++)
		{
			VDVector3 localPosition = infos[i].point - position;
			float distance = VDDot(dir, localPosition);
			if (distance > maxDistance)
			{
				maxDistance = distance;
				maxIndex = i;
			}
		}

		return infos[maxIndex];
	}
};

bool VDAABB::collisionAABB(const VDAABB* pOther, VDAABBContact& contact)
{
	VDAABB region;
	if (intersectionRegion(*pOther, region))
	{
		VDVector3 quadrantDir = VDSign(position - pOther->position);
		contact.setParameters((VDPointer)this, (VDPointer)pOther, region, quadrantDir);
		contact.setPenetrations();
		return true;
	}
	return false;
}


void VDAABB::resolveAABBContact(const VDAABBContact& contact)
{
	float penetration = contact.getPenetrationByDirection(contact.minDirections[0]);
	translate(VDDirectionToVector((VDDirection)contact.minDirections[0]) * penetration);
}


bool VDRayCastPlane(VDVector3 from, VDVector3 dir, VDVector3 planeNormal, VDVector3 pointOnPlane, VDContactInfo& contactPoint)
{
	VDVector3 dp = pointOnPlane - from;
	contactPoint.normal = planeNormal;
	float dpDotNormal = VDDot(dp, contactPoint.normal);
	if (VDAbs(dpDotNormal) < VD_COLLIDER_TOLERANCE)
	{
		contactPoint.normal = planeNormal;
		contactPoint.distance = 0.0f;
		contactPoint.point = from;
		return true;
	}
	float dirDotNormal = VDDot(dir, planeNormal);
	contactPoint.distance = dpDotNormal / dirDotNormal;
	contactPoint.point = from + dir*contactPoint.distance;
	if (dpDotNormal > 0.0f)
	{
		contactPoint.normal = planeNormal * -1.0f;
	}
	contactPoint.type = FACE;
	return contactPoint.distance > 0.0f;
}

bool VDRayCastImplicitPlaneContained(const VDImplicitPlane& plane, VDContactInfo& contactInfo)
{
	VDVector3 dp = contactInfo.point - plane.center;
	float rightDist = VDDot(plane.frame.right, dp);
	float forwardDist = VDDot(plane.frame.forward, dp);
	if (VDAbs(rightDist) <= plane.rightHalfSize && VDAbs(forwardDist) <= plane.forwardHalfSize)
	{
		return true;
	}
	return false;
}

bool VDRayCastTriangleContained(const VDTriangle& triangle, VDContactInfo& contactInfo)
{
	const VDVector3& p = contactInfo.point;
	const VDVector3& v0 = triangle.vertices[0];
	const VDVector3& v1 = triangle.vertices[1];
	const VDVector3& v2 = triangle.vertices[2];
	const VDVector3& n = triangle.normal;


	// Step 1: Check if the point is inside the triangle using edge tests
	VDVector3 edge0 = v1 - v0;
	VDVector3 edge1 = v2 - v1;
	VDVector3 edge2 = v0 - v2;

	VDVector3 c0 = p - v0;
	VDVector3 c1 = p - v1;
	VDVector3 c2 = p - v2;

	// Cross products for the edge tests
	VDVector3 cross0 = VDCross(edge0, c0);
	VDVector3 cross1 = VDCross(edge1, c1);
	VDVector3 cross2 = VDCross(edge2, c2);

	// Check if all cross products are in the same direction as the triangle normal
	if ((VDSign(VDDot(n, cross0))==VDSign(VDDot(n, cross1))) && (VDSign(VDDot(n, cross0)) == VDSign(VDDot(n, cross2))))
		return true;

	return false;
}

bool VDRayCastImplicitPlane(VDVector3 from, VDVector3 dir, const VDImplicitPlane& plane, VDContactInfo& contactInfo)
{
	if (VDRayCastPlane(from, dir, plane.frame.up, plane.center, contactInfo))
	{
		return VDRayCastImplicitPlaneContained(plane, contactInfo);
	}
	return false;
}

bool VDRayCastTriangle(VDVector3 from, VDVector3 dir, const VDTriangle& triangle, VDContactInfo& contactInfo)
{
	if (VDRayCastPlane(from, dir, triangle.normal, triangle.vertices[0], contactInfo))
	{
		return VDRayCastTriangleContained(triangle, contactInfo);
	}
	return false;
}

bool VDRayCastAABB(VDVector3 from, VDVector3 dir, const VDAABB& aabb, VDContactInfo& contactInfo)
{
	if (aabb.isPointInAABB(from))
	{
		contactInfo = VDContactInfo(from, dir, 0.0f, INTERNAL);
		return true;
	}
	VDVector3 dp = aabb.position - from;
	float dot = VDDot(dp, dir);
	if (dot >= 0.0f)
	{
		VDImplicitPlane xPlane = dir.x > 0.0f ? aabb.directionToImplicitPlane(VDDirection::LEFT) : aabb.directionToImplicitPlane(VDDirection::RIGHT);
		if (VDRayCastImplicitPlane(from, dir, xPlane, contactInfo))
			return true;
		VDImplicitPlane yPlane = dir.y > 0.0f ? aabb.directionToImplicitPlane(VDDirection::DOWN) : aabb.directionToImplicitPlane(VDDirection::UP);
		if (VDRayCastImplicitPlane(from, dir, yPlane, contactInfo))
			return true;
		VDImplicitPlane zPlane = dir.z > 0.0f ? aabb.directionToImplicitPlane(VDDirection::BACK) : aabb.directionToImplicitPlane(VDDirection::FORWARD);
		if (VDRayCastImplicitPlane(from, dir, zPlane, contactInfo))
			return true;
	}
	return false;
}

bool VDRayCastAABB(VDVector3 from, VDVector3 dir, const VDAABB& aabb, VDContactInfo& contactInfo, VDImplicitPlane& contactPlane)
{
	if (aabb.isPointInAABB(from))
	{
		contactInfo = VDContactInfo(from, dir, 0.0f, INTERNAL);
		return true;
	}
	VDVector3 dp = aabb.position - from;
	float dot = VDDot(dp, dir);
	if (dot >= 0.0f)
	{
		contactPlane = dir.x > 0.0f ? aabb.directionToImplicitPlane(VDDirection::LEFT) : aabb.directionToImplicitPlane(VDDirection::RIGHT);
		if (VDRayCastImplicitPlane(from, dir, contactPlane, contactInfo))
			return true;
		contactPlane = dir.y > 0.0f ? aabb.directionToImplicitPlane(VDDirection::DOWN) : aabb.directionToImplicitPlane(VDDirection::UP);
		if (VDRayCastImplicitPlane(from, dir, contactPlane, contactInfo))
			return true;
		contactPlane = dir.z > 0.0f ? aabb.directionToImplicitPlane(VDDirection::BACK) : aabb.directionToImplicitPlane(VDDirection::FORWARD);
		if (VDRayCastImplicitPlane(from, dir, contactPlane, contactInfo))
			return true;
	}
	return false;
}

bool VDRayCastOBB(VDVector3 from, VDVector3 dir, const VDOBB& obb, VDContactInfo& contactInfo, VDImplicitPlane& surface = VDImplicitPlane(), float distance = FLT_MAX)
{
	if (obb.isPointInOBB(from))
	{
		contactInfo = VDContactInfo(from, dir, 0.0f, INTERNAL);
		return true;
	}
	VDVector3 dp = obb.position - from;
	float dot = VDDot(dp, dir);
	if (dot >= 0.0f)
	{
		VDVector3 localDirection = obb.frame.localDirection(dir);
		surface = localDirection.x > 0.0f ? obb.directionToImplicitPlane(VDDirection::LEFT) : obb.directionToImplicitPlane(VDDirection::RIGHT);
		if (VDRayCastImplicitPlane(from, dir, surface, contactInfo) && contactInfo.distance <= distance)
			return true;
		surface = localDirection.y > 0.0f ? obb.directionToImplicitPlane(VDDirection::DOWN) : obb.directionToImplicitPlane(VDDirection::UP);
		if (VDRayCastImplicitPlane(from, dir, surface, contactInfo) && contactInfo.distance <= distance)
			return true;
		surface = localDirection.z > 0.0f ? obb.directionToImplicitPlane(VDDirection::BACK) : obb.directionToImplicitPlane(VDDirection::FORWARD);
		if (VDRayCastImplicitPlane(from, dir, surface, contactInfo) && contactInfo.distance <= distance)
			return true;
	}
	return false;
}

void VDCollisionBoxImplicitPlaneEdgeTest(const VDOBB& box, const VDEdge& edge, const VDImplicitPlane& plane, VDManifold& manifold)
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
			if (box.isPointInOBB(gap1.pointTo))
				manifold.insertEdgeContact(gap1);
		}
	}
	if (dir1 != dir2 && edge.closestEdgeToEdgeNoClamp(plane.getEdgeByDirection(dir2), gap2))
	{
		if (VDDot(gap2.dir, plane.frame.up) >= 0.0f && VDDot(gap2.dir, dir2Vec) >= 0.0f)
		{
			if (box.isPointInOBB(gap2.pointTo))
				manifold.insertEdgeContact(gap2);
		}
	}
}

void VDAddPenetrateEdgePoint(const VDOBB& box, VDVector3 edgePoint, VDVector3 dir, VDVector3 planeInward, VDList<VDEdge>& validGaps)
{
    if (box.isPointInOBB(edgePoint))
    {
        VDVector3 localDir = box.frame.localDirection(dir);

        VDImplicitPlane rightPlane = localDir.x >= 0.0f ? box.directionToImplicitPlane(VDDirection::RIGHT) : box.directionToImplicitPlane(VDDirection::LEFT);
        VDImplicitPlane upPlane = localDir.y >= 0.0f ? box.directionToImplicitPlane(VDDirection::UP) : box.directionToImplicitPlane(VDDirection::DOWN);
        VDImplicitPlane forwardPlane = localDir.z >= 0.0f ? box.directionToImplicitPlane(VDDirection::FORWARD) : box.directionToImplicitPlane(VDDirection::BACK);

        VDContactInfo contact, contactTest;
        contact.distance = FLT_MAX;

        // Only raycast if direction is nonzero
        if (fabs(localDir.x) > VD_COLLIDER_TOLERANCE)
        {
			VDRayCastPlane(edgePoint, box.frame.right * VDSign(localDir.x), rightPlane.normal(), rightPlane.center, contactTest);
            if (contactTest.distance < contact.distance)
                contact = contactTest;
        }
        if (fabs(localDir.y) > VD_COLLIDER_TOLERANCE)
        {
			VDRayCastPlane(edgePoint, box.frame.up * VDSign(localDir.y), upPlane.normal(), upPlane.center, contactTest);
            if (contactTest.distance < contact.distance)
                contact = contactTest;
        }
        if (fabs(localDir.z) > VD_COLLIDER_TOLERANCE)
        {
			VDRayCastPlane(edgePoint, box.frame.forward * VDSign(localDir.z), forwardPlane.normal(), forwardPlane.center, contactTest);
            if (contactTest.distance < contact.distance)
                contact = contactTest;
        }

        if (contact.distance < FLT_MAX && contact.distance > 0)
        {
            VDEdge penetrationEdge(contact.point, edgePoint);
			if(VDDot(penetrationEdge.dir, planeInward)>=0.0f)
				validGaps.insertSorted(penetrationEdge);
        }
    }
}

bool VDCollisionBoxEdge(const VDOBB& box, const VDEdge& edge, VDManifold& manifold)
{
	VDContactInfo contactInfo;

	//  The edge must intersect the box
	if(!VDRayCastOBB(edge.pointFrom, edge.dir, box, contactInfo, VDImplicitPlane(), edge.distance))
	{
		return false;
	}
	VDList<VDEdge> validGaps(true);

	// First check the edge points if either of them is inside the box
	VDAddPenetrateEdgePoint(box, edge.pointFrom, edge.dir, edge.planeInward, validGaps);
	VDAddPenetrateEdgePoint(box, edge.pointTo, -edge.dir, edge.planeInward, validGaps);

	// Now check the edges of the box
	VDEdge edges[12];
	box.getEdges(edges);
	for (int i = 0; i < 12; i++)
	{
		VDEdge& e = edges[i];
		VDEdge edgeGap;
		if (e.closestEdgeToEdgeNoClamp(edge, edgeGap))
		{
			if (box.isPointInOBB(edgeGap.pointTo, VD_COLLIDER_TOLERANCE) && edgeGap != e)
			{
				if (edgeGap.distance >= 0.0f && VDDot(edgeGap.dir, edge.planeInward)>=0.0f)
				{
					validGaps.insertSorted(edgeGap);
				}
			}
		}
	}

	if (validGaps.count == 0)
		return false;

	// The list is already sorted by distance
	bool found = false;
	VDEdge* first = &validGaps.pFirst->item;
	VDEdge* second = (validGaps.pFirst->pNext != nullptr) ? &validGaps.pFirst->pNext->item : nullptr;

	//If two edges are close enough, we can consider them both as contacts
	if (second && (second->distance - first->distance) <= VD_COLLIDER_TOLERANCE)
	{
		manifold.insertEdgeContact(*first);
		manifold.insertEdgeContact(*second);
		found = true;
	}
	else
	{
		manifold.insertEdgeContact(*first);
		found = true;
	}
	
	return found;
}

bool VDCollisionBoxImplicitPlane(const VDOBB& box, const VDImplicitPlane& plane, VDManifold& manifold, float skinWidth = 0.005f, float manfoldCombinationFactor = 0.95f)
{
	VDDirection closestFaceDirection = VDVectorToFrameDirection(-plane.frame.up, box.frame);
	VDVector3 closestFaceVector = VDDirectionToFrameVector(closestFaceDirection, box.frame);
	VDImplicitPlane face = box.directionToImplicitPlane(closestFaceDirection);
	VDVector3 faceVerts[4];
	face.extractVerts(faceVerts);
	for (int i = 0; i < 4; i++)
	{
		VDContactInfo ci;
		if (VDRayCastImplicitPlane(faceVerts[i] + closestFaceVector * (-skinWidth), plane.frame.up, plane, ci))
		{
			ci.normal = -ci.normal;
			ci.distance -= skinWidth;
			ci.point = faceVerts[i];
			manifold.insertContact(ci);
		}
	}

	// check for edge collisions
	VDManifold testManfold;
	VDCollisionBoxEdge(box, plane.getEdgeByDirection(VDDirection::RIGHT), testManfold);
	manifold.combineOrSwapIfSimilar(testManfold, manfoldCombinationFactor);
	testManfold = VDManifold(); // Reset test manifold for next edge
	VDCollisionBoxEdge(box, plane.getEdgeByDirection(VDDirection::LEFT), testManfold);
	manifold.combineOrSwapIfSimilar(testManfold, manfoldCombinationFactor);
	testManfold = VDManifold(); // Reset test manifold for next edge
	VDCollisionBoxEdge(box, plane.getEdgeByDirection(VDDirection::FORWARD), testManfold);
	manifold.combineOrSwapIfSimilar(testManfold, manfoldCombinationFactor);
	testManfold = VDManifold(); // Reset test manifold for next edge
	VDCollisionBoxEdge(box, plane.getEdgeByDirection(VDDirection::BACK), testManfold);
	manifold.combineOrSwapIfSimilar(testManfold, manfoldCombinationFactor);


	return manifold.count > 0;
}




#endif