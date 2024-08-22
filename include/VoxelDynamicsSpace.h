#ifndef VOXEL_DYNAMICS_SPACE
#define VOXEL_DYNAMICS_SPACE

#include "VoxelDynamicsCollider.h"
#include "VoxelDynamicsAllocator.h"
#include <vector>

struct VDGrid;
struct VDVoxel
{
	VDVector3i offsets;
	bool occupied;
	VDVector3 lowPosition;
	VDPointer pointer;
	VDuint index;
	VDuint chunkIndex;
	VDList<VDPointer> colliders;

	VDVoxel()
	{
		offsets = VDVector3i();
		lowPosition = VDVector3();
		occupied = false;
		pointer = NULL;
		index = -1;
		colliders = VDList<VDPointer>();
		chunkIndex = 0;
	}

	VDVoxel(VDVector3i _offsets, bool _occupied, VDVector3 _lowPosition, VDPointer _pointer, VDuint _index, VDuint _chunkIndex)
	{
		offsets = _offsets;
		occupied = _occupied;
		lowPosition = _lowPosition;
		pointer = _pointer;
		index = _index;
		colliders = VDList<VDPointer>();
		chunkIndex = _chunkIndex;
	}


	VDVoxel& operator=(const VDVoxel& other)
	{
		if (this != &other)
		{
			offsets = other.offsets;
			occupied = other.occupied;
			lowPosition = other.lowPosition;
			pointer = other.pointer;
			index = other.index;
			colliders = other.colliders;
			chunkIndex = other.chunkIndex;
		}
		return *this;
	}

	VDVector3 midPoint() const
	{
		return lowPosition + VDVector3::half();
	}


	VDAABB toAABB() const
	{
		return VDAABB(lowPosition, lowPosition + VDVector3::one());
	}

	VDAABBContact voxelContact(VDAABB& aabb)
	{
		VDAABB voxelAABB = toAABB();
		VDAABB intersection;
		aabb.intersectionRegion(voxelAABB, intersection);
		VDVector3 quadrantDir = VDSign(aabb.position - voxelAABB.position);
		return VDAABBContact((VDPointer)&aabb, (VDPointer)this, intersection, quadrantDir);
	}
};


struct VDGrid
{
	VDuint gridSize;
	VDVoxel* voxels;
	VDVector3 low;
	VDuint indexCount;
	VDList<VDVoxel*> occupiedVoxels;
	VDuint chunkIndex;

	VDuint getIndex(VDuint lvx, VDuint lvy, VDuint lvz) const
	{
		return lvx + lvy * gridSize + lvz * gridSize * gridSize;
	}

	bool validateCoords(VDVector3i coords) const
	{
		if (coords.x >= 0 && coords.x < gridSize && coords.y >= 0 && coords.y < gridSize && coords.z >= 0 && coords.z < gridSize)
			return true;
		return false;
	}

	VDVector3i getCoordinates(VDuint index) const
	{
		VDVector3i coords;
		coords.z = index / (gridSize * gridSize);
		coords.y = (index % (gridSize * gridSize)) / gridSize;
		coords.x = index % gridSize;
		return coords;
	}


	VDGrid(VDuint _chunkSize, VDVector3 low, VDuint _chunkIndex)
	{
		gridSize = _chunkSize;
		indexCount = gridSize * gridSize * gridSize;
		voxels = new VDVoxel[indexCount];
		for (VDuint x = 0; x < gridSize; ++x)
		{
			for (VDuint y = 0; y < gridSize; ++y)
			{
				for (VDuint z = 0; z < gridSize; ++z)
				{
					VDuint index = getIndex(x, y, z);
					VDVector3i offsets = VDVector3i(x, y, z);
					voxels[index] = VDVoxel(offsets, false, low + VDVector3(offsets), NULL, index, _chunkIndex);
				}
			}
		}
		//aabb = VDAABB(low, low + VDVector3((float)gridSize, (float)gridSize, (float)gridSize));
		this->low = low;
		chunkIndex = _chunkIndex;
		occupiedVoxels = VDList<VDVoxel*>();
	}

	VDGrid() : VDGrid::VDGrid(25, VDVector3(), 0)
	{
		
	}

