#pragma once

#include "IMeshBuilder.h"

namespace Voxel
{

class FMarchingCubesMeshBuilder : public IMeshBuilder
{
public:
	// IMeshBuilder interface
	virtual void BuildMeshForChunk(FChunk* pChunk, FProcMeshSection& meshSection) override;
	// \IMeshBuilder interface

	int BuildMeshForBlock(const FIntVector& worldPos, const FVector& chunkPos, FProcMeshSection& meshSection, int index);
	void CalculateDensitiesForBlock(FIntVector worldPos, float density[]);

	int		ChunkSizeBlocks;
	float	BlockSize;	// size of block in world units
};

}//namespace Voxel