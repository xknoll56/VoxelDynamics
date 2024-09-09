#ifndef VOXEL_DYNAMICS_COLLIDER
#define VOXEL_DYNAMICS_COLLIDER

#include "VoxelDynamicsMath.h"
#include "VoxelDynamicsAllocator.h"


#define VD_COLLIDER_TOLERANCE 1e-5

enum VDDirection
{
	RIGHT = 0,
	UP,
	FORWARD,
	LEFT,
	DOWN,
	BACK
};

VDDirection VDDirectionOpposite(VDDirection dir)
{
	switch (dir)
	{
	case VDDirection::RIGHT:
		return VDDirection::LEFT;
		break;
	case VDDirection::LEFT:
		return VDDirection::RIGHT;
		break;
	case VDDirection::UP:
		return VDDirection::DOWN;
		break;
	case VDDirection::DOWN:
		return VDDirection::UP;
		break;
	case VDDirection::FORWARD:
		return VDDirection::BACK;
		break;
	case VDDirection::BACK:
		return VDDirection::FORWARD;
		break;
	}
}

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

VDVector3 VDDirectionToFrameVector(VDDirection dir, const VDFrame& frame)
{
	switch (dir)
	{
	case VDDirection::RIGHT:
		return frame.right;
		break;
	case VDDirection::LEFT:
		return -frame.right;
		break;
	case VDDirection::UP:
		return frame.up;
		break;
	case VDDirection::DOWN:
		return -frame.up;
		break;
	case VDDirection::FORWARD:
		return frame.forward;
		break;
	case VDDirection::BACK:
		return -frame.forward;
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

VDDirection VDVectorToFrameDirection(VDVector3 vec, const VDFrame& frame)
{
	float xDir = VDDot(vec, frame.right);
	float yDir = VDDot(vec, frame.up);
	float zDir = VDDot(vec, frame.forward);
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

struct VDEdge
{
	VDVector3 pointFrom;
	VDVector3 pointTo;
	VDVector3 dir;
	float distance;

	VDEdge()
	{
		pointFrom = VDVector3::zero();
		pointTo = VDVector3::right();
		setDirection();
	}

	VDEdge(VDVector3 _point1, VDVector3 _point2)
	{
		pointFrom = _point1;
		pointTo = _point2;
		setDirection();
	}

	VDEdge(VDVector3 _pointFrom, VDVector3 _dir, float _distance)
	{
		pointFrom = _pointFrom;
		dir = _dir;
		distance = _distance;
		pointTo = pointFrom + distance * dir;
	}

	VDEdge(VDVector3 _pointFrom, VDVector3 _pointTo, VDVector3 _dir, float _distance)
	{
		pointFrom = _pointFrom;
		pointTo = _pointTo;
		dir = _dir;
		distance = _distance;
	}

	VDEdge closestEdgeToPoint(VDVector3 point) const
	{
		VDVector3 dp = point - pointFrom;
		float projection = VDDot(dp, dir);
		float clampedProjection = VDClamp(projection, 0.0f, distance);
		VDVector3 closestPoint = pointFrom + clampedProjection * dir;
		return VDEdge(closestPoint, point);
	}

	VDEdge closestEdgeToEdge(VDEdge edge) const
	{
		VDVector3 cross = VDCross(dir, edge.dir);
		float crossLength2 = cross.length2();
		if (crossLength2 < VD_COLLIDER_TOLERANCE)
		{
			// The edges are nearly parallel, so find the closest point from one edge to the other
			return closestEdgeToPoint(edge.pointFrom);
		}
		float t = VDDot(VDCross(edge.pointFrom - pointFrom, edge.dir), cross) / crossLength2;
		float s = VDDot(VDCross(edge.pointFrom - pointFrom, dir), cross) / crossLength2;
		t = VDClamp(t, 0.0f, distance);
		s = VDClamp(s, 0.0f, edge.distance);
		return VDEdge(pointFrom + dir * t, edge.pointFrom + edge.dir * s);
	}

	void align(VDVector3 alignment)
	{
		if (VDDot(alignment, dir) < 0.0f)
		{
			VDVector3 temp = pointFrom;
			pointFrom = pointTo;
			pointTo = temp;
			setDirection();
		}
	}

	void setDirection()
	{
		VDVector3 dp = pointTo - pointFrom;
		distance = dp.length();
		dir = dp * (1.0f / distance);
	}
};


struct VDImplicitPlane
{
	VDVector3 center;
	VDFrame frame;
	float rightHalfSize;
	float forwardHalfSize;

	VDImplicitPlane()
	{
		center = VDVector3();
		frame = VDFrame();
		rightHalfSize = 5.0f;
		forwardHalfSize = 5.0f;
	}

	VDImplicitPlane(VDVector3 _center, VDVector3 _normal, float _rightHalfSize, float _forwardHalfSize, float angle)
	{
		center = _center;
		frame.up = VDNormalize(_normal);
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

	VDImplicitPlane(VDVector3 _center, VDFrame _frame, float _rightHalfSize, float _forwardHalfSize)
	{
		center = _center;
		frame = _frame;
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
		}
		return *this;
	}

	VDVector3 normal() const
	{
		return frame.up;
	}

	void extractVerts(VDVector3 verts[4])
	{
		verts[0] = center + frame.right * rightHalfSize + frame.forward * forwardHalfSize;
		verts[1] = center + frame.right * rightHalfSize - frame.forward * forwardHalfSize;
		verts[2] = center - frame.right * rightHalfSize + frame.forward * forwardHalfSize;
		verts[3] = center - frame.right * rightHalfSize - frame.forward * forwardHalfSize;
	}

	VDVector3 getVertexByDirections(VDDirection xDir, VDDirection zDir) const
	{
		if (xDir == VDDirection::RIGHT)
		{
			if (zDir == VDDirection::FORWARD)
			{
				return center + frame.right * rightHalfSize + frame.forward * forwardHalfSize;
			}
			else if (zDir == VDDirection::BACK)
			{
				return center + frame.right * rightHalfSize - frame.forward * forwardHalfSize;
			}
		}
		else if (xDir == VDDirection::LEFT)
		{
			if (zDir == VDDirection::FORWARD)
			{
				return center - frame.right * rightHalfSize + frame.forward * forwardHalfSize;
			}
			else if (zDir == VDDirection::BACK)
			{
				return center - frame.right * rightHalfSize - frame.forward * forwardHalfSize;
			}
		}
		return VDVector3::nan();
	}

	VDEdge getEdgeByDirection(VDDirection dir) const
	{
		switch (dir)
		{
		case VDDirection::RIGHT:
			return VDEdge(getVertexByDirections(VDDirection::RIGHT, VDDirection::BACK),
				getVertexByDirections(VDDirection::RIGHT, VDDirection::FORWARD), frame.forward, 2.0f * forwardHalfSize);
			break;
		case VDDirection::LEFT:
			return VDEdge(getVertexByDirections(VDDirection::LEFT, VDDirection::BACK),
				getVertexByDirections(VDDirection::LEFT, VDDirection::FORWARD), frame.forward, 2.0f * forwardHalfSize);
			break;
		case VDDirection::FORWARD:
			return VDEdge(getVertexByDirections(VDDirection::LEFT, VDDirection::FORWARD),
				getVertexByDirections(VDDirection::RIGHT, VDDirection::FORWARD), frame.right, 2.0f * rightHalfSize);
			break;
		case VDDirection::BACK:
			return VDEdge(getVertexByDirections(VDDirection::LEFT, VDDirection::BACK),
				getVertexByDirections(VDDirection::RIGHT, VDDirection::BACK), frame.right, 2.0f * rightHalfSize);
			break;
		}
		return VDEdge();
	}

	VDEdge closestEdgeToPoint(VDVector3 point) const
	{
		VDVector3 dp = point - center;
		float rightNormDist = VDDot(dp, frame.right) / rightHalfSize;
		float forwardNormDist = VDDot(dp, frame.forward) / forwardHalfSize;
		if (VDAbs(rightNormDist) >= VDAbs(forwardNormDist))
		{
			if (rightNormDist >= 0.0f)
				return getEdgeByDirection(VDDirection::RIGHT);
			else
				return getEdgeByDirection(VDDirection::LEFT);
		}
		else
		{
			if (forwardNormDist >= 0.0f)
				return getEdgeByDirection(VDDirection::FORWARD);
			else
				return getEdgeByDirection(VDDirection::BACK);
		}
		return VDEdge();
	}
};

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

	bool isIntersecting(const VDAABB& other) const
	{
		return (low.x <= other.high.x && high.x >= other.low.x) &&
			(low.y <= other.high.y && high.y >= other.low.y) &&
			(low.z <= other.high.z && high.z >= other.low.z);
	}

	bool isPointInAABB(const VDVector3& point) const
	{
		return (point.x >= low.x && point.x <= high.x) &&
			(point.y >= low.y && point.y <= high.y) &&
			(point.z >= low.z && point.z <= high.z);
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

	VDImplicitPlane directionToImplicitPlane(VDDirection dir) const
	{
		switch (dir)
		{
		case VDDirection::RIGHT:
			return VDImplicitPlane(position + halfExtents.xComponentVector(),
				VDFrame(VDVector3::up(), VDVector3::right(), VDVector3::forward()), halfExtents.y, halfExtents.z);
			break;
		case VDDirection::LEFT:
			return VDImplicitPlane(position - halfExtents.xComponentVector(),
				VDFrame(VDVector3::up(), VDVector3::left(), VDVector3::forward()), halfExtents.y, halfExtents.z);
			break;
		case VDDirection::UP:
			return VDImplicitPlane(position + halfExtents.yComponentVector(),
				VDFrame(VDVector3::right(), VDVector3::up(), VDVector3::forward()), halfExtents.x, halfExtents.z);
				break;
		case VDDirection::DOWN:
			return VDImplicitPlane(position - halfExtents.yComponentVector(),
				VDFrame(VDVector3::right(), VDVector3::down(), VDVector3::forward()), halfExtents.x, halfExtents.z);
			break;
		case VDDirection::FORWARD:
			return VDImplicitPlane(position + halfExtents.zComponentVector(),
				VDFrame(VDVector3::right(), VDVector3::forward(), VDVector3::up()), halfExtents.x, halfExtents.y);
			break;
		case VDDirection::BACK:
			return VDImplicitPlane(position - halfExtents.zComponentVector(),
				VDFrame(VDVector3::right(), VDVector3::back(), VDVector3::up()), halfExtents.x, halfExtents.y);
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


struct VDOBB : VDAABB
{
	VDQuaternion rotation;
	VDFrame frame;
	VDVector3 vertices[8];
	bool vertsUpdated;

	VDOBB() : VDAABB(VDVector3::half()*-1.0f, VDVector3::half())
	{
		rotation = VDQuaternion();
		frame = rotation.toFrame();
		setVertices();
	}

	VDOBB(VDVector3 _position, VDVector3 _halfExtents, VDQuaternion _rotation)
	{
		halfExtents = _halfExtents;
		setPosition(_position);
		rotation = _rotation;
		frame = rotation.toFrame();
		setVertices();
	}

	void setRotation(VDQuaternion _rotation)
	{
		rotation = _rotation;
		frame = rotation.toFrame();
		vertsUpdated = false;
	}

	void rotate(VDQuaternion _rotation)
	{
		rotation.rotate(_rotation);
		frame = rotation.toFrame();
		vertsUpdated = false;
	}

	void setPosition(VDVector3 position)
	{
		VDAABB::setPosition(position);
		vertsUpdated = false;
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
		vertsUpdated = true;
	}

	bool usingVertices()
	{
		if (!vertsUpdated)
			setVertices();
	}
	

	VDImplicitPlane directionToImplicitPlane(VDDirection dir) const
	{
		switch (dir)
		{
		case VDDirection::RIGHT:
			return VDImplicitPlane(position + frame.right * halfExtents.x,
				VDFrame(frame.up, frame.right, frame.forward), halfExtents.y, halfExtents.z);
			break;
		case VDDirection::LEFT:
			return VDImplicitPlane(position - frame.right * halfExtents.x,
				VDFrame(frame.up, -frame.right, frame.forward), halfExtents.y, halfExtents.z);
			break;
		case VDDirection::UP:
			return VDImplicitPlane(position + frame.up*halfExtents.y,
				VDFrame(frame.right, frame.up, frame.forward), halfExtents.x, halfExtents.z);
			break;
		case VDDirection::DOWN:
			return VDImplicitPlane(position - frame.up*halfExtents.y,
				VDFrame(frame.right, -frame.up, frame.forward), halfExtents.x, halfExtents.z);
			break;
		case VDDirection::FORWARD:
			return VDImplicitPlane(position + frame.forward*halfExtents.z,
				VDFrame(frame.right, frame.forward, frame.up), halfExtents.x, halfExtents.y);
			break;
		case VDDirection::BACK:
			return VDImplicitPlane(position - frame.forward * halfExtents.z,
				VDFrame(frame.right, -frame.forward, frame.up), halfExtents.x, halfExtents.y);
			break;
		}
	}

	bool isPointInOBB(const VDVector3& point) const
	{
		VDVector3 dists = VDAbs(frame.localPosition(point, position));
		return (dists.x <= halfExtents.x && dists.y <= halfExtents.y && dists.z <= halfExtents.z);
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