	VDuint getIndex(VDVector3 position) const
	{
		VDVector3 localPosition = position - low;
		VDVector3i lpi = VDVector3i(localPosition);
		return getIndex(lpi.x, lpi.y, lpi.z);
	}

	bool getOccupied(VDVector3 position) const
	{
		VDuint index = getIndex(position);
		if (index < indexCount)
			return voxels[index].occupied;
	}

	bool getOccupied(VDuint lvx, VDuint lvy, VDuint lvz) const
	{
		VDuint index = getIndex(lvx, lvy, lvz);
		if (index < indexCount)
			return voxels[index].occupied;
	}

	bool getOccupied(VDuint index) const
	{
		if (index < indexCount)
			return voxels[index].occupied;
	}

	VDVoxel getVoxel(VDVector3 position)
	{
		VDuint index = getIndex(position);
		if (index < indexCount)
			return voxels[index];
	}

	VDVoxel getVoxel(VDuint lvx, VDuint lvy, VDuint lvz)
	{
		VDuint index = getIndex(lvx, lvy, lvz);
		if (index < indexCount)
			return voxels[index];
	}

	VDVoxel getVoxel(VDuint index)
	{
		if (index < indexCount)
			return voxels[index];
	}



	void setOccupied(VDuint index)
	{
		if (index < indexCount && !voxels[index].occupied)
		{
			voxels[index].occupied = true;
			occupiedVoxels.insert(&voxels[index]);
		}
	}

	void setOccupied(VDVector3 position)
	{
		VDuint index = getIndex(position);
		setOccupied(index);
	}

	void setOccupied(VDuint lvx, VDuint lvy, VDuint lvz)
	{
		VDuint index = getIndex(lvx, lvy, lvz);
		setOccupied(index);
	}

	void setVoxel(VDuint index, VDVoxel voxel)
	{
		if (index < indexCount)
		{
			voxels[index] = voxel;
		}
	}

	VDuint moveIndex(VDuint index, VDDirection direction) const
	{
		VDVector3i coords = getCoordinates(index);
		switch (direction)
		{
		case VDDirection::RIGHT:
			coords.x += 1;
			break;
		case VDDirection::UP:
			coords.y += 1;
			break;
		case VDDirection::FORWARD:
			coords.z += 1;
			break;
		case VDDirection::LEFT:
			coords.x -= 1;
			break;
		case VDDirection::DOWN:
			coords.y -= 1;
			break;
		case VDDirection::BACK:
			coords.z -= 1;
			break;
		}
		if (!validateCoords(coords))
			return -1;
		return getIndex(coords.x, coords.y, coords.z);
	}

	VDList<VDVoxel*> sampleOccupiedRegion(VDAABB aabb) const
	{
		VDList<VDVoxel*> occupiedVoxels;
		VDVector3 low = aabb.low - this->low;
		if (low.x > gridSize || low.y > gridSize || low.z > gridSize)
			return occupiedVoxels;
		VDVector3 high = aabb.high - this->low;
		if (high.x < 0.0f || high.y < 0.0f || high.z < 0.0f)
			return occupiedVoxels;

		VDVector3i lowInd = VDVector3i(low);
		VDVector3i highInd = VDVector3i(high);
		VDVector3i anchor = VDVector3i(this->low);

		for (int x = lowInd.x; x <= highInd.x; ++x)
		{
			for (int y = lowInd.y; y <= highInd.y; ++y)
			{
				for (int z = lowInd.z; z <= highInd.z; ++z)
				{
					if (x >= 0 && y >= 0 && z >= 0 && x < gridSize && y < gridSize && z < gridSize)
					{
						VDuint index = getIndex(x, y, z);
						if (index < indexCount && voxels[index].occupied)
						{
							occupiedVoxels.insert(&voxels[index]);
						}
					}
				}
			}
		}
		return occupiedVoxels;
	}

