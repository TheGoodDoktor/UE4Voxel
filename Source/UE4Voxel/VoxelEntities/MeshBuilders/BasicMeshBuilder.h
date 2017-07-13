// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "VoxelEntities/MeshBuilders/MeshBuilderBase.h"
#include "BasicMeshBuilder.generated.h"



/**
 * 
 */
UCLASS()
class UE4VOXEL_API UBasicMeshBuilder : public UMeshBuilderBase
{
	GENERATED_BODY()
	
public:
	virtual void BuildMeshForChunk(Voxel::FChunk* pChunk) override;
private:
	int BuildMeshForBlock(const FIntVector& chunkPos, Voxel::FChunk* pChunk, int index);
	void AddBlockFace(FVector va, FVector vb, FVector vc, FVector vd, Voxel::FChunk* pChunk, int index, int32 blockType, Voxel::EBlockFace blockFace);

};
