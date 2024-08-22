#ifndef VOXEL_DYNAMICS_BODY
#define VOXEL_DYNAMICS_BODY

#include "VoxelDynamicsCollider.h"

struct VDBody : VDCollider
{
	float mass;
	VDVector3 velocity;
	VDList<VDVector3> deltaMomentums;
	VDList<VDVector3> forces;
	float restitution;
	bool sleeping;
	float friction;
	bool useGravity;


	VDBody() :
		VDCollider(VDAABB())
	{
		mass = 1.0f;
		restitution = 0.5f;
		friction = 0.5f;
		sleeping = false;
		useGravity = true;
		velocity = VDVector3();
		forces = VDList<VDVector3>();
		deltaMomentums = VDList<VDVector3>();
	}

	VDBody(VDVector3 position, VDVector3 halfExtents, float mass) :
		VDCollider(VDAABB::fromMidPointAndHalfExtents(halfExtents, position))
	{
		this->mass = mass;
		restitution = 0.5f;
		friction = 0.5f;
		sleeping = false;
		useGravity = true;
		this->velocity = VDVector3();
		forces = VDList<VDVector3>();
		deltaMomentums = VDList<VDVector3>();
	}

	void applyDeltaMomentums()
	{
		for (auto dpData = deltaMomentums.pFirst; dpData != nullptr; dpData = dpData->pNext)
		{
			velocity += dpData->item * (1.0f / mass);
		}
		deltaMomentums.free();
		deltaMomentums = VDList<VDVector3>();
	}


	void applyForces(float dt)
	{
		for (auto fData = forces.pFirst; fData != nullptr; fData = fData->pNext)
		{
			velocity += fData->item * (dt / mass);
		}
		forces.free();
		forces = VDList<VDVector3>();
	}

	void clearForces()
	{
		forces.free();
		deltaMomentums.free();
	}


	void setSleeping(bool _sleeping)
	{
		sleeping = _sleeping;
		if (sleeping)
		{
			forces.free();
			deltaMomentums.free();
		}
	}


	virtual void simulate(float dt)
	{
		if (!sleeping)
		{
			applyDeltaMomentums();
			applyForces(dt);
			translate(velocity * dt);
		}
	}
};

struct VDAgentController : VDBody
{
	float speed;
	bool isGrounded;

	VDAgentController()
	{

	}

	VDAgentController(VDVector3 position, VDVector3 halfExtents, float speed) :
		VDBody(position, halfExtents, 1.0f)
	{
		this->speed = speed;
		isGrounded = false;
	}

	void move(VDVector3 direction, float dt)
	{
		translate(direction * dt * speed);
	}


	void jump(float force)
	{
		if (isGrounded)
		{
			deltaMomentums.insert(VDVector3::up() * force);
			isGrounded = false;
		}
	}
};

#endif