using UnrealBuildTool;
using System.Collections.Generic;

public class StealAnEggServerTarget : TargetRules
{
	public StealAnEggServerTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Server;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		
		ExtraModuleNames.AddRange(new string[]
		{
			"StealAnEgg",
		});
	}
}