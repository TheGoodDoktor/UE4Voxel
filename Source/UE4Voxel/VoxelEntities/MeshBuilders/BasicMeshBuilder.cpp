// Fill out your copyright notice in the Description page of Project Settings.

#include "BasicMeshBuilder.h"

#include "../VoxelWorld.h"

static FVector g_FaceNormals[] =
{
	FVector(0, 1, 0),
	FVector(0, -1, 0),
	FVector(0, 0, 1),
	FVector(0, 0, -1),
	FVector(1, 0, 0),
	FVector(-1, 0, 0),
};

void UBasicMeshBuilder::BuildMeshForChunk(Voxel::FChunk* pChunk)
{
	int index = 0;

	// reset arrays
	pChunk->Vertices.Reset();
	pChunk->UVs.Reset();
	pChunk->Triangles.Reset();

	//Debug.Log("Building mesh for chunk at " + chunk.WorldPos.ToString());

	const int32 chunkSizeBlocks = World->GetChunkSizeInBlocks();

	for (int x = 0; x < chunkSizeBlocks; x++)
	{
		for (int y = 0; y < chunkSizeBlocks; y++)
		{
			for (int z = 0; z < chunkSizeBlocks; z++)
			{
				index = BuildMeshForBlock(FIntVector(x, y, z), pChunk, index);
			}
		}
	}
}

// Build the mesh for a given block within a chunk
int UBasicMeshBuilder::BuildMeshForBlock(const FIntVector& chunkPos, Voxel::FChunk* pChunk, int index)
{
	Voxel::FBlock currentBlock = pChunk->GetBlockAt(chunkPos);

	// if it isn't a transparent block then bail because it won't have any faces to build
	if ((currentBlock.IsTransparent() == false))
		return index;

	FIntVector worldPos = chunkPos + pChunk->WorldPos;
	FVector chunkPosF = FVector(chunkPos);
	//byte lightAmount = 255;//currentBlock.LightAmount;

	// Check surround blocks, if they aren't transparent then they have an outside face

	// Bottom
	Voxel::FBlock block = World->GetBlockAt(worldPos + FIntVector(0, 1, 0));

	if (block.IsTransparent() == false)
	{
		AddBlockFace(chunkPosF + FVector(1, 0, 0), chunkPosF + FVector(1, 0, 1),
			chunkPosF + FVector(0, 0, 1), chunkPosF,
			pChunk, index, block.Type, Voxel::EBlockFace::Bottom);
		index += 4;
	}

/* TODO: Fix this up
	// West
	block = m_WorldData.GetBlock(blockX - 1, blockY, blockZ);
	if (block.IsTransparent() == false)
	{
		AddBlockFace(new IntVec3(x, y, z),
			new IntVec3(x, y, z + 1),
			new IntVec3(x, y + 1, z + 1),
			new IntVec3(x, y + 1, z),
			0.8f, chunk, index, block.m_Type, BlockFace.West, lightAmount);
		index += 4;
	}

	// Top
	block = m_WorldData.GetBlock(blockX, blockY + 1, blockZ);
	if (block.IsTransparent() == false)
	{
		AddBlockFace(new IntVec3(x, y + 1, z),
			new IntVec3(x, y + 1, z + 1),
			new IntVec3(x + 1, y + 1, z + 1),
			new IntVec3(x + 1, y + 1, z),
			0.9f, chunk, index, block.m_Type, BlockFace.Top, lightAmount);

		index += 4;
	}

	// East 
	block = m_WorldData.GetBlock(blockX + 1, blockY, blockZ);
	if (block.IsTransparent() == false)
	{
		AddBlockFace(new IntVec3(x + 1, y + 1, z),
			new IntVec3(x + 1, y + 1, z + 1),
			new IntVec3(x + 1, y, z + 1),
			new IntVec3(x + 1, y, z),
			0.7f, chunk, index, block.m_Type, BlockFace.East, lightAmount);

		index += 4;
	}

	// North
	block = m_WorldData.GetBlock(blockX, blockY, blockZ + 1);
	if (block.IsTransparent() == false)
	{
		AddBlockFace(new IntVec3(x + 1, y, z + 1),
			new IntVec3(x + 1, y + 1, z + 1),
			new IntVec3(x, y + 1, z + 1),
			new IntVec3(x, y, z + 1),
			0.4f, chunk, index, block.m_Type, BlockFace.North, lightAmount);

		index += 4;
	}

	// South
	block = m_WorldData.GetBlock(blockX, blockY, blockZ - 1);
	if (block.IsTransparent() == false)
	{
		AddBlockFace(new IntVec3(x, y, z),
			new IntVec3(x, y + 1, z),
			new IntVec3(x + 1, y + 1, z),
			new IntVec3(x + 1, y, z),
			1.0f, chunk, index, block.m_Type, BlockFace.South, lightAmount);

		index += 4;
	}
	*/
	return index;
}

// Add a face to the chunk
// This function could be in a base class if we need it for more mesh generators
void UBasicMeshBuilder::AddBlockFace(FVector va, FVector vb, FVector vc, FVector vd, Voxel::FChunk* pChunk, int index, int32 blockType,	Voxel::EBlockFace blockFace)
{
	//float actualColour = (colour * blockLight) / 255;
	const float epsilon = 0.001f;
	const float blockSize = World->BlockSize;

	// Vertices
	pChunk->Vertices.Add(va * blockSize);
	pChunk->Vertices.Add(vb * blockSize);
	pChunk->Vertices.Add(vc * blockSize);
	pChunk->Vertices.Add(vd * blockSize);

	// Normals
	//pChunk->Normals.Add(g_FaceNormals[(int)blockFace]);
	//pChunk->Normals.Add(g_FaceNormals[(int)blockFace]);
	//pChunk->Normals.Add(g_FaceNormals[(int)blockFace]);
	//pChunk->Normals.Add(g_FaceNormals[(int)blockFace]);

	// Colours
	/*var item = new Color(actualColour, actualColour, actualColour, 1.0f);
	chunk.Colours.Add(item);
	chunk.Colours.Add(item);
	chunk.Colours.Add(item);
	chunk.Colours.Add(item);
	*/
	// Index quad of 2 tris
	pChunk->Triangles.Add(index + 2);
	pChunk->Triangles.Add(index + 1);
	pChunk->Triangles.Add(index + 0);

	pChunk->Triangles.Add(index + 0);
	pChunk->Triangles.Add(index + 3);
	pChunk->Triangles.Add(index + 2);

	// Texture coords
	/*FRect worldTextureAtlasUv = GetTextureCropForBlockFace(blockType, blockFace);

	chunk.UVs.Add(FVector2(worldTextureAtlasUv.x + epsilon, worldTextureAtlasUv.y + epsilon));
	chunk.UVs.Add(FVector2(worldTextureAtlasUv.x + epsilon,
		worldTextureAtlasUv.y + worldTextureAtlasUv.height - epsilon));
	chunk.UVs.Add(FVector2(worldTextureAtlasUv.x + worldTextureAtlasUv.width - epsilon,
		worldTextureAtlasUv.y + worldTextureAtlasUv.height - epsilon));
	chunk.UVs.Add(FVector2(worldTextureAtlasUv.x + worldTextureAtlasUv.width - epsilon,
		worldTextureAtlasUv.y + epsilon));
		*/
}