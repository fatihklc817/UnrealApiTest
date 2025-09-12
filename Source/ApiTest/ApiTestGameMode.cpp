// Copyright Epic Games, Inc. All Rights Reserved.

#include "ApiTestGameMode.h"
#include "ApiTestCharacter.h"
#include "UObject/ConstructorHelpers.h"

AApiTestGameMode::AApiTestGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