	VDList<VDVoxel*> sampleRegion(VDAABB aabb) const
	{
		VDList<VDVoxel*> occupiedVoxels;
		VDVector3 low = aabb.low - this->low;
		if (low.x > gridSize || low.y > gridSize || low.z > gridSize)
			return occupiedVoxels;
		VDVector3 high = aabb.high - this->low;
		if (high.x < 0.0f || high.y < 0.0f || high.z < 0.0f)
			return occupiedVoxels;

		VDVector3i lowInd = VDVector3i(low);
		VDVector3i highInd = VDVector3i(high);
		VDVector3i anchor = VDVector3i(this->low);

		for (int x = lowInd.x; x <= highInd.x; ++x)
		{
			for (int y = lowInd.y; y <= highInd.y; ++y)
			{
				for (int z = lowInd.z; z <= highInd.z; ++z)
				{
					if (x >= 0 && y >= 0 && z >= 0 && x < gridSize && y < gridSize && z < gridSize)
					{
						VDuint index = getIndex(x, y, z);
						if (index < indexCount)
						{
							occupiedVoxels.insert(&voxels[index]);
						}
					}
				}
			}
		}
		return occupiedVoxels;
	}

	void sampleOccupiedRegion(VDAABB aabb, VDList<VDVoxel*>& occupiedVoxels, VDList<VDPointer>& uniqueColliders) const
	{
		VDVector3 low = aabb.low - this->low;
		if (low.x > gridSize || low.y > gridSize || low.z > gridSize)
			return;
		VDVector3 high = aabb.high - this->low;
		if (high.x < 0.0f || high.y < 0.0f || high.z < 0.0f)
			return;

		VDVector3i lowInd = VDVector3i(low);
		VDVector3i highInd = VDVector3i(high);
		VDVector3i anchor = VDVector3i(this->low);

		for (int x = lowInd.x; x <= highInd.x; ++x)
		{
			for (int y = lowInd.y; y <= highInd.y; ++y)
			{
				for (int z = lowInd.z; z <= highInd.z; ++z)
				{
					if (x >= 0 && y >= 0 && z >= 0 && x < gridSize && y < gridSize && z < gridSize)
					{
						VDuint index = getIndex(x, y, z);
						if (index < indexCount)
						{
							if (voxels[index].colliders.count > 0)
							{
								for (auto it = voxels[index].colliders.pFirst; it != nullptr; it = it->pNext)
								{
									uniqueColliders.insertSortedUnique(it->item);
								}
							}
							if(voxels[index].occupied)
								occupiedVoxels.insert(&voxels[index]);
						}
					}
				}
			}
		}
	}

	void insertCollider(VDCollider& collider, VDList<VDPointer>* occupiedVoxels)
	{
		VDList<VDVoxel*> sampled = sampleRegion(collider);
		for (auto it = sampled.pFirst; it != nullptr; it = it->pNext)
		{
			it->item->colliders.insertSortedUnique(&collider);
			occupiedVoxels->insertSortedUnique((VDPointer)it->item);
		}
		sampled.free();
	}

	void removeCollider(VDCollider& collider, VDList<VDPointer>* occupiedVoxels)
	{
		for (auto it = occupiedVoxels->pFirst; it != nullptr; it = it->pNext)
		{
			VDVoxel* pVoxel = (VDVoxel*)it->item;
			pVoxel->colliders.removeSorted(&collider);
		}
		occupiedVoxels->free();
	}

	void updateCollider(VDCollider& collider, VDList<VDPointer>* occupiedVoxels)
	{
		removeCollider(collider, occupiedVoxels);
		insertCollider(collider, occupiedVoxels);
	}

};

struct VDSpace
{
	struct VDChunkOccupation
	{
		bool occupied = false;
		VDGrid* pChunk;

		VDChunkOccupation()
		{
			occupied = false;
			pChunk = nullptr;
		}

		VDChunkOccupation& operator=(const VDChunkOccupation& other)
		{
			if (this != &other)
			{
				occupied = other.occupied;
				pChunk = other.pChunk;
			}

			return *this;
		}
	};

	VDChunkOccupation* grids;
	VDuint gridSize;
	VDVector3i anchor;
	VDuint horizontalGrids;
	VDuint verticalGrids;

