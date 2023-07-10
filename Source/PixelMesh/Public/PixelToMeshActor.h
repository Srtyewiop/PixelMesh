//Created by Srtyewiop

#pragma once

#include "CoreMinimal.h"
#include "HelpStructs.h"
#include "MeshAttributes.h"
#include "ProceduralMeshComponent.h"
#include "GameFramework/Actor.h"
#include "PixelToMeshActor.generated.h"

UCLASS()
class PIXELMESH_API APixelToMeshActor : public AActor
{
	GENERATED_BODY()
	
public:	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation Setup")
	UTexture2D* GenerationTexture;

	UPROPERTY(EditAnywhere, Category = "Generation Setup")
	UMaterialInterface* Material;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Generation Transform")
	FVector MeshScale = FVector(100.f, 100.f, 100.f);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Generation Setup")
	bool bGenerateTop = true;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Generation Setup")
	bool bGenerateSides = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Generation Setup")
	bool bGenerateBottom = true;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Generation Transform")
	FRotator MeshRotation = FRotator(0, 0 ,0);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Generation Transform")
	FVector MeshOffset = FVector(0, 0 ,0);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation Setup")
	TEnumAsByte<ETopType> CreateDetailedTopAndBottom = Planes;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation Collision")
	TEnumAsByte<ECollisionGeneration> CollisionType = None;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	UProceduralMeshComponent* Mesh;

	UFUNCTION(BlueprintCallable)
	void CreateMesh();
	
	APixelToMeshActor();

private:
	TArray<FVector> Vertices;
	TArray<FVector> Collision;
	TArray<int> Triangles;
	TArray<FVector2D> UV;
	TArray<FVector> Normals;
	TMap<FVector2D, FNeighbors> PixelData;
	int32 XSize = 0;
	int32 YSize = 0;
	
	void ClearInfo();
	void GetPixelDataAndFillMeshInfo();
	void GreedyMeshSideGeneration();
	void GreedyMeshTopGeneration(TMap<FVector2D, FNeighbors> &DataCopy);
};



