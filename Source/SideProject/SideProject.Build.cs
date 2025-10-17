// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SideProject : ModuleRules
{
	public SideProject(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"SideProject",
			"SideProject/Variant_Platforming",
			"SideProject/Variant_Platforming/Animation",
			"SideProject/Variant_Combat",
			"SideProject/Variant_Combat/AI",
			"SideProject/Variant_Combat/Animation",
			"SideProject/Variant_Combat/Gameplay",
			"SideProject/Variant_Combat/Interfaces",
			"SideProject/Variant_Combat/UI",
			"SideProject/Variant_SideScrolling",
			"SideProject/Variant_SideScrolling/AI",
			"SideProject/Variant_SideScrolling/Gameplay",
			"SideProject/Variant_SideScrolling/Interfaces",
			"SideProject/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
