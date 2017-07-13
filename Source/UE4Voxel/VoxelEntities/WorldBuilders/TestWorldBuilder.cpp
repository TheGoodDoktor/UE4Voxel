// Fill out your copyright notice in the Description page of Project Settings.

#include "TestWorldBuilder.h"
#include "../VoxelWorld.h"

void UTestWorldBuilder::BeginPlay()
{

}

void UTestWorldBuilder::BuildWorldChunk(Voxel::FChunk* pChunk)
{
	const int32 chunkSize = pChunk->kChunkSize;

	for (int x = 0; x<chunkSize; x++)
	{
		for (int y = 0; y<chunkSize; y++)
		{
			const int height = FMath::RandRange(1, 10);

			for (int z = 0; z<height; z++)
			{
				World->SetBlockAt(FIntVector(x,y,z) + pChunk->WorldPos, SolidBlock);
			}
		}
	}
}