	VDSpace()
	{
		grids = nullptr;
		gridSize = 0;
		anchor = VDVector3i();
		horizontalGrids = 0;
		verticalGrids = 0;
	}
	VDSpace(VDuint _chunkSize, VDVector3i _anchor, VDuint _horizontalChunks, VDuint _verticalChunks)
	{
		gridSize = _chunkSize;
		anchor = _anchor;
		horizontalGrids = _horizontalChunks;
		verticalGrids = _verticalChunks;
		grids = new VDChunkOccupation[horizontalGrids * horizontalGrids * verticalGrids];
		for (VDuint z = 0; z < horizontalGrids; z++)
		{
			for (VDuint y = 0; y < verticalGrids; y++)
			{
				for (VDuint x = 0; x < horizontalGrids; x++)
				{
					VDuint index = x + y * horizontalGrids + z * horizontalGrids * verticalGrids;
					grids[index] = VDChunkOccupation();
				}
			}
		}
	}

	bool validateChunkCoord(VDVector3i chunkCoord) const
	{
		if (chunkCoord.x >= 0 && chunkCoord.x < horizontalGrids)
		{
			if (chunkCoord.y >= 0 && chunkCoord.y < verticalGrids)
			{
				if (chunkCoord.z >= 0 && chunkCoord.z < horizontalGrids)
				{
					return true;
				}
			}
		}
		return false;
	}

	VDuint getIndex(VDVector3i chunkCoord) const 
	{
		return chunkCoord.x + chunkCoord.y * horizontalGrids + chunkCoord.z * horizontalGrids * verticalGrids;
	}

	VDVector3i getChunkLow(VDVector3i chunkCoord) const
	{
		return anchor + chunkCoord * gridSize;
	}

	void setChunkOccupied(VDVector3i chunkCoord)
	{
		if (validateChunkCoord(chunkCoord))
		{
			VDuint index = getIndex(chunkCoord);
			grids[index].occupied = true;
			grids[index].pChunk = new VDGrid(gridSize, getChunkLow(chunkCoord), index);
		}
	}

	void insertChunk(VDVector3i chunkCoord, VDGrid chunk)
	{
		if (validateChunkCoord(chunkCoord))
		{
			VDuint index = getIndex(chunkCoord);
			if (!grids[index].occupied)
			{
				grids[index].occupied = true;
				grids[index].pChunk = new VDGrid(gridSize, getChunkLow(chunkCoord), index);
			}
			*grids[index].pChunk = chunk;
		}
	}

	VDList<VDChunkOccupation*> sampleChunkOccupations(VDAABB aabb)
	{
		VDList<VDChunkOccupation*> chunkOccupations;
		VDVector3 low = aabb.low - anchor;
		if (low.x > gridSize*horizontalGrids || low.y > gridSize*verticalGrids || low.z > gridSize*horizontalGrids)
			return chunkOccupations;
		VDVector3 high = aabb.high - anchor;
		if (high.x < 0.0f || high.y < 0.0f || high.z < 0.0f)
			return chunkOccupations;

		VDVector3i lowInd = VDVector3i(low)/gridSize;
		VDVector3i highInd = VDVector3i(high)/gridSize;

		for (int x = lowInd.x; x <= highInd.x; ++x)
		{
			for (int y = lowInd.y; y <= highInd.y; ++y)
			{
				for (int z = lowInd.z; z <= highInd.z; ++z)
				{
					VDVector3i chunkCoord(x, y, z);
					if (validateChunkCoord(chunkCoord))
					{
						chunkOccupations.insert(&grids[getIndex(chunkCoord)]);
					}
				}
			}
		}
		return chunkOccupations;
	}

	VDList<VDGrid*> sampleChunks(VDAABB aabb) const
	{
		VDList<VDGrid*> sampled;
		VDVector3 low = aabb.low - anchor;
		if (low.x > gridSize * horizontalGrids || low.y > gridSize * verticalGrids || low.z > gridSize * horizontalGrids)
			return sampled;
		VDVector3 high = aabb.high - anchor;
		if (high.x < 0.0f || high.y < 0.0f || high.z < 0.0f)
			return sampled;

		VDVector3i lowInd = VDVector3i(low) / gridSize;
		VDVector3i highInd = VDVector3i(high) / gridSize;

		for (int x = lowInd.x; x <= highInd.x; ++x)
		{
			for (int y = lowInd.y; y <= highInd.y; ++y)
			{
				for (int z = lowInd.z; z <= highInd.z; ++z)
				{
					VDVector3i chunkCoord(x, y, z);
					if (validateChunkCoord(chunkCoord))
					{
						VDuint index = getIndex(chunkCoord);
						if(grids[index].occupied)
							sampled.insert(grids[index].pChunk);
					}
				}
			}
		}
		return sampled;
	}

