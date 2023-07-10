// Copyright Epic Games, Inc. All Rights Reserved.

#include "PixelMeshGameMode.h"
#include "PixelMeshCharacter.h"
#include "UObject/ConstructorHelpers.h"

APixelMeshGameMode::APixelMeshGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
