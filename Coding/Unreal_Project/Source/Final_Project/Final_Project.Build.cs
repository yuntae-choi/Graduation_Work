// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class Final_Project : ModuleRules
{
	public Final_Project(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PrivateIncludePaths.AddRange(new string[] {
				"../Plugins/HoudiniNiagara/Source/HoudiniNiagara/Private", "../Plugins/HoudiniNiagara/Source/HoudiniNiagaraEditor/Private"
		});

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "UMG", "GameplayTasks", "Niagara", "HoudiniNiagara" });

		PrivateDependencyModuleNames.AddRange(new string[] {  });

        // Uncomment if you are using Slate UI
        PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }
}
