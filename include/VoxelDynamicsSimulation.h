#ifndef VOXEL_DYNAMICS_SIMULATION
#define VOXEL_DYNAMICS_SIMULATION

#include "VoxelDynamicsAllocator.h"
#include "VoxelDynamicsMath.h"
#include "VoxelDynamicsCollider.h"
#include "VoxelDynamicsSpace.h"
#include "VoxelDynamicsBody.h"

struct VDSimulation
{
	VDSpace space;
	VDList<VDBody*> pBodies;
	VDList<VDBody> aabbBodies;
	VDList<VDAgentController> agents;
	VDVector3 gravity;
	float dtCap;
	float frictionFactor = 0.15f;

	VDSimulation() : space(VDSpace())
	{
		pBodies = VDList<VDBody*>();
		aabbBodies = VDList<VDBody>();
		agents = VDList<VDAgentController>();
		gravity = VDVector3(0, -9.81f, 0);
		dtCap = 1.0f / 60.0f;
	}

	VDSimulation(VDuint gridSize, VDVector3i anchor, VDuint horizontalGrids, VDuint verticalGrids) :
		space(gridSize, anchor, horizontalGrids, verticalGrids)
	{
		pBodies = VDList<VDBody*>();
		aabbBodies = VDList<VDBody>();
		agents = VDList<VDAgentController>();
		gravity = VDVector3(0, -9.81f, 0);
		dtCap = 1.0f / 60.0f;
	}



	VDBody* createAABBBody(VDAABB aabb, float mass)
	{
		VDBody* pBody = aabbBodies.insert(VDBody(aabb.position, aabb.halfExtents, mass));
		space.insertCollider(*pBody);
		pBodies.insert(pBody);
		return pBody;
	}

	VDAgentController* createAgentController(VDVector3 position, VDVector3 halfExtents, float speed)
	{
		VDAgentController* pAgent = agents.insert(VDAgentController(position, halfExtents, speed));
		return pAgent;
	}

	void multiVoxelContactResolution(VDAABB& aabb, const VDList<VDVoxel*>& voxels, VDPenetrationField& penetrationsField, VDList<VDContactPoint>& contactPoints) const
	{
		VDuint chunkIndex = 0;
		VDVector3i chunkCoords;
		for (auto voxelListData = voxels.pFirst; voxelListData != nullptr; voxelListData = voxelListData->pNext)
		{
			if (voxelListData->item->chunkIndex != chunkIndex)
			{
				chunkIndex = voxelListData->item->chunkIndex;
				chunkCoords = space.getCoordinates(chunkIndex);
			}
			VDAABBContact c = voxelListData->item->voxelContact(aabb);
			c.setPenetrations();
			VDDirection minDirection = c.minDirections[0];
			VDVector3i newVoxCoord = space.moveIndex(voxelListData->item->index, chunkCoords, minDirection);
			bool valid = space.validateChunkCoord(chunkCoords) && space.grids[chunkIndex].pChunk->validateCoords(newVoxCoord);
			VDuint newVoxIndex = space.grids[chunkIndex].pChunk->getIndex(newVoxCoord);
			bool occupied = !valid && space.grids[chunkIndex].pChunk->getOccupied(newVoxIndex);
			bool allOccupied = false;
			if (occupied)
			{
				minDirection = c.minDirections[1];
				newVoxCoord = space.moveIndex(voxelListData->item->index, chunkCoords, minDirection);
				valid = space.validateChunkCoord(chunkCoords) && space.grids[chunkIndex].pChunk->validateCoords(newVoxCoord);
				newVoxIndex = space.grids[chunkIndex].pChunk->getIndex(newVoxCoord);
				occupied = !valid && space.grids[chunkIndex].pChunk->getOccupied(newVoxIndex);
				if (occupied)
				{
					allOccupied = true;
				}
			}
			if (!allOccupied)
			{
				float penetration = c.getPenetrationByDirection(minDirection);
				penetrationsField.insertPenetration(minDirection, penetration, voxelListData->item);
			}
		}
		for (int i = 0; i < 6; i++)
		{
			if (penetrationsField.maxPenetrations[i] > 0.0f)
			{
				contactPoints.insert(VDContactPoint((VDPointer)&aabb, penetrationsField.pVoxels[i],
					aabb.position, VDDirectionToVector((VDDirection)i), penetrationsField.maxPenetrations[i]));
			}
		}
	}

	void updateAgent(VDAgentController& agent, const VDSpace& space, float dt)
	{
		agent.forces.insert(gravity);
		agent.simulate(dt);

		VDList<VDPointer> uniqueColliders(true);
		VDList<VDVoxel*> sampledVoxels(true);
		space.sampleOccupiedRegion(agent, sampledVoxels, uniqueColliders);
		for (auto it = uniqueColliders.pFirst; it != nullptr; it = it->pNext)
		{
			VDAABBContact contact;
			if (agent.collisionAABB((VDAABB*)it->item, contact))
			{
				agent.resolveAABBContact(contact);
				if (contact.minDirections[0] == VDDirection::UP)
				{
					agent.momentum.y = 0.0f;
					agent.isGrounded = true;
				}
			}
		}
		VDPenetrationField field;
		VDList<VDContactPoint> voxelContactPoints(true);
		multiVoxelContactResolution(agent, sampledVoxels, field, voxelContactPoints);
		for (auto it = voxelContactPoints.pFirst; it != nullptr; it = it->pNext)
		{
			agent.translate(it->item.normal*it->item.penetrationDistance);
		}
		if (field.maxPenetrations[VDDirection::UP] > 0.0f)
		{
			agent.momentum.y = 0.0f;
			agent.isGrounded = true;
		}
	}



