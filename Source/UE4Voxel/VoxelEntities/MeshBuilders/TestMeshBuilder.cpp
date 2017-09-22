// Fill out your copyright notice in the Description page of Project Settings.

#include "TestMeshBuilder.h"
#include "../VoxelWorld.h"

void UTestMeshBuilder::BuildMeshForChunk(Voxel::FChunk* pChunk)
{
	int index = 0;

	// reset arrays
	pChunk->Vertices.Reset();
	pChunk->Normals.Reset();
	pChunk->Tangents.Reset();
	pChunk->UVs.Reset();
	pChunk->Colours.Reset();
	pChunk->Triangles.Reset();

	VertexIndex = 0;
	TriangleIndex = 0;
	
	// Build a cube
	float offsetX = 100.0f;
	float offsetY = 100.0f;
	float offsetZ = 100.0f;

	// Define the 8 corners of the cube
	FVector p0 = FVector(offsetX, offsetY, -offsetZ);
	FVector p1 = FVector(offsetX, -offsetY, -offsetZ);
	FVector p2 = FVector(offsetX, -offsetY, offsetZ);
	FVector p3 = FVector(offsetX, offsetY, offsetZ);
	FVector p4 = FVector(-offsetX, offsetY, -offsetZ);
	FVector p5 = FVector(-offsetX, -offsetY, -offsetZ);
	FVector p6 = FVector(-offsetX, -offsetY, offsetZ);
	FVector p7 = FVector(-offsetX, offsetY, offsetZ);

	FVector normal, tangent;
	FLinearColor colour(1.0f, 1.0f, 1.0f, 1.0f);

	// Front (+X) face: 0-1-2-3
	normal = FVector(1, 0, 0);
	tangent = FVector(0, 1, 0);
	AddQuad(pChunk, p0, p1, p2, p3, normal, tangent, colour);

	// Back (-X) face: 5-4-7-6
	normal = FVector(-1, 0, 0);
	tangent = FVector(0, -1, 0);
	AddQuad(pChunk, p5, p4, p7, p6, normal, tangent, colour);

	// Left (-Y) face: 1-5-6-2
	normal = FVector(0, -1, 0);
	tangent = FVector(1, 0, 0);
	AddQuad(pChunk, p1, p5, p6, p2, normal, tangent, colour);

	// Right (+Y) face: 4-0-3-7
	normal = FVector(0, 1, 0);
	tangent = FVector(-1, 0, 0);
	AddQuad(pChunk, p4, p0, p3, p7, normal, tangent, colour);

	// Top (+Z) face: 6-7-3-2
	normal = FVector(0, 0, 1);
	tangent = FVector(0, 1, 0);
	AddQuad(pChunk, p6, p7, p3, p2, normal, tangent, colour);

	// Bottom (-Z) face: 1-0-4-5
	normal = FVector(0, 0, -1);
	tangent = FVector(0, -1, 0);
	AddQuad(pChunk, p1, p0, p4, p5, normal, tangent, colour);
}

void UTestMeshBuilder::AddQuad(Voxel::FChunk* pChunk, FVector bottomLeft, FVector bottomRight, FVector topRight, FVector topLeft, FVector normal, FVector tangent, FLinearColor colour)
{
	pChunk->Vertices.Add(bottomLeft);
	pChunk->Vertices.Add(bottomRight);
	pChunk->Vertices.Add(topRight);
	pChunk->Vertices.Add(topLeft);
	pChunk->UVs.Add(FVector2D(0.0f, 1.0f));
	pChunk->UVs.Add(FVector2D(1.0f, 1.0f));
	pChunk->UVs.Add(FVector2D(1.0f, 0.0f));
	pChunk->UVs.Add(FVector2D(0.0f, 0.0f));
	pChunk->Triangles.Add(VertexIndex+0);
	pChunk->Triangles.Add(VertexIndex+1);
	pChunk->Triangles.Add(VertexIndex+2);
	pChunk->Triangles.Add(VertexIndex+0);
	pChunk->Triangles.Add(VertexIndex+2);
	pChunk->Triangles.Add(VertexIndex+3);

	pChunk->Normals.Add(normal);
	pChunk->Normals.Add(normal);
	pChunk->Normals.Add(normal);
	pChunk->Normals.Add(normal);

	pChunk->Tangents.Add(FProcMeshTangent(tangent, false));
	pChunk->Tangents.Add(FProcMeshTangent(tangent, false));
	pChunk->Tangents.Add(FProcMeshTangent(tangent, false));
	pChunk->Tangents.Add(FProcMeshTangent(tangent, false));

	pChunk->Colours.Add(colour);
	pChunk->Colours.Add(colour);
	pChunk->Colours.Add(colour);
	pChunk->Colours.Add(colour);

	VertexIndex += 4;
	TriangleIndex += 6;
}


