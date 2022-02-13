// Fill out your copyright notice in the Description page of Project Settings.

#include "GridManager.h"
#include "Characters.h"

AGridManager::AGridManager() 
{
    PrimaryActorTick.bCanEverTick = false;
    SetupNodesMesh();
    SetupEdgesMesh();
}

int AGridManager::GetNeighbours(int CurrentNode, TArray<bool>& NodesMarks) 
{
    TArray<int> Neighbours;
    int NeighbourNode;
    for (int y = -1; y <= 1; y++)
        for (int x = -1; x <= 1; x++) 
        {
            int cx = CurrentNode % GridW, cy = CurrentNode / GridW;
            if (cx + x < 0 || cy + y < 0 || cx + x >= GridW || cy + y >= GridH)
                continue;

            NeighbourNode = (cx + x) + (cy + y) * GridW;
            if (GMEdges[CurrentNode][NeighbourNode] ||
                GMEdges[NeighbourNode][CurrentNode])
                continue;

            if (x != 0 && y != 0) 
            {
                if (GMEdges[cx + (cy + y) * GridW][(cx + x) + cy * GridW] ||
                    GMEdges[(cx + x) + cy * GridW][cx + (cy + y) * GridW])
                    continue;
            }

            if (NodesMarks[NeighbourNode]) continue;

            Neighbours.Add(NeighbourNode);
        }

    if (Neighbours.Num() == 0) return -1;

    return Neighbours[FMath::RandRange(0, Neighbours.Num() - 1)];
}

int AGridManager::GetNeighbours(int CurrentNode) 
{
    TArray<bool> NodesMarks;
    NodesMarks.Init(false, GNodes.Num());
    NodesMarks[CurrentNode] = true;

    return this->GetNeighbours(CurrentNode, NodesMarks);
}

FColor AGridManager::GetContrastColor(int h) 
{
	int s = 100, v = 100;

	int Hi = (h / 60) % 6;
	int vmin = (100 - s) * v / 100;
	int a = (v - vmin) * (h % 60) / 60;
	int vinc = vmin + a;
	int vdec = v - a;

	if (Hi == 0) return FColor(v, vinc, vmin);
	if (Hi == 1) return FColor(vdec, v, vmin);
	if (Hi == 2) return FColor(vmin, v, vinc);
	if (Hi == 3) return FColor(vmin, vdec, v);
	if (Hi == 4) return FColor(vinc, vmin, v);
	if (Hi == 5) return FColor(v, vmin, vdec);

	return FColor(0, 0, 0);
}

void AGridManager::CreateCharacters() 
{
    TArray<int> NodesEmpty;
    ACharacters* Characters = GetWorld()->SpawnActor<ACharacters>(GetActorLocation(), GetActorRotation());

    if (GNodes.Num() == 0)
    {
        return;
    }

    for (int i = 0; i < GNodes.Num(); i++) 
    {
        NodesEmpty.Add(i);
    }

    for (int i = 0; i < CharactersCount; i++) 
    {
        int Node = FMath::RandRange(0, NodesEmpty.Num() - 1);
        int StartNode = NodesEmpty[Node];
        int EndNode = StartNode;
        FVector Location = GNodes[StartNode];
        Characters->CharactersMesh->AddInstance(FTransform(Location));
        Characters->RouteColors.Add(GetContrastColor(i * 360 / CharactersCount));
        Characters->StartNodes.Add(StartNode);
        Characters->EndNodes.Add(EndNode);
        Characters->Routes.AddZeroed();
        Characters->CurrentNodes.AddZeroed();
        Characters->SetupRouteMesh(i);
        Characters->CharacterLocations.Add(Location);
        if (NodesEmpty.Num() - 1 > CharactersCount - i)
        { 
            NodesEmpty.RemoveAt(Node);
        }
    }

    Characters->GMParent = this;
}

