// Fill out your copyright notice in the Description page of Project Settings.

#include "MarchingCubesMeshBuilder.h"

#include "../VoxelWorld.h"
#include "MarchingCubesLUT.h"

FVector VertexInterp(float isolevel, FVector p1, FVector p2, float valp1, float valp2);

void UMarchingCubesMeshBuilder::BuildMeshForChunk(Voxel::FChunk* pChunk)
{
	int index = 0;

	// reset arrays
	pChunk->Vertices.Reset();
	pChunk->UVs.Reset();
	pChunk->Triangles.Reset();

	//Debug.Log("Building mesh for chunk at " + chunk.WorldPos.ToString());

	int32 chunkSizeBlocks = World->GetChunkSizeInBlocks();

	for (int x = 0; x < chunkSizeBlocks; x++)
	{
		int blockX = pChunk->WorldPos.X + x;
		for (int y = 0; y < chunkSizeBlocks; y++)
		{
			int blockY = pChunk->WorldPos.Y + y;
			for (int z = 0; z < chunkSizeBlocks; z++)
			{
				int blockZ = pChunk->WorldPos.Z + z;
				// x,y,z is co-ord of block inside chunk
				// blockX,blockY,blockZ is co-ord of block in world
				index = BuildMeshForBlock(FIntVector(blockX, blockY, blockZ), FVector(x, y, z), pChunk, index);
			}
		}
	}
}


// Build the mesh for a given block within a chunk
int UMarchingCubesMeshBuilder::BuildMeshForBlock(const FIntVector& worldPos, const FVector& chunkPos, Voxel::FChunk* pChunk, int index)
{
	//FBlock currentBlock = pChunk->BlockData[];
	FVector points[8];
	FVector vertlist[12];
	float isoLevel = 0.8f;  // TODO: proper value
	float density[8];
	const float blockSize = World->BlockSize;

	CalculateDensitiesForBlock(worldPos, density);// Fill in vertex density values

												  // Fill in block points
	FVector offset(0.5f, 0.5f, 0.5f);
	points[0] = (chunkPos + FVector(0, 0, 1) + offset) * blockSize;
	points[1] = (chunkPos + FVector(1, 0, 1) + offset) * blockSize;
	points[2] = (chunkPos + FVector(1, 0, 0) + offset) * blockSize;
	points[3] = (chunkPos + FVector(0, 0, 0) + offset) * blockSize;

	points[4] = (chunkPos + FVector(0, 1, 1) + offset) * blockSize;
	points[5] = (chunkPos + FVector(1, 1, 1) + offset) * blockSize;
	points[6] = (chunkPos + FVector(1, 1, 0) + offset) * blockSize;
	points[7] = (chunkPos + FVector(0, 1, 0) + offset) * blockSize;

	int cubeindex = 0;
	if (density[0] < isoLevel) cubeindex |= 1;
	if (density[1] < isoLevel) cubeindex |= 2;
	if (density[2] < isoLevel) cubeindex |= 4;
	if (density[3] < isoLevel) cubeindex |= 8;
	if (density[4] < isoLevel) cubeindex |= 16;
	if (density[5] < isoLevel) cubeindex |= 32;
	if (density[6] < isoLevel) cubeindex |= 64;
	if (density[7] < isoLevel) cubeindex |= 128;

	// Cube is entirely in/out of the surface 
	if (GEdgeTable[cubeindex] == 0)
		return index;

	// Find the vertices where the surface intersects the cube 
	if ((GEdgeTable[cubeindex] & 1) != 0)
		vertlist[0] = VertexInterp(isoLevel, points[0], points[1], density[0], density[1]);
	if ((GEdgeTable[cubeindex] & 2) != 0)
		vertlist[1] = VertexInterp(isoLevel, points[1], points[2], density[1], density[2]);
	if ((GEdgeTable[cubeindex] & 4) != 0)
		vertlist[2] = VertexInterp(isoLevel, points[2], points[3], density[2], density[3]);
	if ((GEdgeTable[cubeindex] & 8) != 0)
		vertlist[3] = VertexInterp(isoLevel, points[3], points[0], density[3], density[0]);
	if ((GEdgeTable[cubeindex] & 16) != 0)
		vertlist[4] = VertexInterp(isoLevel, points[4], points[5], density[4], density[5]);
	if ((GEdgeTable[cubeindex] & 32) != 0)
		vertlist[5] = VertexInterp(isoLevel, points[5], points[6], density[5], density[6]);
	if ((GEdgeTable[cubeindex] & 64) != 0)
		vertlist[6] = VertexInterp(isoLevel, points[6], points[7], density[6], density[7]);
	if ((GEdgeTable[cubeindex] & 128) != 0)
		vertlist[7] = VertexInterp(isoLevel, points[7], points[4], density[7], density[4]);
	if ((GEdgeTable[cubeindex] & 256) != 0)
		vertlist[8] = VertexInterp(isoLevel, points[0], points[4], density[0], density[4]);
	if ((GEdgeTable[cubeindex] & 512) != 0)
		vertlist[9] = VertexInterp(isoLevel, points[1], points[5], density[1], density[5]);
	if ((GEdgeTable[cubeindex] & 1024) != 0)
		vertlist[10] = VertexInterp(isoLevel, points[2], points[6], density[2], density[6]);
	if ((GEdgeTable[cubeindex] & 2048) != 0)
		vertlist[11] = VertexInterp(isoLevel, points[3], points[7], density[3], density[7]);

	// Create the triangles 
	int ntriang = 0;
	for (int i = 0; GTriTable[cubeindex][i] != -1; i += 3)
	{
		//Vector3 vertA = points[1];
		//Vector3 vertB = points[2];
		//Vector3 vertC = points[3]; 
		FVector vertA = vertlist[GTriTable[cubeindex][i + 2]];
		FVector vertB = vertlist[GTriTable[cubeindex][i + 1]];
		FVector vertC = vertlist[GTriTable[cubeindex][i + 0]];

		//Vector3 normal = Vector3.Cross(vertB - vertA, vertC - vertA).normalized;
		//Debug.Log("vert A: " + vertA.ToString());
		//Debug.Log("vert B: " + vertB.ToString());
		//Debug.Log("vert C: " + vertC.ToString());

		// add vertices
		pChunk->Vertices.Add(vertA);
		pChunk->Vertices.Add(vertB);
		pChunk->Vertices.Add(vertC);

		// TODO: UVs ?

		// index triangles
		pChunk->Triangles.Add(index++);
		pChunk->Triangles.Add(index++);
		pChunk->Triangles.Add(index++);

		/*
		chunk.Vertices.Add(vertA);
		chunk.Vertices.Add(vertB);
		chunk.Vertices.Add(vertC);
		chunk.Normals.Add(normal);
		chunk.Normals.Add(normal);
		chunk.Normals.Add(normal);
		chunk.Colours.Add(Color.red);
		chunk.Colours.Add(Color.red);
		chunk.Colours.Add(Color.red);
		chunk.Indices.Add(index++);
		chunk.Indices.Add(index++);
		chunk.Indices.Add(index++);
		*/
		ntriang++;
	}

	//Debug.Log("Created " + ntriang.ToString() + " for block at " + new IntVec3(x, y, z).ToString());

	return index;
}


