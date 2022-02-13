// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Pawn.h"
#include "FlyingCamera.generated.h"

UCLASS()
class ALGORITHM_API AFlyingCamera : public APawn
{
	GENERATED_BODY()

public:
	AFlyingCamera();

	UPROPERTY()
		UCameraComponent* Camera;

	void MoveVertically(float Value);
	void MoveHorizontally(float Value);

	void RotateYaw(float Value);
	void RotatePitch(float Value);

	UPROPERTY(EditAnywhere, Category = "Camera")
		float MoveSpeed = 5000.0f;

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	float CurrentDeltaTime = 0.0f;
};
