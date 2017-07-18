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
	
	bool IsTransparent() {return Type == (int)EBlockType::Air;}
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

	Voxel::FBlock &GetBlockAt(FIntVector blockPos)
	{
		return BlockData[blockPos.X + (blockPos.Y << kChunkSizeShift) + (blockPos.Z << (kChunkSizeShift + kChunkSizeShift))];
	}

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
	TArray<FColor>		Colours;
	TArray<FProcMeshTangent> Tangents;

	//TArray<FVector>				Normals;
	//TArray<FProcMeshTangent>	Tangents;

	UProceduralMeshComponent *	MeshComponent;
};

// voxel world
struct FWorld
{
	FIntVector	WorldSizeChunks;	// The world size in chunks
	FIntVector	WorldSizeBlocks;

	TArray<Voxel::FChunk *>	Chunks;
};

}	//namespace Voxel