void UMarchingCubesMeshBuilder::CalculateDensitiesForBlock(FIntVector worldPos, float *density)
{
	density[0] = World->GetBlockAt(worldPos + FIntVector(0, 0, 1)).Density;
	density[1] = World->GetBlockAt(worldPos + FIntVector(1, 0, 1)).Density;
	density[2] = World->GetBlockAt(worldPos + FIntVector(1, 0, 0)).Density;
	density[3] = World->GetBlockAt(worldPos + FIntVector(0, 0, 0)).Density;
	density[4] = World->GetBlockAt(worldPos + FIntVector(0, 1, 1)).Density;
	density[5] = World->GetBlockAt(worldPos + FIntVector(1, 1, 1)).Density;
	density[6] = World->GetBlockAt(worldPos + FIntVector(1, 1, 0)).Density;
	density[7] = World->GetBlockAt(worldPos + FIntVector(0, 1, 0)).Density;
}


// Util Funcs

FVector VertexInterp(float isolevel, FVector p1, FVector p2, float valp1, float valp2)
{
	float mu;
	FVector p;

	if (FMath::Abs(isolevel - valp1) < 0.00001)
		return(p1);
	if (FMath::Abs(isolevel - valp2) < 0.00001)
		return(p2);
	if (FMath::Abs(valp1 - valp2) < 0.00001)
		return(p1);
	mu = (isolevel - valp1) / (valp2 - valp1);

	//if (mu > 1.0f)
	//	Debug.Log("mu value is: " + mu.ToString());

	p.X = p1.X + (mu* (p2.X - p1.X));
	p.Y = p1.Y + (mu* (p2.Y - p1.Y));
	p.Z = p1.Z + (mu* (p2.Z - p1.Z));

	return(p);
}