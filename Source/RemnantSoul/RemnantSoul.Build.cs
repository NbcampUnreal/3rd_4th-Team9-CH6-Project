// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class RemnantSoul : ModuleRules
{
	public RemnantSoul(ReadOnlyTargetRules Target) : base(Target)
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
			"Slate",
			"DLSSBlueprint",
			"DLSS",
            "MoviePlayer",
            "MediaAssets",
			// Niagara
			"Niagara",
            "PhysicsCore",
        });

		PrivateDependencyModuleNames.AddRange(new string[] { "GameplayAbilities", "GameplayTags", "GameplayTasks" });

		PublicIncludePaths.AddRange(new string[] {
			"RemnantSoul",
			"RemnantSoul/Variant_Platforming",
			"RemnantSoul/Variant_Platforming/Animation",
			"RemnantSoul/Variant_Combat",
			"RemnantSoul/Variant_Combat/AI",
			"RemnantSoul/Variant_Combat/Animation",
			"RemnantSoul/Variant_Combat/Gameplay",
			"RemnantSoul/Variant_Combat/Interfaces",
			"RemnantSoul/Variant_Combat/UI",
			"RemnantSoul/Variant_SideScrolling",
			"RemnantSoul/Variant_SideScrolling/AI",
			"RemnantSoul/Variant_SideScrolling/Gameplay",
			"RemnantSoul/Variant_SideScrolling/Interfaces",
			"RemnantSoul/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
