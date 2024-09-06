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

VDDirection VDVectorToDirection(VDVector3 vec)
{
	float xDir = VDDot(vec, VDVector3::right());
	float yDir = VDDot(vec, VDVector3::up());
	float zDir = VDDot(vec, VDVector3::forward());
	float absXDir = VDAbs(xDir);
	float absYDir = VDAbs(yDir);
	float absZDir = VDAbs(zDir);
	if (absXDir >= absYDir && absXDir >= absZDir)
	{
		if (xDir >= 0.0f)
		{
			return VDDirection::RIGHT;
		}
		else
		{
			return VDDirection::LEFT;
		}
	}
	else if (absYDir >= absZDir)
	{
		if (yDir >= 0.0f)
		{
			return VDDirection::UP;
		}
		else
		{
			return VDDirection::DOWN;
		}
	}
	else
	{
		if (zDir >= 0.0f)
		{
			return VDDirection::FORWARD;
		}
		else
		{
			return VDDirection::BACK;
		}
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

	virtual void setLowAndHigh()
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

	VDAABB skinnedAABB(float skinWidth)
	{
		VDAABB copy = *this;
		VDVector3 skin(skinWidth, skinWidth, skinWidth);
		copy.high += skin;
		copy.low += skin * -1.0f;
		copy.halfExtents += skin;
		return copy;
	}

	float crossSection(VDDirection dir)
	{
		switch (dir)
		{
		case VDDirection::RIGHT:
			return halfExtents.y * halfExtents.z*2.0f;
			break;
		case VDDirection::LEFT:
			return halfExtents.y * halfExtents.z * 2.0f;
			break;
		case VDDirection::UP:
			return halfExtents.x * halfExtents.z * 2.0f;
			break;
		case VDDirection::DOWN:
			return halfExtents.x * halfExtents.z * 2.0f;
			break;
		case VDDirection::FORWARD:
			return halfExtents.x * halfExtents.y * 2.0f;
			break;
		case VDDirection::BACK:
			return halfExtents.x * halfExtents.y * 2.0f;
			break;
		}
	}
};

enum VDOctant
{
	LEFT_DOWN_BACK = 0,
	RIGHT_DOWN_BACK = 1,
	LEFT_DOWN_FORWARD = 2,
	RIGHT_DOWN_FORWARD = 3,
	LEFT_UP_BACK = 4,
	RIGHT_UP_BACK = 5,
	LEFT_UP_FORWARD = 6,
	RIGHT_UP_FORWARD = 7,
};

struct VDImplicitPlane
{
	VDVector3 center;
	VDFrame frame;
	float rightHalfSize;
	float forwardHalfSize;
	float angle;

	VDImplicitPlane()
	{
		center = VDVector3();
		frame = VDFrame();
		rightHalfSize = 5.0f;
		forwardHalfSize = 5.0f;
		angle = 0.0f;
	}

	VDImplicitPlane(VDVector3 _center, VDVector3 _normal, float _rightHalfSize, float _forwardHalfSize, float _angle)
	{
		center = _center;
		frame.up = VDNormalize(_normal);
		angle = _angle;
		float dot = VDDot(frame.up, VDVector3::forward());
		if (dot == 1.0f)
			frame.right = VDVector3::right();
		else if (dot == -1.0f)
			frame.right = VDVector3::left();
		else
			frame.right = VDNormalize(VDCross(frame.up, VDVector3::forward()));
		VDQuaternion rotation = VDQuaternion::fromAngleAxis(frame.up, angle);
		frame.right = rotation.rotatePoint(frame.right);
		frame.forward = VDCross(frame.right, frame.up);
		rightHalfSize = _rightHalfSize;
		forwardHalfSize = _forwardHalfSize;
	}

	VDImplicitPlane& operator=(const VDImplicitPlane& other)
	{
		if (&other != this)
		{
			center = other.center;
			frame.up = other.frame.up;
			frame.right = other.frame.right;
			rightHalfSize = other.rightHalfSize;
			frame.forward = other.frame.forward;
			forwardHalfSize = other.forwardHalfSize;
			angle = other.angle;
		}
		return *this;
	}
};

struct VDOBB : VDAABB
{
	VDQuaternion rotation;
	VDFrame frame;
	VDVector3 vertices[8];

	VDOBB() : VDAABB(VDVector3::half()*-1.0f, VDVector3::half())
	{
		rotation = VDQuaternion();
		frame = rotation.toFrame();
	}

	VDOBB(VDVector3 _position, VDVector3 _halfExtents, VDQuaternion _rotation)
	{
		halfExtents = _halfExtents;
		setPosition(_position);
		rotation = _rotation;
		frame = rotation.toFrame();
	}

	void setRotation(VDQuaternion _rotation)
	{
		rotation = _rotation;
		frame = rotation.toFrame();
	}

	void rotate(VDQuaternion _rotation)
	{
		rotation.rotate(_rotation);
		frame = rotation.toFrame();
	}

	void setLowAndHigh()
	{
		VDVector3 absRight = VDAbs(frame.right);
		VDVector3 absUp = VDAbs(frame.up);
		VDVector3 absForward = VDAbs(frame.forward);
		VDVector3 curHalfExtents(absRight.x * halfExtents.x + absUp.x * halfExtents.y + absForward.x * halfExtents.z,
			absRight.y * halfExtents.x + absUp.y * halfExtents.y + absForward.y * halfExtents.z,
			absRight.z * halfExtents.x + absUp.z * halfExtents.y + absForward.z * halfExtents.z);
		low = position - curHalfExtents;
		high = position + curHalfExtents;
	}

	void setVertices()
	{
		vertices[VDOctant::LEFT_DOWN_BACK] = position - (frame.right * halfExtents.x) - (frame.up * halfExtents.y) - (frame.forward * halfExtents.z);
		vertices[VDOctant::RIGHT_DOWN_BACK] = position + (frame.right * halfExtents.x) - (frame.up * halfExtents.y) - (frame.forward * halfExtents.z);
		vertices[VDOctant::LEFT_DOWN_FORWARD] = position - (frame.right * halfExtents.x) - (frame.up * halfExtents.y) + (frame.forward * halfExtents.z);
		vertices[VDOctant::RIGHT_DOWN_FORWARD] = position + (frame.right * halfExtents.x) - (frame.up * halfExtents.y) + (frame.forward * halfExtents.z);
		vertices[VDOctant::LEFT_UP_BACK] = position - (frame.right * halfExtents.x) + (frame.up * halfExtents.y) - (frame.forward * halfExtents.z);
		vertices[VDOctant::RIGHT_UP_BACK] = position + (frame.right * halfExtents.x) + (frame.up * halfExtents.y) - (frame.forward * halfExtents.z);
		vertices[VDOctant::LEFT_UP_FORWARD] = position - (frame.right * halfExtents.x) + (frame.up * halfExtents.y) + (frame.forward * halfExtents.z);
		vertices[VDOctant::RIGHT_UP_FORWARD] = position + (frame.right * halfExtents.x) + (frame.up * halfExtents.y) + (frame.forward * halfExtents.z);
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



#endif