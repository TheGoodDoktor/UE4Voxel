// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VoxelWorld.generated.h"

UCLASS()
class UE4VOXEL_API AVoxelWorld : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AVoxelWorld();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Which mesh builder to use
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UMeshBuilderBase>	MeshBuilder;
	
	// Which world builder to use
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UWorldBuilderBase>	WorldBuilder;
};
