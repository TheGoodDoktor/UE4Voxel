// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VoxelTypes.h"
#include "VoxelWorld.generated.h"

UCLASS()
class UE4VOXEL_API AVoxelWorld : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AVoxelWorld();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	// public interface should only be dealing with voxel coords - chunks are implementation details
	bool			IsPosOutsideWorld(FIntVector pos) const;
	Voxel::FBlock&	GetBlockAt(FIntVector voxelPos);
	bool			SetBlockAt(FIntVector voxelPos, Voxel::FBlock block);

	int32			GetChunkSizeInBlocks() const { return kChunkSize; }
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// World size in voxels
	UPROPERTY(EditAnywhere)
	FIntVector	WorldSize;

	// How big each block should be in world units
	UPROPERTY(EditAnywhere)
	float BlockSize;

	// Which mesh builder to use
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UMeshBuilderBase>	MeshBuilder;
	
	// Which world builder to use
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UWorldBuilderBase>	WorldBuilder;

	// private data & methods
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
};
