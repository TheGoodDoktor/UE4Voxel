// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "VoxelEntities/WorldBuilders/WorldBuilderBase.h"
#include "../VoxelTypes.h"
#include "TestWorldBuilder.generated.h"

/**
 * 
 */
UCLASS()
class UE4VOXEL_API UTestWorldBuilder : public UWorldBuilderBase
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	virtual void BuildWorldChunk(Voxel::FChunk* pChunk) override;

	Voxel::FBlock	SolidBlock;	
};
