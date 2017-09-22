// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VoxelTypes.h"
#include "VoxelWorld.generated.h"

namespace Voxel
{
	struct FBlock;
}

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
	const Voxel::FBlock*	GetBlockAt(FIntVector voxelPos) const;
	bool			SetBlockAt(FIntVector voxelPos, Voxel::FBlock block);

	int32			GetChunkSizeInBlocks() const { return Voxel::FChunk::kChunkSize; }
	const Voxel::FWorld&	GetVoxelWorld() const { return	World; }

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// World size in chunks
	UPROPERTY(EditAnywhere)
	FIntVector	WorldSizeChunks;

	// How big each block should be in world units
	UPROPERTY(EditAnywhere)
	float BlockSize;

	// Which mesh builder to use
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UMeshBuilderBase>	MeshBuilderClass;
	
	// Which world builder to use
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UWorldBuilderBase>	WorldBuilderClass;

	// private data & methods
private:

	/*const Voxel::FBlock *GetBlockFromChunk(const Voxel::FChunk *pChunk, FIntVector blockPos) const 
	{
		return pChunk->GetBlockAt(blockPos);
	}

	Voxel::FBlock *GetBlockFromChunk(const Voxel::FChunk *pChunk, FIntVector blockPos)
	{
		return const_cast<Voxel::FBlock *>( static_cast<const AVoxelWorld&>(*this).GetBlockFromChunk(pChunk, blockPos));
	}*/

	Voxel::FChunk*			GetChunkAt(FIntVector chuckPos);
	const Voxel::FChunk*	GetChunkAt(FIntVector chuckPos) const;
	Voxel::FChunk*			CreateChunkAt(FIntVector chuckPos);

	class UMeshBuilderBase*		MeshBuilder;
	class UWorldBuilderBase*	WorldBuilder;

	Voxel::FWorld			World;
	Voxel::FBlock			OutsideBlock;	// block to use when outside of world

	TQueue<Voxel::FChunk *>	BuiltChunks;
};
