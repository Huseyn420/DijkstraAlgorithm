// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/LineBatchComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Math/UnrealMathUtility.h"
#include "GridManager.generated.h"

UCLASS()
class ALGORITHM_API AGridManager : public AActor
{
	GENERATED_BODY()

public:
	AGridManager();
	TArray<FVector> GNodes;
	TArray<TArray<bool>> GMEdges;
	TArray<TArray<FColor>> GMEdgeColors;

	int GMedgesCount;

	UPROPERTY(EditAnywhere, Category = "Grid Manager")
		int GridW = 15;
	UPROPERTY(EditAnywhere, Category = "Grid Manager")
		int GridH = 15;

	UPROPERTY(EditAnywhere, Category = "Grid Manager")
		float NodeRX = 256.0f;
	UPROPERTY(EditAnywhere, Category = "Grid Manager")
		float NodeRY = 256.0f;
	UPROPERTY(EditAnywhere, Category = "Grid Manager")
		float NodeRZ = 256.0f;

	UPROPERTY(EditAnywhere, Category = "Grid Manager")
		int CharactersCount = 5;

	UPROPERTY(EditAnywhere)
		UInstancedStaticMeshComponent* EdgesMesh;

	UPROPERTY(EditAnywhere)
		UInstancedStaticMeshComponent* NodesMesh;

protected:
	virtual void BeginPlay() override;


public:
	void Dijkstra(TArray<FVector>& Route, int StartNode, int EndNode);

private:
	void CreateGrid();
	void SetupNodesMesh();
	void SetupEdgesMesh();
	FColor GetContrastColor(int h);
	int GetNeighbours(int CurrentNode, TArray<bool>& NodesMarks);
	int GetNeighbours(int CurrentNode);
	void CreateCharacters();
};
