// Copyright Epic Games, Inc. All Rights Reserved.

#include "UE_SandboxGameMode.h"
#include "UE_SandboxCharacter.h"
#include "UObject/ConstructorHelpers.h"

AUE_SandboxGameMode::AUE_SandboxGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
