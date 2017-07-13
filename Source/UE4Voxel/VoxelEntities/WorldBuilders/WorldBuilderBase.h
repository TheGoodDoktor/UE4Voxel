// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WorldBuilderBase.generated.h"

namespace Voxel
{
	struct FChunk;
}

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Abstract )
class UE4VOXEL_API UWorldBuilderBase : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWorldBuilderBase();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void BuildWorldChunk(Voxel::FChunk* pChunk) {}
	
protected:
	class AVoxelWorld*	World;
};
