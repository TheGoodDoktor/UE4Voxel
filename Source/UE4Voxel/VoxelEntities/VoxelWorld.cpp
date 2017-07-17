// Fill out your copyright notice in the Description page of Project Settings.

#include "VoxelWorld.h"
#include "WorldBuilders/WorldBuilderBase.h"
#include "MeshBuilders/MeshBuilderBase.h"
#include "ProceduralMeshComponent.h"

// Sets default values
AVoxelWorld::AVoxelWorld()
	: WorldSize(16,16,16)
	, BlockSize(100.0f)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set screen root
	USceneComponent *pSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("WorldRoot"));
	RootComponent = pSceneRoot;
}

// Called when the game starts or when spawned
void AVoxelWorld::BeginPlay()
{
	Super::BeginPlay();

	// calculate number of chunks we need for given world size
	const FIntVector worldSizeChunks(
		(WorldSize.X >> Voxel::FChunk::kChunkSizeShift) + 1, 
		(WorldSize.Z >> Voxel::FChunk::kChunkSizeShift) + 1, 
		(WorldSize.Z >> Voxel::FChunk::kChunkSizeShift) + 1
	);
	World.WorldSizeChunks = worldSizeChunks;

	// calculate world size in blocks
	World.WorldSizeBlocks.X = Voxel::FChunk::kChunkSize * worldSizeChunks.X;
	World.WorldSizeBlocks.Y = Voxel::FChunk::kChunkSize * worldSizeChunks.Y;
	World.WorldSizeBlocks.Z = Voxel::FChunk::kChunkSize * worldSizeChunks.Z;

	// fill up the chunk pointer array
	const int noWorldChunks = worldSizeChunks.X * worldSizeChunks.Y * worldSizeChunks.Z;
	World.Chunks.Reset();
	World.Chunks.AddZeroed(noWorldChunks);

	// Create world builder
	if (WorldBuilderClass != nullptr)
	{
		WorldBuilder = NewObject<UWorldBuilderBase>(this, WorldBuilderClass);
		WorldBuilder->RegisterComponent();
	}

	// Create mesh builder
	if (MeshBuilderClass != nullptr)
	{
		MeshBuilder = NewObject<UMeshBuilderBase>(this, MeshBuilderClass);
		MeshBuilder->RegisterComponent();
	}

	// Build World
	if (WorldBuilder != nullptr)
	{
		for (int x = 0; x < worldSizeChunks.X; x++)
		{
			for (int y = 0; y < worldSizeChunks.Y; y++)
			{
				for (int z = 0; z < worldSizeChunks.Z; z++)
				{
					Voxel::FChunk* pChunk = CreateChunkAt(FIntVector(x, y, z));
					WorldBuilder->BuildWorldChunk(pChunk);
				}
			}
		}
	}
}

// Called every frame
void AVoxelWorld::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Rebuild dirty chunks
	if (DirtyChunks.Num() > 0)
	{
		for (Voxel::FChunk* pChunk : DirtyChunks)
		{
			MeshBuilder->BuildMeshForChunk(pChunk);
			// TODO: setup mesh component
			TArray<FVector> normals;
			TArray<FColor> colours; 
			TArray<FProcMeshTangent> tangents;
			
			pChunk->MeshComponent->CreateMeshSection(0, pChunk->Vertices, pChunk->Triangles, normals, pChunk->UVs, colours, tangents,true);

			pChunk->IsDirty = false;
		}

		DirtyChunks.Empty();
	}

}

const Voxel::FChunk*	AVoxelWorld::GetChunkAt(FIntVector chuckPos) const
{
	return World.Chunks[chuckPos.X + (chuckPos.Y * World.WorldSizeChunks.X) + (chuckPos.Z * World.WorldSizeChunks.X * World.WorldSizeChunks.Y)];
}

Voxel::FChunk*	AVoxelWorld::GetChunkAt(FIntVector chuckPos)
{
	return World.Chunks[chuckPos.X + (chuckPos.Y * World.WorldSizeChunks.X) + (chuckPos.Z * World.WorldSizeChunks.X * World.WorldSizeChunks.Y)];
}

