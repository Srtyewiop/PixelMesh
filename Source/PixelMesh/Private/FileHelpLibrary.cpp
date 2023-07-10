//Created by Srtyewiop

#include "FileHelpLibrary.h"
#include "JsonObjectConverter.h"
#include "ProceduralMeshComponent.h"
#include "ProceduralMeshConversion.h"
#include "Engine/StaticMesh.h"
#include "IAssetTools.h"
#include "StaticMeshAttributes.h"
#include "PhysicsEngine/BodySetup.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "MeshDescription.h"
#include "Materials/MaterialInterface.h"
#include "UObject/SavePackage.h"

bool UFileHelpLibrary::SaveAsStaticMesh(UProceduralMeshComponent* ProcMeshComp, FString TextureName, UMaterialInterface* SavedMaterial)
{
	//Mostly doing the same as button "Generate Mesh" in ProceduralMesh Component
	// Find first selected ProcMeshComp
	if (ProcMeshComp != nullptr)
	{
		const FString AssetName = FString(TEXT("SM_") + TextureName);
		const FString PathName = FString(TEXT("/Game/PixelMeshes/"));
		const FString PackageName = PathName + AssetName;
		
			FMeshDescription MeshDescription = BuildMeshDescription(ProcMeshComp);

			// If we got some valid data.
			if (MeshDescription.Polygons().Num() > 0)
			{
				// Then find/create it.
				UPackage* Package = CreatePackage(*PackageName);
				check(Package);

				// Create StaticMesh object				
				UStaticMesh* StaticMesh = NewObject<UStaticMesh>(Package, FName(*AssetName), RF_Public | RF_Standalone);
				StaticMesh->InitResources();

				StaticMesh->SetLightingGuid();

				// Add source to new StaticMesh
				FStaticMeshSourceModel& SrcModel = StaticMesh->AddSourceModel();
				SrcModel.BuildSettings.bRecomputeNormals = false;
				SrcModel.BuildSettings.bRecomputeTangents = false;
				SrcModel.BuildSettings.bRemoveDegenerates = false;
				SrcModel.BuildSettings.bUseHighPrecisionTangentBasis = false;
				SrcModel.BuildSettings.bUseFullPrecisionUVs = false;
				SrcModel.BuildSettings.bGenerateLightmapUVs = true;
				SrcModel.BuildSettings.SrcLightmapIndex = 0;
				SrcModel.BuildSettings.DstLightmapIndex = 1;
				StaticMesh->CreateMeshDescription(0, MoveTemp(MeshDescription));
				StaticMesh->CommitMeshDescription(0);

				//// SIMPLE COLLISION
				if (!ProcMeshComp->bUseComplexAsSimpleCollision )
				{
					StaticMesh->CreateBodySetup();
					UBodySetup* NewBodySetup = StaticMesh->GetBodySetup();
					NewBodySetup->BodySetupGuid = FGuid::NewGuid();
					NewBodySetup->AggGeom.ConvexElems = ProcMeshComp->ProcMeshBodySetup->AggGeom.ConvexElems;
					NewBodySetup->bGenerateMirroredCollision = false;
					NewBodySetup->bDoubleSidedGeometry = true;
					NewBodySetup->CollisionTraceFlag = CTF_UseDefault;
					NewBodySetup->CreatePhysicsMeshes();
				}

				//// MATERIALS
				TSet<UMaterialInterface*> UniqueMaterials;
				const int32 NumSections = ProcMeshComp->GetNumSections();
				for (int32 SectionIdx = 0; SectionIdx < NumSections; SectionIdx++)
				{
					/*FProcMeshSection *ProcSection =
						ProcMeshComp->GetProcMeshSection(SectionIdx);*/
					UMaterialInterface *Material = ProcMeshComp->GetMaterial(SectionIdx);
					UniqueMaterials.Add(Material);
				}
				// Copy materials to new mesh
				for (auto* Material : UniqueMaterials)
				{
					StaticMesh->GetStaticMaterials().Add(FStaticMaterial(SavedMaterial));
				}

				//Set the Imported version before calling the build
				StaticMesh->ImportVersion = EImportStaticMeshVersion::LastVersion;

				// Build mesh from source
				StaticMesh->Build(false);
				StaticMesh->PostEditChange();
				// Notify asset registry of new asset
				FAssetRegistryModule::AssetCreated(StaticMesh);
				return true;
		}
	}

	return false;
}
