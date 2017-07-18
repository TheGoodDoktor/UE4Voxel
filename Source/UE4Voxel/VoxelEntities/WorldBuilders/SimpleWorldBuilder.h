// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "VoxelEntities/WorldBuilders/WorldBuilderBase.h"
#include "SimpleWorldBuilder.generated.h"

/**
 * 
 */
UCLASS()
class UE4VOXEL_API USimpleWorldBuilder : public UWorldBuilderBase
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;
	virtual void BuildWorldChunk(Voxel::FChunk* pChunk) override;

	UPROPERTY(EditAnywhere)
	bool	BuildCaves;
	
	UPROPERTY(EditAnywhere)
	float	CaveThreshold;
private:
	void GenerateTerrain(Voxel::FChunk* pChunk, int blockXInChunk, int blockYInChunk, int blockX, int blockY, int worldHeightInBlocks);
	float GetHeightField(int x, int y) const;
	void SetHeightField(int x, int y, float val);

	float *	HeightField;

};
