//Created by Srtyewiop

#pragma once
#include "Engine/DataTable.h"
#include "HelpStructs.generated.h"

USTRUCT(BlueprintType)
struct FNeighbors
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool UP = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool DOWN = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool LEFT = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool RIGHT = false;	
};

UENUM(BlueprintType)
enum ECollisionGeneration
{
	FromTop     UMETA(DisplayName = "From Top"),
	FromSides      UMETA(DisplayName = "From Sides"),
	None   UMETA(DisplayName = "None"),
};

UENUM(BlueprintType)
enum ETopType
{
	Planes     UMETA(DisplayName = "Planes"),
	GreedyMesh      UMETA(DisplayName = "Greedy Mesh"),
};