	void simulateAgents(float dt)
	{
		for (auto agentIt = agents.pFirst; agentIt != nullptr; agentIt = agentIt->pNext)
			updateAgent(agentIt->item, space, dt);
	}

	void resolveAABBStaticBodyContact(VDBody* pBody, const VDContactPoint& contactPoint, float dt)
	{
		pBody->translate(contactPoint.normal * contactPoint.penetrationDistance);
		VDVector3 vn = contactPoint.normal * VDDot(contactPoint.normal, pBody->velocity) * -1.0f;
		pBody->deltaMomentums.insert(vn * pBody->mass * pBody->restitution );
		VDVector3 normalVelocity = VDNormalComponent(pBody->velocity, contactPoint.normal);
		VDVector3 frictionDir = pBody->velocity - normalVelocity;
		frictionDir.normalize();
		frictionDir = frictionDir * -1.0f;
		
		float vMag = pBody->velocity.length();
		if(frictionDir.length()>0.0f)
			pBody->deltaMomentums.insert(frictionDir * vMag * pBody->mass * pBody->friction * frictionFactor );
		if (vMag < 0.5f && contactPoint.normal.y > 0.0f)
			pBody->setSleeping(true);
	}

	void resolveAABBDynamicBodyContact(VDBody* pBody, const VDContactPoint& contactPoint, float dt)
	{
		VDBody* pOtherBody = (VDBody*)contactPoint.pOtherContact;
		pBody->translate(contactPoint.normal * contactPoint.penetrationDistance*0.5f);
		pOtherBody->translate(contactPoint.normal * -contactPoint.penetrationDistance * 0.5f);

		VDVector3 vRel = (pOtherBody->velocity - pBody->velocity);
		VDVector3 vn = contactPoint.normal * VDDot(contactPoint.normal, vRel);

		float vMag = vRel.length();
		float vnMag = vn.length();
		
		if (vMag < 0.5f)
		{
			if(contactPoint.normal.y > 0.0f)
				pBody->setSleeping(true);
		}
		else
		{
			if (pOtherBody->sleeping)
				pOtherBody->setSleeping(false);
			if (contactPoint.penetrationDistance>0.1f)
			{
				int test = 1;
			}
			pBody->deltaMomentums.insert(vn * pBody->mass * pBody->restitution);
			pOtherBody->deltaMomentums.insert(vn * pBody->mass * -pBody->restitution);
			//VDVector3 normalVelocity = VDNormalComponent(vRel, contactPoint.normal);
			VDVector3 frictionDir = VDTangentialComponent(vRel, contactPoint.normal);
			frictionDir.normalize();
			frictionDir = frictionDir;
			if (frictionDir.length() > 0.0f)
			{
				pBody->deltaMomentums.insert(frictionDir * vnMag * pBody->mass * pBody->friction * frictionFactor);
				pOtherBody->deltaMomentums.insert(frictionDir * vnMag * -pBody->mass * pBody->friction * frictionFactor);
			}
		}
	}

	void simulateBodies(float dt)
	{
		for (auto it = pBodies.pFirst; it != nullptr; it = it->pNext)
		{
			if(it->item->useGravity && !it->item->sleeping)
				it->item->forces.insert(gravity*it->item->mass);
			it->item->simulate(dt);
			space.updateCollider(*it->item);


			VDList<VDPointer> uniqueColliders(true);
			VDList<VDVoxel*> sampledVoxels(true);
			space.sampleOccupiedRegion(*it->item, sampledVoxels, uniqueColliders);
			VDCollider* pThis = (VDCollider*)it->item;
			for (auto collIt = uniqueColliders.pFirst; collIt != nullptr; collIt = collIt->pNext)
			{
				VDCollider* pAABBCollider = collIt->item;
				if (pThis != pAABBCollider)
				{
					VDAABB intersectionRegion;
					bool doesIntersect = it->item->intersectionRegion(*pAABBCollider, intersectionRegion);
					if (doesIntersect)
					{
						VDVector3 quadrantDir = VDSign(it->item->position - pAABBCollider->position);
						VDAABBContact contact((VDPointer)it->item, (VDPointer)collIt->item, intersectionRegion, quadrantDir);
						contact.setPenetrations();
						VDContactPoint contactPoint((VDPointer)it->item, (VDPointer)collIt->item, intersectionRegion.position,
							VDDirectionToVector(contact.minDirections[0]), contact.getPenetrationByDirection(contact.minDirections[0]));
						resolveAABBDynamicBodyContact(it->item, contactPoint, dt);
					}
				}
			}
			VDPenetrationField field;
			VDList<VDContactPoint> voxelContactPoints(true);
			multiVoxelContactResolution(*it->item, sampledVoxels, field, voxelContactPoints);
			for (auto cpIt = voxelContactPoints.pFirst; cpIt != nullptr; cpIt = cpIt->pNext)
			{
				resolveAABBStaticBodyContact(it->item, cpIt->item, dt);
			}
		}
	}

	void simulate(float dt)
	{
		if (dt > dtCap)
			dt = dtCap;
		simulateAgents(dt);
		simulateBodies(dt);
	}
};

#endif