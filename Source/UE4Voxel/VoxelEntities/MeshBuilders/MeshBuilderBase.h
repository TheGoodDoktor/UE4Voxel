// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MeshBuilderBase.generated.h"

namespace Voxel
{
	enum class EBlockType : uint8;
	enum class EBlockFace : uint8;
	struct FChunk;
}

UCLASS(ClassGroup = (Custom), BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent), Abstract)
class UE4VOXEL_API UMeshBuilderBase : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMeshBuilderBase();

	virtual void BuildMeshForChunk(Voxel::FChunk* pChunk) { }

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
protected:
	class AVoxelWorld *	World;	// world this builder belongs to
	
};
