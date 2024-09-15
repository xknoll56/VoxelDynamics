#ifndef VOXEL_DYNAMICS_COLLISION_DETECTION
#define VOXEL_DYNAMICS_COLLISION_DETECTION

#include "VoxelDynamicsCollider.h"


struct VDPenetrationField
{
	float maxPenetrations[6];
	VDPointer pVoxels[6];

	VDPenetrationField()
	{
		memset(maxPenetrations, 0.0f, 6 * sizeof(float));
		memset(pVoxels, NULL, 6 * sizeof(float));
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
		contactPoint.point = pointOnPlane;
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

bool VDRayCastImplicitPlane(VDVector3 from, VDVector3 dir, const VDImplicitPlane& plane, VDContactInfo& contactInfo)
{
	if (VDRayCastPlane(from, dir, plane.frame.up, plane.center, contactInfo))
	{
		return VDRayCastImplicitPlaneContained(plane, contactInfo);
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

bool VDRayCastOBB(VDVector3 from, VDVector3 dir, const VDOBB& obb, VDContactInfo& contactInfo)
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
		VDImplicitPlane xPlane = localDirection.x > 0.0f ? obb.directionToImplicitPlane(VDDirection::LEFT) : obb.directionToImplicitPlane(VDDirection::RIGHT);
		if (VDRayCastImplicitPlane(from, dir, xPlane, contactInfo))
			return true;
		VDImplicitPlane yPlane = localDirection.y > 0.0f ? obb.directionToImplicitPlane(VDDirection::DOWN) : obb.directionToImplicitPlane(VDDirection::UP);
		if (VDRayCastImplicitPlane(from, dir, yPlane, contactInfo))
			return true;
		VDImplicitPlane zPlane = localDirection.z > 0.0f ? obb.directionToImplicitPlane(VDDirection::BACK) : obb.directionToImplicitPlane(VDDirection::FORWARD);
		if (VDRayCastImplicitPlane(from, dir, zPlane, contactInfo))
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
		if (VDRayCastImplicitPlane(faceVerts[i] + closestFaceVector * (-skinWidth), plane.frame.up, plane, ci))
		{
			ci.normal = -ci.normal;
			ci.distance -= skinWidth;
			manifold.insertContact(ci);
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
			VDCollisionBoxImplicitPlaneEdgeTest(box, edge, plane, manifold);
		}
	}
	else
	{
		for (int i = 0; i < 4; i++)
		{
			VDEdge edge = inwardFace.getEdgeByDirection(edgeDirs[i]);
			VDCollisionBoxImplicitPlaneEdgeTest(box, edge, plane, manifold);
		}
	}


	return true;
}



#endif