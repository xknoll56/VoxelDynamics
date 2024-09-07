#ifndef VOXEL_DYNAMICS_COLLISION_DETECTION
#define VOXEL_DYNAMICS_COLLISION_DETECTION

#include "VoxelDynamicsCollider.h"

#define VD_COLLIDER_TOLERANCE 1e-5

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


struct VDContactInfo
{
	VDVector3 normal;
	float distance;
	VDVector3 point;

	VDContactInfo()
	{
		point = VDVector3();
		normal = VDVector3();
		distance = 0.0;
	}

	VDContactInfo(VDVector3 _point, VDVector3 _normal, float _distance)
	{
		normal = _normal;
		distance = _distance;
		point = _point;
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
		contactInfo = VDContactInfo(from, dir, 0.0f);
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



#endif