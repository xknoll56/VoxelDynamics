#ifndef VOXEL_DYNAMICS_COLLIDER
#define VOXEL_DYNAMICS_COLLIDER

#include "VoxelDynamicsMath.h"
#include "VoxelDynamicsAllocator.h"

enum VDDirection
{
	RIGHT = 0,
	UP,
	FORWARD,
	LEFT,
	DOWN,
	BACK
};

VDVector3 VDDirectionToVector(VDDirection dir)
{
	switch (dir)
	{
	case VDDirection::RIGHT:
		return VDVector3::right();
		break;
	case VDDirection::LEFT:
		return VDVector3::left();
		break;
	case VDDirection::UP:
		return VDVector3::up();
		break;
	case VDDirection::DOWN:
		return VDVector3::down();
		break;
	case VDDirection::FORWARD:
		return VDVector3::forward();
		break;
	case VDDirection::BACK:
		return VDVector3::back();
		break;
	}
}

enum VDColliderType
{
	AABB = 0,
};

struct VDAABB;

struct VDAABBContact;



struct VDVoxel;

struct VDAABB
{
	VDVector3 low;
	VDVector3 high;
	VDVector3 halfExtents;
	VDVector3 position;
	

	void setMidPointAndHalfExtents()
	{
		position = (low + high) * 0.5f;
		halfExtents = high - position;
	}

	void setLowAndHigh()
	{
		low = position - halfExtents;
		high = position + halfExtents;
	}

	VDAABB(VDVector3 _low, VDVector3 _high)
	{
		//colliderType = VDColliderType::AABB;
		low = _low;
		high = _high;
		setMidPointAndHalfExtents();
		//colliderType == VDColliderType::AABB;
	}

	VDAABB()
	{
		VDAABB(VDVector3::zero(), VDVector3::one());
	}

	VDAABB(const VDAABB& other)
	{
		low = other.low;
		high = other.high;
		halfExtents = other.halfExtents;
		position = other.position;
	}

	VDAABB& operator=(const VDAABB& other)
	{
		if (this != &other)
		{
			low = other.low;
			high = other.high;
			halfExtents = other.halfExtents;
			position = other.position;
		}
		return *this;
	}

	void setPosition(VDVector3 position)
	{
		this->position = position;
		setLowAndHigh();
	}

	void translate(VDVector3 translation)
	{
		position += translation;
		setLowAndHigh();
	}

	void setHalfExtents(VDVector3 _halfExtents)
	{
		halfExtents = _halfExtents;
		setLowAndHigh();
	}

	static VDAABB fromMidPointAndHalfExtents(VDVector3 _halfExtents, VDVector3 _midPoint)
	{
		return VDAABB(_midPoint - _halfExtents, _midPoint + _halfExtents);
	}

	void expandToInclide(VDAABB other)
	{
		high = VDVector3(VDMax(high.x, other.high.x), VDMax(high.y, other.high.y), VDMax(high.z, other.high.z));
		low = VDVector3(VDMin(low.x, other.low.x), VDMin(low.y, other.low.y), VDMin(low.z, other.low.z));
		setMidPointAndHalfExtents();
	}

	bool validate()
	{
		return high.x >= low.x && high.y >= low.y && high.z >= low.z;
	}

	bool isIntersecting(const VDAABB& other)
	{
		return (low.x <= other.high.x && high.x >= other.low.x) &&
			(low.y <= other.high.y && high.y >= other.low.y) &&
			(low.z <= other.high.z && high.z >= other.low.z);
	}

	bool intersectionRegion(VDAABB other, VDAABB& region)
	{
		region.high = VDMin(high, other.high);
		region.low = VDMax(low, other.low);
		region.setMidPointAndHalfExtents();
		return region.validate();
	}

	bool collisionAABB(const VDAABB* pOther, VDAABBContact& contact);

	void resolveAABBContact(const VDAABBContact& contact);
};

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

struct VDCollider : VDAABB
{
	VDColliderType colliderType;
	VDList<VDList<VDPointer>> occupiedChunks;

	VDCollider() :VDAABB()
	{
		colliderType = VDColliderType::AABB;
	}

	VDCollider(VDAABB aabb) : VDAABB(aabb)
	{
		occupiedChunks = VDList<VDList<VDPointer>>();
		colliderType = VDColliderType::AABB;
	}

	const VDCollider& operator=(const VDCollider& other)
	{
		if (this != &other)
		{
			VDAABB::operator=(other);
			colliderType = other.colliderType;
			occupiedChunks = other.occupiedChunks;
		}

		return *this;
	}
};



struct VDContactPoint
{
	VDCollider* pContact;
	VDCollider* pOtherContact;
	VDVector3 normal;
	float penetrationDistance;
	VDVector3 point;

	VDContactPoint()
	{
		pContact = nullptr;
		pOtherContact = nullptr;
		point = VDVector3();
		normal = VDVector3();
		penetrationDistance = 0.0;
	}

	VDContactPoint(VDCollider* _pContact, VDCollider* _pOtherContact, VDVector3 _point, VDVector3 _normal, float _penetrationDistance)
	{
		pContact = _pContact;
		pOtherContact = _pOtherContact;
		normal = _normal;
		penetrationDistance = _penetrationDistance;
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

#endif