void AGridManager::CreateGrid() 
{
    for (int y = 0; y < GridH; y++) 
    {
        for (int x = 0; x < GridW; x++) 
        {
            FVector NodeV = FVector(x * NodeRX * 4, y * NodeRY * 4, 0);
            NodeV.X += FMath::RandRange(-NodeRX, NodeRX);
            NodeV.Y += FMath::RandRange(-NodeRY, NodeRY);
            NodeV.Z += FMath::RandRange(-NodeRZ, NodeRZ);
            GNodes.Add(NodeV);
            NodesMesh->AddInstance(FTransform(NodeV));
        }
    }

    GMedgesCount = 0;
    GMEdges.Init({}, GridW * GridH);
    for (int x = 0; x < GridW * GridH; ++x) GMEdges[x].Init(false, GridW * GridH);

    GMEdgeColors.Init({}, GridW * GridH);
    for (int x = 0; x < GridW * GridH; ++x)
        GMEdgeColors[x].Init(FColor(255, 255, 255), GridW * GridH);

    TArray<int> NodesStack;
    TArray<bool> NodesMarks;

    for (int i = 0; i < GNodes.Num(); i++) 
    {
        NodesMarks.Add(false);
    }

    int CurrentNode = FMath::RandRange(0, GNodes.Num() - 1);
    int NeighbourNode;

    NodesStack.Push(CurrentNode);
    NodesMarks[CurrentNode] = true;

    while (NodesStack.Num() > 0) 
    {
        CurrentNode = NodesStack.Pop();

        NeighbourNode = this->GetNeighbours(CurrentNode, NodesMarks);
        if (NeighbourNode == -1) continue;

        NodesMarks[NeighbourNode] = true;

        GMEdges[CurrentNode][NeighbourNode] = true;
        GMEdges[NeighbourNode][CurrentNode] = true;
        NodesStack.Push(CurrentNode);
        NodesStack.Push(NeighbourNode);
        ++GMedgesCount;
    }

    for (int i = 0; i < 20; i++) 
    {
        CurrentNode = FMath::RandRange(0, GNodes.Num() - 1);

        NeighbourNode = this->GetNeighbours(CurrentNode);
        if (NeighbourNode == -1) continue;

        GMEdges[CurrentNode][NeighbourNode] = true;
        GMEdges[NeighbourNode][CurrentNode] = true;
        ++GMedgesCount;
    }

    for (int x = 0; x < GridW * GridH; x++) 
    {
        for (int y = 0; y < x; y++) 
        {
            if (GMEdges[x][y]) 
            {
                FVector Direction = GNodes[x] - GNodes[y];
                FVector Location = (GNodes[x] + GNodes[y]) / 2;
                FVector Scale = FVector(0.2f, 0.2f, Direction.Size() / 100);
                EdgesMesh->AddInstance(
                    FTransform(Direction.Rotation() + FRotator(90, 0, 0), Location, Scale));
            }
        }
    }
}

void AGridManager::SetupNodesMesh() 
{
    NodesMesh =
        CreateDefaultSubobject<UInstancedStaticMeshComponent>("NodesMesh");
    NodesMesh->SetupAttachment(RootComponent);

    static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMeshAsset(
        TEXT("/Engine/BasicShapes/Sphere"));
    if (SphereMeshAsset.Succeeded()) 
    {
        NodesMesh->SetStaticMesh(SphereMeshAsset.Object);
    }

    static ConstructorHelpers::FObjectFinder<UMaterialInterface>
        AssetMaterial(
            TEXT("MaterialInstanceConstant'/Game/MaterialFunctionCollection/"
                "Materials/Materials/Scanline/MI_Scanline_01.MI_Scanline_01'"));
    if (AssetMaterial.Succeeded()) 
    {
        NodesMesh->SetMaterial(0, AssetMaterial.Object);
    }

    NodesMesh->SetMobility(EComponentMobility::Static);
    NodesMesh->SetGenerateOverlapEvents(false);
    NodesMesh->CanCharacterStepUpOn = ECB_No;
    NodesMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AGridManager::SetupEdgesMesh() 
{
    EdgesMesh =
        CreateDefaultSubobject<UInstancedStaticMeshComponent>("EdgesMesh");
    EdgesMesh->SetupAttachment(RootComponent);

    static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderMeshAsset(
        TEXT("/Engine/BasicShapes/Cylinder"));
    if (CylinderMeshAsset.Succeeded()) 
    {
        EdgesMesh->SetStaticMesh(CylinderMeshAsset.Object);
    }
    
    static ConstructorHelpers::FObjectFinder<UMaterialInterface>
        AssetMaterial(
            TEXT("Material'/Game/MaterialFunctionCollection/Materials/Materials/"
                 "Emissive/M_Emissive.M_Emissive'"));
    if (AssetMaterial.Succeeded()) 
    {
        EdgesMesh->SetMaterial(0, AssetMaterial.Object);
    }

    EdgesMesh->SetMobility(EComponentMobility::Static);
    EdgesMesh->SetGenerateOverlapEvents(false);
    EdgesMesh->CanCharacterStepUpOn = ECB_No;
    EdgesMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AGridManager::Dijkstra(TArray<FVector>& Route, int StartNode, int EndNode) 
{
    TArray<int> MinRoutes;
    TArray<int> Parents;
    TArray<int> NodesStack;
    TArray<bool> NodesMarks;
    int CurrentNode;

    MinRoutes.Init(GMedgesCount * 2, GMedgesCount);
    Parents.Init(-1, GMedgesCount);
    NodesMarks.Init(false, GMedgesCount);

    NodesStack.Add(StartNode);
    MinRoutes[StartNode] = 0;

    while (NodesStack.Num() > 0) 
    {
        CurrentNode = NodesStack.Pop();
        NodesMarks[CurrentNode] = true;
        for (int x = 0; x < GridW * GridH; x++) 
        {
            if (!GMEdges[x][CurrentNode]) continue;

            if (MinRoutes[x] > MinRoutes[CurrentNode] + 1) 
            {
                MinRoutes[x] = MinRoutes[CurrentNode] + 1;
                Parents[x] = CurrentNode;
            }

            if (!NodesMarks[x]) 
            {
                NodesStack.Add(x);
            }
        }
    }

    CurrentNode = EndNode;
    Route.Empty();
    while (CurrentNode != StartNode) 
    {
        Route.Insert(GNodes[CurrentNode], 0);
        CurrentNode = Parents[CurrentNode];
    }
    Route.Insert(GNodes[CurrentNode], 0);
}

void AGridManager::BeginPlay() 
{
    Super::BeginPlay();
    this->CreateGrid();
    this->CreateCharacters();
}