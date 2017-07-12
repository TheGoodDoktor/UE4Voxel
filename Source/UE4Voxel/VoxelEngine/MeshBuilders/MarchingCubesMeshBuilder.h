#pragma once

#include "IMeshBuilder.h"

namespace Voxel
{

class FMarchingCubesMeshBuilder : public IMeshBuilder
{
public:
	// IMeshBuilder interface
	virtual void Init(FVoxelEngine *pEngine) override;
	virtual void BuildMeshForChunk(FChunk* pChunk) override;
	// \IMeshBuilder interface

	int BuildMeshForBlock(const FIntVector& worldPos, const FVector& chunkPos, FChunk* pChunk, int index);
	void CalculateDensitiesForBlock(FIntVector worldPos, float density[]);

	FVoxelEngine	*Engine;
	int		ChunkSizeBlocks;
	float	BlockSize;	// size of block in world units
};

}//namespace Voxel