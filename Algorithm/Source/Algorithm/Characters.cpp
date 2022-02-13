// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters.h"
#include "GridManager.h"

ACharacters::ACharacters() 
{
    PrimaryActorTick.bCanEverTick = true;

    CharactersMesh =
        CreateDefaultSubobject<UInstancedStaticMeshComponent>("CharactersMesh");
    CharactersMesh->SetupAttachment(RootComponent);

    static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(
        TEXT("StaticMesh'/Game/MaterialFunctionCollection/ShowcaseAssets/DemoRoom/"
            "Meshes/SM_MatPreviewMesh_02_Automotive.SM_MatPreviewMesh_02_Automotive'"));
    if (MeshAsset.Succeeded()) 
    {
        CharactersMesh->SetStaticMesh(MeshAsset.Object);
    }

    static ConstructorHelpers::FObjectFinder<UMaterialInterface>
        AssetEmissiveMaterial(
            TEXT("MaterialInstanceConstant'/Game/MaterialFunctionCollection/"
                "Materials/Materials/Emissive/MI_Emissive_01.MI_Emissive_01'"));
    if (AssetEmissiveMaterial.Succeeded()) 
    {
        CharactersMesh->SetMaterial(0, AssetEmissiveMaterial.Object);
    }

    CharactersMesh->SetGenerateOverlapEvents(false);
    CharactersMesh->CanCharacterStepUpOn = ECB_No;
    CharactersMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ACharacters::Tick(float DeltaTime) 
{
    Super::Tick(DeltaTime);
 
    int CharactersCount = CharacterLocations.Num();
    
    for (int i = 0; i < CharactersCount; i++) 
    {
        if (CurrentNodes[i] != Routes[i].Num()) 
        {
            FVector Direction = Routes[i][CurrentNodes[i]] - CharacterLocations[i];
            if (Direction.Size() > Speed * DeltaTime) 
            {
                FRotator NewRotation = Direction.Rotation() + FRotator(0, -90, 0);
                FVector NewLocation = Direction.GetSafeNormal() * Speed * DeltaTime + CharacterLocations[i];
                FVector NewScale = FVector(5.0f);
                CharacterLocations[i] = NewLocation;
                CharactersMesh->UpdateInstanceTransform(
                    i, FTransform(NewRotation, NewLocation, NewScale), false,
                    i == CharactersCount - 1, false);
            }
            else 
            {
                CharacterLocations[i] = Routes[i][CurrentNodes[i]];
                CurrentNodes[i] += 1;
            }
        }
        else 
        {
            Routes[i].Empty();
            StartNodes[i] = EndNodes[i];
            while (true) 
            {
                if ((EndNodes[i] = FMath::RandRange(0, GMParent->GNodes.Num() - 1)) != StartNodes[i]) 
                {
                    break;
                }
            }
            GMParent->Dijkstra(Routes[i], StartNodes[i], EndNodes[i]);
            CurrentNodes[i] = 0;
            
            DrawRoute(i);
        }
    }
}

void ACharacters::SetupRouteMesh(int RouteIndex) 
{
    FString Name = "RouteMesh" + FString::FromInt(RouteIndex);
    UInstancedStaticMeshComponent* RouteMesh =
        NewObject<UInstancedStaticMeshComponent>(this, FName(Name));

    RouteMesh->SetupAttachment(RootComponent);
    RouteMesh->RegisterComponent();

	UStaticMesh* CylinderMesh = LoadObject<UStaticMesh>(
		NULL, TEXT("/Engine/BasicShapes/Cylinder"));
    if (CylinderMesh) 
    {
        RouteMesh->SetStaticMesh(CylinderMesh);
    }

	UMaterialInterface* Material = LoadObject<UMaterialInterface>(
        NULL, TEXT("Material'/Game/MaterialFunctionCollection/Materials/"
                   "Materials/Emissive/M_Emissive.M_Emissive'"));
    if (Material) 
    {
        RouteMesh->SetMaterial(0, Material);
        RouteMesh->SetVectorParameterValueOnMaterials(
            TEXT("Main Color"), FVector(RouteColors[RouteIndex]));
    }
    
    RouteMesh->SetMobility(EComponentMobility::Static);
    RouteMesh->SetGenerateOverlapEvents(false);
    RouteMesh->CanCharacterStepUpOn = ECB_No;
    RouteMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    RoutesMesh.Insert(RouteMesh, RouteIndex);
}

void ACharacters::DrawRoute(int CharacterIndex) 
{
    TArray<FVector> Route = Routes[CharacterIndex];
    UInstancedStaticMeshComponent* RouteMesh = RoutesMesh[CharacterIndex];

    RouteMesh->ClearInstances();
    for (int i = 0; i < Route.Num() - 1; i++) 
    {
		FVector Direction = Route[i] - Route[i + 1];
		FVector Location = (Route[i] + Route[i + 1]) / 2;

		FVector Scale = FVector(0.3f, 0.3f, Direction.Size() / 100);
        RouteMesh->AddInstance(
			FTransform(Direction.Rotation() + FRotator(90, 0, 0), Location, Scale));
    }
}
