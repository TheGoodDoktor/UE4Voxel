// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "VoxelEntities/MeshBuilders/MeshBuilderBase.h"
#include "TestMeshBuilder.generated.h"

/**
 * 
 */
UCLASS()
class UE4VOXEL_API UTestMeshBuilder : public UMeshBuilderBase
{
	GENERATED_BODY()
	
public:
	virtual void BuildMeshForChunk(Voxel::FChunk* pChunk) override;
private:
	void AddQuad(Voxel::FChunk* pChunk, FVector topLeft, FVector topRight, FVector bottomLeft, FVector bottomRight, FVector normal, FVector tangent, FLinearColor colour);

	int32 VertexIndex;
	int32 TriangleIndex;
	
	
};
