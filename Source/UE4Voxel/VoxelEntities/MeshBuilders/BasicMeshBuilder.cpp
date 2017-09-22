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
	pChunk->Normals.Reset();
	pChunk->Tangents.Reset();
	pChunk->UVs.Reset();
	pChunk->Colours.Reset();
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
	const Voxel::FBlock *pCurrentBlock = pChunk->GetBlockAt(chunkPos);

	// if it isn't valid or a transparent block then bail because it won't have any faces to build
	if (pCurrentBlock == nullptr || pCurrentBlock->IsTransparent() == false)
		return index;

	const FIntVector worldPos = chunkPos + pChunk->WorldPos;	// get world position of this block
	const FVector chunkPosF = FVector(chunkPos);
	//byte lightAmount = 255;//currentBlock.LightAmount;

	// Check surround blocks, if they aren't transparent then they have an outside face

	const Voxel::FBlock *pFaceBlock = nullptr;
	
	// X positive face
	pFaceBlock = World->GetBlockAt(worldPos + FIntVector(-1, 0, 0));
	check(pFaceBlock != nullptr);

	if (pFaceBlock->IsTransparent() == false)
	{
		AddBlockFace(
			chunkPosF + FVector(0, 1, 0),
			chunkPosF + FVector(0, 1, 1), 
			chunkPosF + FVector(0, 0, 1),
			chunkPosF + FVector(0, 0, 0), 
			pChunk, index, pFaceBlock->Type, Voxel::EBlockFace::XP);
		index += 4;
	}

	// X negative face
	pFaceBlock = World->GetBlockAt(worldPos + FIntVector(1, 0, 0));
	check(pFaceBlock != nullptr);

	if (pFaceBlock->IsTransparent() == false)
	{
		AddBlockFace(
			chunkPosF + FVector(1, 0, 0),
			chunkPosF + FVector(1, 0, 1),
			chunkPosF + FVector(1, 1, 1),
			chunkPosF + FVector(1, 1, 0),
			pChunk, index, pFaceBlock->Type, Voxel::EBlockFace::XN);
		index += 4;
	}

	// y positive face
	pFaceBlock = World->GetBlockAt(worldPos + FIntVector(0, -1, 0));
	check(pFaceBlock != nullptr);

	if (pFaceBlock->IsTransparent() == false)
	{
		AddBlockFace(
			chunkPosF + FVector(0, 0, 0),
			chunkPosF + FVector(0, 0, 1),
			chunkPosF + FVector(1, 0, 1),
			chunkPosF + FVector(1, 0, 0),
			pChunk, index, pFaceBlock->Type, Voxel::EBlockFace::YP);
		index += 4;
	}

	// y negative face
	pFaceBlock = World->GetBlockAt(worldPos + FIntVector(0, 1, 0));
	check(pFaceBlock != nullptr);

	if (pFaceBlock->IsTransparent() == false)
	{
		AddBlockFace(
			chunkPosF + FVector(1, 1, 0),
			chunkPosF + FVector(1, 1, 1),
			chunkPosF + FVector(0, 1, 1),
			chunkPosF + FVector(0, 1, 0),
			pChunk, index, pFaceBlock->Type, Voxel::EBlockFace::YN);
		index += 4;
	}
	

	// z positive face
	pFaceBlock = World->GetBlockAt(worldPos + FIntVector(0, 0, -1));
	check(pFaceBlock != nullptr);

	if (pFaceBlock->IsTransparent() == false)
	{
		AddBlockFace(
			chunkPosF + FVector(1, 0, 0),
			chunkPosF + FVector(1, 1, 0),
			chunkPosF + FVector(0, 1, 0),
			chunkPosF + FVector(0, 0, 0),
			pChunk, index, pFaceBlock->Type, Voxel::EBlockFace::ZP);
		index += 4;
	}

	// z negative face
	pFaceBlock = World->GetBlockAt(worldPos + FIntVector(0, 0, 1));
	check(pFaceBlock != nullptr);

	if (pFaceBlock->IsTransparent() == false)
	{
		AddBlockFace(
			chunkPosF + FVector(0, 0, 1),
			chunkPosF + FVector(0, 1, 1),
			chunkPosF + FVector(1, 1, 1),
			chunkPosF + FVector(1, 0, 1),
			pChunk, index, pFaceBlock->Type, Voxel::EBlockFace::ZN);
		index += 4;
	}

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