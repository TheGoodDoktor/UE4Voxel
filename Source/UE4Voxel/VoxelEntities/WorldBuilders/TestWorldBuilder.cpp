// Fill out your copyright notice in the Description page of Project Settings.

#include "TestWorldBuilder.h"
#include "../VoxelWorld.h"

void UTestWorldBuilder::BeginPlay()
{
	Super::BeginPlay();

	// Setup solid block
	SolidBlock.Density = 1.0f;
	SolidBlock.Type = 1;

	for (int blockX = 0; blockX < World->WorldSize.X; blockX++)
	{
		for (int blockY = 0; blockY < World->WorldSize.Y; blockY++)
		{
			const int height = FMath::RandRange(1, World->WorldSize.Z);

			for (int z = 0; z < height; z++)
			{
				World->SetBlockAt(FIntVector(blockX, blockY, z), SolidBlock);
			}
		}
	}
}

void UTestWorldBuilder::BuildWorldChunk(Voxel::FChunk* pChunk)
{
	const int32 chunkSize = pChunk->kChunkSize;

	return;

	for (int x = 0; x<chunkSize; x++)
	{
		for (int y = 0; y<chunkSize; y++)
		{
			for (int z = 0; z < chunkSize; z++)
			{
				if(x == 1 || y ==1 || z==1 || x == chunkSize-2 || y == chunkSize - 2 || z == chunkSize - 2)
					World->SetBlockAt(FIntVector(x, y, z) + pChunk->WorldPos, SolidBlock);
			}
			//const int height = ((x == chunkSize /2) && (y == chunkSize / 2)) ? 10 : 1;// FMath::RandRange(1, 10);

			/*const int height = FMath::RandRange(1, 5);
			
			for (int z = 0; z<height; z++)
			{
				World->SetBlockAt(FIntVector(x,y,z) + pChunk->WorldPos, SolidBlock);
			}*/
		}
	}
}