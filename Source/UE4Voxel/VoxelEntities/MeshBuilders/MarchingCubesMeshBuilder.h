// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "VoxelEntities/MeshBuilders/MeshBuilderBase.h"
#include "MarchingCubesMeshBuilder.generated.h"

/**
 * 
 */
UCLASS()
class UE4VOXEL_API UMarchingCubesMeshBuilder : public UMeshBuilderBase
{
	GENERATED_BODY()

public:
	UMarchingCubesMeshBuilder();

	virtual void BuildMeshForChunk(Voxel::FChunk* pChunk) override;

	UPROPERTY(EditAnywhere)
	float	IsoThreshold;

private:
	void CalculateDensitiesForBlock(FIntVector worldPos, float *density);
	int BuildMeshForBlock(const FIntVector& worldPos, const FVector& chunkPos, Voxel::FChunk* pChunk, int index);

};