	void sampleOccupiedRegion(VDAABB aabb, VDList<VDVoxel*>& occupiedVoxels, VDList<VDPointer>& uniqueColliders) const
	{
		VDList<VDGrid*> sampledChunks = sampleChunks(aabb);
		for (auto chunkIt = sampledChunks.pFirst; chunkIt != nullptr; chunkIt = chunkIt->pNext)
		{
			chunkIt->item->sampleOccupiedRegion(aabb, occupiedVoxels, uniqueColliders);
		}
		sampledChunks.free();
	}

	void insertCollider(VDCollider& collider)
	{
		VDList<VDGrid*> sampled = sampleChunks(collider);
		for (auto it = sampled.pFirst; it != nullptr; it = it->pNext)
		{
			VDList<VDPointer>* occupiedVoxels = collider.occupiedChunks.insert(VDList<VDPointer>());
			occupiedVoxels->id = it->item->chunkIndex;
			it->item->insertCollider(collider, occupiedVoxels);
		}
		sampled.free();
	}

	void removeCollider(VDCollider& collider)
	{
		for (auto it = collider.occupiedChunks.pFirst; it != nullptr; it = it->pNext)
		{
			VDGrid* pChunk = grids[it->item.id].pChunk;
			pChunk->removeCollider(collider, &it->item);
		}
		collider.occupiedChunks.free();
	}

	void updateCollider(VDCollider& collider)
	{
		removeCollider(collider);
		insertCollider(collider);
	}

	VDVector3i moveIndex(VDuint& index, VDVector3i& chunkCoord, VDDirection direction) const
	{
		VDuint chunkIndex = getIndex(chunkCoord);
		VDuint chunkMax = horizontalGrids * horizontalGrids * verticalGrids;
		if (chunkIndex < chunkMax)
			return VDVector3i(-1, -1, -1);
		if(!grids[chunkIndex].occupied)
			return VDVector3i(-1, -1, -1);
		VDVector3i voxCoord = grids[chunkIndex].pChunk->getCoordinates(index);
		switch (direction)
		{
		case VDDirection::RIGHT:
			voxCoord.x += 1;
			break;
		case VDDirection::UP:
			voxCoord.y += 1;
			break;
		case VDDirection::FORWARD:
			voxCoord.z += 1;
			break;
		case VDDirection::LEFT:
			voxCoord.x -= 1;
			break;
		case VDDirection::DOWN:
			voxCoord.y -= 1;
			break;
		case VDDirection::BACK:
			voxCoord.z -= 1;
			break;
		}
		if (voxCoord.x < 0)
		{
			chunkCoord.x -= 1;
			voxCoord.x = gridSize - 1;
		}
		if (voxCoord.x >= gridSize)
		{
			chunkCoord.x += 1;
			voxCoord.x = 0;
		}
		if (voxCoord.y < 0)
		{
			chunkCoord.y -= 1;
			voxCoord.y = gridSize - 1;
		}
		if (voxCoord.y >= gridSize)
		{
			chunkCoord.y += 1;
			voxCoord.y = 0;
		}
		if (voxCoord.z < 0)
		{
			chunkCoord.z -= 1;
			voxCoord.z = gridSize - 1;
		}
		if (voxCoord.z >= gridSize)
		{
			chunkCoord.z += 1;
			voxCoord.z = 0;
		}
		return voxCoord;
	}

	VDVector3i getCoordinates(VDuint index) const
	{
		VDVector3i coords;
		coords.z = index / (horizontalGrids * verticalGrids);
		coords.y = (index % (horizontalGrids * verticalGrids)) / horizontalGrids;
		coords.x = index % horizontalGrids;
		return coords;
	}

	void setVoxelOccupied(VDVector3 worldPosition)
	{
		VDVector3 local = worldPosition - anchor;
		VDVector3i gridCoord = VDVector3i(local) / gridSize;
		if (validateChunkCoord(gridCoord))
		{
			VDuint index = getIndex(gridCoord);
			if (grids[index].occupied)
			{
				grids[index].pChunk->setOccupied(worldPosition);
				
			}
		}
	}
};

#endif