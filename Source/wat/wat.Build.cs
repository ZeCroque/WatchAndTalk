// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class wat : ModuleRules
{
    public wat(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "AIModule",
            "GameplayTasks",
            "NavigationSystem",
            "UMG",
            "OnlineSubsystem", 
            "OnlineSubsystemUtils"
        });

        PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore"});
        DynamicallyLoadedModuleNames.AddRange(new string[]
        {
            "OnlineSubsystemWat",
            "OnlineSubsystemNull"
        });
    }
}