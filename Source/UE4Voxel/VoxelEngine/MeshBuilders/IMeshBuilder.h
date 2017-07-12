#pragma once

class FVoxelEngine;

namespace Voxel
{
	
struct FChunk;
struct FProcMeshSection;

class IMeshBuilder
{
public:
	virtual void Init(FVoxelEngine *pEngine) = 0;
	virtual void BuildMeshForChunk(FChunk* pChunk) = 0;
};
	
}