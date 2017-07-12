#pragma once

#include "VoxelTypes.h"
#include "MeshBuilders/IMeshBuilder.h"
#include "WorldBuilders/IWorldBuilder.h"

// could make this a UObject?
class FVoxelEngine
{
public:
	// public interface should only be dealing with voxel coords - chunks are implementation details
	bool			InitialiseWorld(FIntVector worldSizeVoxels);	
	bool			IsPosOutsideWorld(FIntVector pos) const;
	Voxel::FBlock&	GetBlockAt(FIntVector voxelPos);
	bool			SetBlockAt(FIntVector voxelPos, Voxel::FBlock block);
	
private:
	const int kChunkSize = 16;
	const int kChunkSizeShift = 4;
	const int kChunkSizeMask = 0xf;

	Voxel::FBlock &GetBlockFromChunk(const Voxel::FChunk *pChunk, FIntVector blockPos) 
	{
		return pChunk->BlockData[blockPos.X + (blockPos.Y << kChunkSizeShift) + (blockPos.Y << (kChunkSizeShift + kChunkSizeShift))];
	}

	Voxel::FChunk*			GetChunkAt(FIntVector chuckPos);
	const Voxel::FChunk*	GetChunkAt(FIntVector chuckPos) const;
	Voxel::FChunk*			CreateChunkAt(FIntVector chuckPos);
	bool					AddDirtyChunk(Voxel::FChunk *pChunk);


	Voxel::FWorld			World;
	Voxel::FBlock			OutsideBlock;	// block to use when outside of world

	TArray<Voxel::FChunk *>	DirtyChunks;	// list of dirty chunks that need re-meshing

	Voxel::IMeshBuilder *		MeshBuilder;
	Voxel::IWorldBuilder *		WorldBuilder;
};