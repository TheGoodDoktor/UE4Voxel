#include "UE4Voxel.h"

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
	Top = 0,
	Bottom,
	North,
	South,
	East,
	West,

	Count
};

// forward decs
struct FWorld;

// base types

// individual voxel
struct FBlock
{
	uint8 Type;
	float Density;
	
	bool IsTransparent() {return Type == (int)EBlockType::Air;}
};

// chunk of voxels - a cubic subsection of the world
struct FChunk
{
	bool		IsDirty;	// chunk is dirty and needs re-meshing
	FIntVector 	WorldPos;	// World position of chunk bottom corner in voxel coords
	FIntVector  ChunkPos;	// position of chunk in world chunk grid 
	FBlock *	BlockData;	// 3D array of blocks
	FWorld *	World;		// Which world this chunk belongs to
	
	// TODO: Add data to produce mesh
};

// voxel world
struct FWorld
{
	FIntVector	WorldSizeChunks;	// The world size in chunks
	FIntVector	WorldSizeBlocks;

	TArray<Voxel::FChunk *>	Chunks;
};

}	//namespace Voxel