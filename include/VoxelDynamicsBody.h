#ifndef VOXEL_DYNAMICS_BODY
#define VOXEL_DYNAMICS_BODY

#include "VoxelDynamicsCollider.h"

struct VDBodyBase
{
	float mass;
	VDVector3 velocity;
	VDVector3 momentum;
	VDList<VDVector3> deltaMomentums;
	VDList<VDVector3> forces;
	float restitution;
	bool sleeping;
	float friction;
	bool useGravity;


	VDBodyBase()
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



	void applyDeltaMomentums()
	{
		for (auto dpData = deltaMomentums.pFirst; dpData != nullptr; dpData = dpData->pNext)
		{
			momentum += dpData->item;
		}
		deltaMomentums.free();
		deltaMomentums = VDList<VDVector3>();
	}


	void applyForces(float dt)
	{
		for (auto fData = forces.pFirst; fData != nullptr; fData = fData->pNext)
		{
			momentum += fData->item * dt;
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
			momentum = VDVector3();
			velocity = VDVector3();
		}
	}


	virtual void simulate(float dt)
	{
		if (!sleeping)
		{
			applyDeltaMomentums();
			applyForces(dt);
			velocity = momentum * (1.0f / mass);
		}
	}
};
struct VDBody : VDCollider, VDBodyBase
{

	VDBody() :
		VDBodyBase(), VDCollider(VDAABB()) {

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

	void simulate(float dt) override
	{
		VDBodyBase::simulate(dt);
		if(!sleeping)
			translate(velocity * dt);
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