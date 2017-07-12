#include "VoxelEngine.h"
#include "Meshbuilders/MarchingCubesMeshBuilder.h"

bool	FVoxelEngine::InitialiseWorld(FIntVector worldSizeVoxels)
{
	// calculate number of chunks we need for given worldsize
	const FIntVector worldSizeChunks((worldSizeVoxels.X >> kChunkSizeShift) + 1, (worldSizeVoxels.Z >> kChunkSizeShift) + 1, (worldSizeVoxels.Z >> kChunkSizeShift) + 1);
	World.WorldSizeChunks = worldSizeChunks;

	// calculate world size in blocks
	World.WorldSizeBlocks.X = kChunkSize * worldSizeChunks.X;
	World.WorldSizeBlocks.Y = kChunkSize * worldSizeChunks.Y;
	World.WorldSizeBlocks.Z = kChunkSize * worldSizeChunks.Z;

	// fill up the chunk pointer array
	const int noWorldChunks = worldSizeChunks.X * worldSizeChunks.Y * worldSizeChunks.Z;
	World.Chunks.Reset();
	World.Chunks.AddZeroed(noWorldChunks);

	// TODO: Call world builder

	// TODO: setup mesh builder
	MeshBuilder = new Voxel::FMarchingCubesMeshBuilder;
	MeshBuilder->Init(this);

	return true;
}

const Voxel::FChunk*	FVoxelEngine::GetChunkAt(FIntVector chuckPos) const
{
	return World.Chunks[chuckPos.X + (chuckPos.Y * World.WorldSizeChunks.X) + (chuckPos.Z * World.WorldSizeChunks.X * World.WorldSizeChunks.Y)];
}

Voxel::FChunk*	FVoxelEngine::GetChunkAt(FIntVector chuckPos)
{
	return World.Chunks[chuckPos.X + (chuckPos.Y * World.WorldSizeChunks.X) + (chuckPos.Z * World.WorldSizeChunks.X * World.WorldSizeChunks.Y)];
}

Voxel::FChunk*	FVoxelEngine::CreateChunkAt(FIntVector chunkPos)
{
	Voxel::FChunk* pChunk = GetChunkAt(chunkPos);
	if (pChunk != nullptr)
		return pChunk;

	pChunk = new Voxel::FChunk;
	pChunk->ChunkPos = chunkPos;
	pChunk->World = &World;
	pChunk->BlockData = new Voxel::FBlock[kChunkSize * kChunkSize * kChunkSize];	// TODO: use our own allocator
	World.Chunks[chunkPos.X + (chunkPos.Y * World.WorldSizeChunks.X) + (chunkPos.Z * World.WorldSizeChunks.X * World.WorldSizeChunks.Y)] = pChunk;
	return pChunk;
}

bool FVoxelEngine::AddDirtyChunk(Voxel::FChunk *pChunk)
{
	check(pChunk != nullptr);

	if (pChunk->IsDirty)	// chunk is already marked
		return false;

	// mark & add
	pChunk->IsDirty = true;
	DirtyChunks.Add(pChunk);
	return true;
}

// Is a given voxel position outside of the world?
bool FVoxelEngine::IsPosOutsideWorld(FIntVector voxelPos) const
{
	if (voxelPos.X < 0 || voxelPos.Y < 0 || voxelPos.Z < 0 ||
		voxelPos.X >= World.WorldSizeBlocks.X || voxelPos.Y >= World.WorldSizeBlocks.Y || voxelPos.Z >= World.WorldSizeBlocks.Z)
		return false;
	else
		return true;
}

// get a block at a given position in voxel coordinates
// return outside block if there is no chunk or we are outside the world
Voxel::FBlock &FVoxelEngine::GetBlockAt(FIntVector voxelPos) 
{
	if (IsPosOutsideWorld(voxelPos))
		return OutsideBlock;

	FIntVector chunkPos(voxelPos.X >> kChunkSizeShift,voxelPos.Y >> kChunkSizeShift, voxelPos.Z >> kChunkSizeShift);
	
	Voxel::FChunk* pChunk = GetChunkAt(chunkPos);
	if (pChunk == nullptr)
		return OutsideBlock;
	
	FIntVector blockPos(voxelPos.X & kChunkSizeMask,voxelPos.Y & kChunkSizeMask, voxelPos.Z & kChunkSizeMask);
	return GetBlockFromChunk(pChunk, blockPos);
}

bool FVoxelEngine::SetBlockAt(FIntVector voxelPos, Voxel::FBlock block)
{
	if (IsPosOutsideWorld(voxelPos))
		return false;

	FIntVector chunkPos(voxelPos.X >> kChunkSizeShift, voxelPos.Y >> kChunkSizeShift, voxelPos.Z >> kChunkSizeShift);
	Voxel::FChunk* pChunk = GetChunkAt(chunkPos);
	
	// If there is no chunk at the position then create one
	if (pChunk == nullptr)
		pChunk = CreateChunkAt(chunkPos);
	
	FIntVector blockPos(voxelPos.X & kChunkSizeMask, voxelPos.Y & kChunkSizeMask, voxelPos.Z & kChunkSizeMask);
	GetBlockFromChunk(pChunk, blockPos) = block;

	// Mark chunk to have its mesh rebuilt
	AddDirtyChunk(pChunk);

	// If we are on a block edge then make neighbour dirty too

	// X Edges
	if (blockPos.X == 0 && chunkPos.X > 0)
	{
		Voxel::FChunk* pNeighbourChunk = GetChunkAt(chunkPos + FIntVector(-1,0,0));
		if(pNeighbourChunk != nullptr)
			AddDirtyChunk(pNeighbourChunk);
	}
	if (blockPos.X == kChunkSize-1 && chunkPos.X < World.WorldSizeChunks.X - 1)
	{
		Voxel::FChunk* pNeighbourChunk = GetChunkAt(chunkPos + FIntVector(1, 0, 0));
		if (pNeighbourChunk != nullptr)
			AddDirtyChunk(pNeighbourChunk);
	}

	// Y Edges
	if (blockPos.Y == 0 && chunkPos.Y > 0)
	{
		Voxel::FChunk* pNeighbourChunk = GetChunkAt(chunkPos + FIntVector(0, -1, 0));
		if (pNeighbourChunk != nullptr)
			AddDirtyChunk(pNeighbourChunk);
	}
	if (blockPos.Y == kChunkSize - 1 && chunkPos.Y < World.WorldSizeChunks.Y - 1)
	{
		Voxel::FChunk* pNeighbourChunk = GetChunkAt(chunkPos + FIntVector(0, 1, 0));
		if (pNeighbourChunk != nullptr)
			AddDirtyChunk(pNeighbourChunk);
	}

	// Z Edges
	if (blockPos.Z == 0 && chunkPos.Z > 0)
	{
		Voxel::FChunk* pNeighbourChunk = GetChunkAt(chunkPos + FIntVector(0, 0, -1));
		if (pNeighbourChunk != nullptr)
			AddDirtyChunk(pNeighbourChunk);
	}
	if (blockPos.Z == kChunkSize - 1 && chunkPos.Z < World.WorldSizeChunks.Z - 1)
	{
		Voxel::FChunk* pNeighbourChunk = GetChunkAt(chunkPos + FIntVector(0, 0, 1));
		if (pNeighbourChunk != nullptr)
			AddDirtyChunk(pNeighbourChunk);
	}

	return true;
}

