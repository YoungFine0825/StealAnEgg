using UnrealBuildTool;

/*
 *
 * 客户端UI模块
 * 
 */
public class StealAnEggUI : ModuleRules
{
	public StealAnEggUI(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", 
			"CoreUObject", 
			"Engine", 
			"StealAnEgg",
			"UMG",
		});
		
		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"Slate",
			"SlateCore",
			"InputCore",
			"EnhancedInput",
		});
	}
}