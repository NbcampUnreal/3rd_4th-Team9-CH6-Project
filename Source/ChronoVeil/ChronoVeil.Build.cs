  // Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ChronoVeil : ModuleRules
{
	public ChronoVeil(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { 
			"Core",
			"CoreUObject",
			"Engine",
			// Input
			"InputCore",
			"EnhancedInput",
			// Animation
			"AnimGraphRuntime",
			// GameplayAbility
			"GameplayTags",
			"GameplayTasks",
			"GameplayAbilities",
			// ModularGameplay / GameFeatures
			"ModularGameplay",
			"GameFeatures",
			//"ModularGameplayActors", // 5,7에서는 ModularGameplay에 통합된듯.. 주석없애면 오류생김
			// CommonUI
            "CommonUI",
            // UMG
            "UMG",
            // UIExtension
            //"UIExtension",
            // Slate
            "Slate",
            "SlateCore",
			// Niagara
			"Niagara",
			"PhysicsCore",
		});

		PrivateDependencyModuleNames.AddRange(new string[] {  });

		PrivateIncludePaths.Add("ChronoVeil");

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
