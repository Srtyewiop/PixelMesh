// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class PixelMesh : ModuleRules
{
	public PixelMesh(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine","InputCore", "HeadMountedDisplay", "EnhancedInput", "ProceduralMeshComponent", "MeshDescription"});
	}
}