Voxel::FChunk*	AVoxelWorld::CreateChunkAt(FIntVector chunkPos)
{
	Voxel::FChunk* pChunk = GetChunkAt(chunkPos);
	if (pChunk != nullptr)
		return pChunk;

	const int chunkSize = Voxel::FChunk::kChunkSize;
	pChunk = new Voxel::FChunk;
	pChunk->ChunkPos = chunkPos;
	pChunk->WorldPos = chunkPos * chunkSize;
	pChunk->World = &World;
	pChunk->BlockData = new Voxel::FBlock[chunkSize * chunkSize * chunkSize];	// TODO: use our own allocator
	World.Chunks[chunkPos.X + (chunkPos.Y * World.WorldSizeChunks.X) + (chunkPos.Z * World.WorldSizeChunks.X * World.WorldSizeChunks.Y)] = pChunk;

	// Also create mesh component
	pChunk->MeshComponent = NewObject<UProceduralMeshComponent>(this);
	pChunk->MeshComponent->RegisterComponent();
	pChunk->MeshComponent->AttachToComponent(RootComponent,FAttachmentTransformRules::KeepRelativeTransform);
	pChunk->MeshComponent->SetRelativeLocation(FVector(pChunk->WorldPos) * BlockSize);
	return pChunk;
}

bool AVoxelWorld::AddDirtyChunk(Voxel::FChunk *pChunk)
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
bool AVoxelWorld::IsPosOutsideWorld(FIntVector voxelPos) const
{
	if (voxelPos.X < 0 || voxelPos.Y < 0 || voxelPos.Z < 0 ||
		voxelPos.X >= World.WorldSizeBlocks.X || voxelPos.Y >= World.WorldSizeBlocks.Y || voxelPos.Z >= World.WorldSizeBlocks.Z)
		return true;
	else
		return false;
}

// get a block at a given position in voxel coordinates
// return outside block if there is no chunk or we are outside the world
const Voxel::FBlock &AVoxelWorld::GetBlockAt(FIntVector voxelPos) const
{
	if (IsPosOutsideWorld(voxelPos))
		return OutsideBlock;

	const FIntVector chunkPos(voxelPos.X >> Voxel::FChunk::kChunkSizeShift, voxelPos.Y >> Voxel::FChunk::kChunkSizeShift, voxelPos.Z >> Voxel::FChunk::kChunkSizeShift);

	const Voxel::FChunk* pChunk = GetChunkAt(chunkPos);
	if (pChunk == nullptr)
		return OutsideBlock;

	const FIntVector blockPos(voxelPos.X & Voxel::FChunk::kChunkSizeMask, voxelPos.Y & Voxel::FChunk::kChunkSizeMask, voxelPos.Z & Voxel::FChunk::kChunkSizeMask);
	return GetBlockFromChunk(pChunk, blockPos);
}

bool AVoxelWorld::SetBlockAt(FIntVector voxelPos, Voxel::FBlock block)
{
	if (IsPosOutsideWorld(voxelPos))
		return false;

	const FIntVector chunkPos(voxelPos.X >> Voxel::FChunk::kChunkSizeShift, voxelPos.Y >> Voxel::FChunk::kChunkSizeShift, voxelPos.Z >> Voxel::FChunk::kChunkSizeShift);
	Voxel::FChunk* pChunk = GetChunkAt(chunkPos);

	// If there is no chunk at the position then create one
	if (pChunk == nullptr)
		pChunk = CreateChunkAt(chunkPos);

	const FIntVector blockPos(voxelPos.X & Voxel::FChunk::kChunkSizeMask, voxelPos.Y & Voxel::FChunk::kChunkSizeMask, voxelPos.Z & Voxel::FChunk::kChunkSizeMask);
	GetBlockFromChunk(pChunk, blockPos) = block;

	// Mark chunk to have its mesh rebuilt
	AddDirtyChunk(pChunk);

	// If we are on a block edge then make neighbour dirty too

	// X Edges
	if (blockPos.X == 0 && chunkPos.X > 0)
	{
		Voxel::FChunk* pNeighbourChunk = GetChunkAt(chunkPos + FIntVector(-1, 0, 0));
		if (pNeighbourChunk != nullptr)
			AddDirtyChunk(pNeighbourChunk);
	}
	if (blockPos.X == Voxel::FChunk::kChunkSize - 1 && chunkPos.X < World.WorldSizeChunks.X - 1)
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
	if (blockPos.Y == Voxel::FChunk::kChunkSize - 1 && chunkPos.Y < World.WorldSizeChunks.Y - 1)
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
	if (blockPos.Z == Voxel::FChunk::kChunkSize - 1 && chunkPos.Z < World.WorldSizeChunks.Z - 1)
	{
		Voxel::FChunk* pNeighbourChunk = GetChunkAt(chunkPos + FIntVector(0, 0, 1));
		if (pNeighbourChunk != nullptr)
			AddDirtyChunk(pNeighbourChunk);
	}

	return true;
}

