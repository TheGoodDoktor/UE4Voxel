#pragma once

#include "UE4Voxel.h"
#include "ProceduralMeshComponent.h"

class UProceduralMeshComponent;

namespace Voxel
{

// enums	
enum class EBlockType : uint8
{
	Air = 0,
	Solid,

	Count
};

enum class EBlockFace : uint8
{
	XP = 0,
	XN,
	YP,
	YN,
	ZP,
	ZN,

	Count
};

// forward decs
struct FWorld;

// base types

// individual voxel
struct FBlock
{
	FBlock() : Type(0), Density(0) {}	// default to empty block

	uint8 Type;
	float Density;
	
	bool IsTransparent() const {return Type == (int)EBlockType::Air;}
};

// chunk of voxels - a cubic subsection of the world
struct FChunk
{
	static const int kChunkSize = 16;
	static const int kChunkSizeShift = 4;
	static const int kChunkSizeMask = 0xf;

	FChunk() 
		: IsDirty(false)
		, BlockData(nullptr)
		, World(nullptr)
		, MeshComponent(nullptr)
	{}

	// Return if a block position within the chunk is valid
	bool ValidBlockPos(FIntVector blockPos) const 
	{
		if (blockPos.X < 0 || blockPos.X >= kChunkSize ||
			blockPos.Y < 0 || blockPos.Y >= kChunkSize ||
			blockPos.Z < 0 || blockPos.Z >= kChunkSize)
			return false;

		return true;
	}

	// Get a pointer to a block in the chunk - returns null if invalid
	const Voxel::FBlock *GetBlockAt(FIntVector blockPos) const 
	{
		if (ValidBlockPos(blockPos))
			return &BlockData[blockPos.X + (blockPos.Y << kChunkSizeShift) + (blockPos.Z << (kChunkSizeShift + kChunkSizeShift))];
		else
			return nullptr;
	}

	// Set a block in the chunk - return false if position is invalid
	bool SetBlockAt(FIntVector blockPos, const Voxel::FBlock &block)
	{
		if (ValidBlockPos(blockPos))
		{
			BlockData[blockPos.X + (blockPos.Y << kChunkSizeShift) + (blockPos.Z << (kChunkSizeShift + kChunkSizeShift))] = block;
			MarkDirty();
			return true;
		}

		return false;
	}

	bool MarkDirty();

	bool		IsDirty;	// chunk is dirty and needs re-meshing
	FIntVector 	WorldPos;	// World position of chunk bottom corner in voxel coords
	FIntVector  ChunkPos;	// position of chunk in world chunk grid 
	FBlock *	BlockData;	// 3D array of blocks
	FWorld *	World;		// Which world this chunk belongs to

	// Data to produce mesh
	TArray<FVector>		Vertices;
	TArray<int32>		Triangles;
	TArray<FVector2D>	UVs;
	TArray<FVector>		Normals;
	TArray<FLinearColor>	Colours;
	TArray<FProcMeshTangent> Tangents;

	UProceduralMeshComponent *	MeshComponent;
};

// voxel world
struct FWorld
{
	void AddDirtyChunk(Voxel::FChunk *pChunk)
	{
		DirtyChunks.Add(pChunk);
	}

	FIntVector	WorldSizeChunks;	// The world size in chunks
	FIntVector	WorldSizeBlocks;

	TArray<Voxel::FChunk *>	Chunks;
	TArray<Voxel::FChunk *>	DirtyChunks;	// list of dirty chunks that need re-meshing
};

}	//namespace Voxel