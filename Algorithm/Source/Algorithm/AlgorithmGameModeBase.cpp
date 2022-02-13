// Copyright Epic Games, Inc. All Rights Reserved.


#include "AlgorithmGameModeBase.h"

AAlgorithmGameModeBase::AAlgorithmGameModeBase()
{
	DefaultPawnClass = AFlyingCamera::StaticClass();
	GameSessionClass = nullptr;
	HUDClass = nullptr;
}