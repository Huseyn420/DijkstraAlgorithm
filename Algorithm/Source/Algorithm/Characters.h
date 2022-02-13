// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Math/UnrealMathUtility.h"
#include "Components/LineBatchComponent.h"
#include "Characters.generated.h"

UCLASS()
class ALGORITHM_API ACharacters : public AActor
{
	GENERATED_BODY()

public:
	ACharacters();

	TArray<TArray<FVector>> Routes;

	UPROPERTY(EditAnywhere)
		UInstancedStaticMeshComponent* CharactersMesh;
	UPROPERTY(EditAnywhere)
		TArray<UInstancedStaticMeshComponent*> RoutesMesh;

	UPROPERTY(EditAnywhere)
		float Speed = 300.0f;

	TArray<int> StartNodes;
	TArray<int> CurrentNodes;
	TArray<int> EndNodes;

	TArray<FVector> CharacterLocations;
	TArray<FColor> RouteColors;

	class AGridManager* GMParent;

public:
	virtual void Tick(float DeltaTime) override;

	void SetupRouteMesh(int RouteIndex);

private:
	void DrawRoute(int CharacterIndex);
};

