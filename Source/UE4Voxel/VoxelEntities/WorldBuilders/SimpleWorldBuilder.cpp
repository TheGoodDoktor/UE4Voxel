// Fill out your copyright notice in the Description page of Project Settings.

#include "SimpleWorldBuilder.h"
#include "../VoxelWorld.h"
#include "SimplexNoiseBPLibrary.h"

void USimpleWorldBuilder::BeginPlay()
{
	Super::BeginPlay();

	const Voxel::FWorld &world = World->GetVoxelWorld();

	HeightField = new float[world.WorldSizeBlocks.X * world.WorldSizeBlocks.Y];

	const int worldHeightInBlocks = world.WorldSizeBlocks.Z;
	const int minimumGroundheight = worldHeightInBlocks / 4;
	const int groundheightRange = (int)(worldHeightInBlocks * 0.75f);

	for (int blockX = 0; blockX < world.WorldSizeBlocks.X; blockX++)
	{
		for (int blockY = 0; blockY < world.WorldSizeBlocks.Y; blockY++)
		{
			const float octave1 = USimplexNoiseBPLibrary::SimplexNoise2D(blockX * 0.0001f, blockY * 0.0001f) * 0.5f;
			const float octave2 = USimplexNoiseBPLibrary::SimplexNoise2D(blockX * 0.0005f, blockY * 0.0005f) * 0.25f;
			const float octave3 = USimplexNoiseBPLibrary::SimplexNoise2D(blockX * 0.005f, blockY * 0.005f) * 0.12f;
			const float octave4 = USimplexNoiseBPLibrary::SimplexNoise2D(blockX * 0.01f, blockY * 0.01f) * 0.12f;
			const float octave5 = USimplexNoiseBPLibrary::SimplexNoise2D(blockX * 0.03f, blockY * 0.03f) * octave4;
			float lowerGroundHeight = octave1 + octave2 + octave3 + octave4 + octave5;
			lowerGroundHeight = (lowerGroundHeight * groundheightRange) + minimumGroundheight;
			SetHeightField(blockX,blockY,lowerGroundHeight);
		}
	}
}

void USimpleWorldBuilder::BuildWorldChunk(Voxel::FChunk* pChunk)
{
	if (HeightField == nullptr)
		return;

	const Voxel::FWorld &world = World->GetVoxelWorld();

	for (int x = 0; x < Voxel::FChunk::kChunkSize; x++)
	{
		int blockX = pChunk->WorldPos.X + x;

		for (int y = 0; y < Voxel::FChunk::kChunkSize; y++)
		{
			int blockY = pChunk->WorldPos.Y + y;
			GenerateTerrain(pChunk, x, y, blockX, blockY, world.WorldSizeBlocks.Z);
		}
	}
}


void USimpleWorldBuilder::GenerateTerrain(Voxel::FChunk* pChunk, int blockXInChunk, int blockYInChunk, int blockX, int blockY, int worldHeightInBlocks)
{
	const int groundHeightInChunk = floor(GetHeightField(blockX, blockY)) - pChunk->WorldPos.Z;

	for (int z = 0; z < Voxel::FChunk::kChunkSize; z++)
	{
		const bool bUnderground = z < groundHeightInChunk;
		float density = bUnderground ? 1.0f : 0.0f;
		int blockType = bUnderground ? 1 : 0;

		if (bUnderground)    // are we underground - check for caves
		{
			if (BuildCaves)
			{
				const int worldZ = z + pChunk->WorldPos.Z;
				const float octave1 = USimplexNoiseBPLibrary::SimplexNoise3D(blockX * 0.009f, blockY * 0.009f, worldZ * 0.009f) * 0.25f;

				float initialNoise = octave1 + USimplexNoiseBPLibrary::SimplexNoise3D(blockX * 0.04f, blockY * 0.04f, worldZ * 0.04f) * 0.15f;
				initialNoise += USimplexNoiseBPLibrary::SimplexNoise3D(blockX * 0.08f, blockY * 0.08f, worldZ * 0.08f) * 0.05f;

				if (initialNoise > CaveThreshold)
				{
					blockType = 0; // cave
					density = 0;
				}
				else
				{
					// TODO: work out cave density
					//density = 1.0f - initialNoise;
				}
			}

		}
		else
		{
			// Work out above ground density
			if (z == groundHeightInChunk)
				density = GetHeightField(blockX, blockY) - floor(GetHeightField(blockX,blockY));
		}

		// set block
		Voxel::FBlock newBlock;
		newBlock.Type = blockType;
		newBlock.Density = density;
		pChunk->SetBlockAt(FIntVector(blockXInChunk, blockYInChunk, z), newBlock);
		//chunk.Blocks[blockXInChunk, y, blockZInChunk].m_Type = blockType;
		//chunk.Blocks[blockXInChunk, y, blockZInChunk].m_Density = density;
	}

}

float USimpleWorldBuilder::GetHeightField(int x, int y) const 
{ 
	const Voxel::FWorld &world = World->GetVoxelWorld();
	check(x >= 0 && x < world.WorldSizeBlocks.X && y >= 0 && y < world.WorldSizeBlocks.Y);
	if (x >= 0 && x < world.WorldSizeBlocks.X && y >= 0 && y < world.WorldSizeBlocks.Y)
		return HeightField[x + (y * world.WorldSizeBlocks.X)];
	else
		return 0;
}

void  USimpleWorldBuilder::SetHeightField(int x, int y, float val) 
{ 
	const Voxel::FWorld &world = World->GetVoxelWorld();
	check(x >= 0 && x < world.WorldSizeBlocks.X && y >= 0 && y < world.WorldSizeBlocks.Y);
	check(val >= 0 && val < world.WorldSizeBlocks.Z);
	HeightField[x + (y * world.WorldSizeBlocks.X)] = val;
}

