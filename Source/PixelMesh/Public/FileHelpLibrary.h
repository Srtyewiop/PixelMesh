//Created by Srtyewiop

#pragma once

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FileHelpLibrary.generated.h"

/**
 * 
 */
UCLASS()
class PIXELMESH_API UFileHelpLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	UFUNCTION(BlueprintCallable)
	static bool SaveAsStaticMesh(UProceduralMeshComponent* ProcMeshComp, FString TextureName, UMaterialInterface* SavedMaterial);
};
