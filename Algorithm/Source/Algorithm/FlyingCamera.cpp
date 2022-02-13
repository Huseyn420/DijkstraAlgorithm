// Fill out your copyright notice in the Description page of Project Settings.

#include "FlyingCamera.h"

AFlyingCamera::AFlyingCamera() 
{
    PrimaryActorTick.bCanEverTick = true;

    Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
    Camera->SetupAttachment(RootComponent);
}

void AFlyingCamera::BeginPlay() 
{
    Super::BeginPlay();

    SetActorLocation(FVector(-600, 600, 1500));
    SetActorRotation(FRotator(-15, 5, 0));
}

void AFlyingCamera::Tick(float DeltaTime) 
{
    Super::Tick(DeltaTime);
    CurrentDeltaTime = DeltaTime;
}

void AFlyingCamera::SetupPlayerInputComponent(
    UInputComponent* PlayerInputComponent) 
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    InputComponent->BindAxis("VerticalAxis", this,
        &AFlyingCamera::MoveVertically);
    InputComponent->BindAxis("HorizontalAxis", this,
        &AFlyingCamera::MoveHorizontally);

    InputComponent->BindAxis("MouseX", this, &AFlyingCamera::RotateYaw);
    InputComponent->BindAxis("MouseY", this, &AFlyingCamera::RotatePitch);
}

void AFlyingCamera::MoveVertically(float Value) 
{
    FQuat Rotation = GetActorRotation().Quaternion();
    FVector Direction =
        Rotation * FVector(1, 0, 0) * Value * MoveSpeed * CurrentDeltaTime;
    SetActorLocation(GetActorLocation() + Direction);
}

void AFlyingCamera::MoveHorizontally(float Value) 
{
    FQuat Rotation = GetActorRotation().Quaternion();
    FVector Direction =
        Rotation * FVector(0, 1, 0) * Value * MoveSpeed * CurrentDeltaTime;
    SetActorLocation(GetActorLocation() + Direction);
}

void AFlyingCamera::RotateYaw(float Value) 
{
    FRotator NewRotation = GetActorRotation();
    NewRotation.Yaw = NewRotation.Yaw + Value;
    SetActorRotation(NewRotation);
}

void AFlyingCamera::RotatePitch(float Value) 
{
    FRotator NewRotation = GetActorRotation();
    NewRotation.Pitch = FMath::Clamp(NewRotation.Pitch + Value, -80.0f, 80.0f);
    SetActorRotation(NewRotation);
}