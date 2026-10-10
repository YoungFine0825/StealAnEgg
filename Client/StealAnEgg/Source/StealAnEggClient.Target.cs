using UnrealBuildTool;
using System.Collections.Generic;

public class StealAnEggClientTarget : TargetRules
{
	public StealAnEggClientTarget(TargetInfo Target) : base(Target) 
	{
		Type = TargetType.Client;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		
		ExtraModuleNames.AddRange(new string[]
		{
			"StealAnEgg",
			"StealAnEggUI"
		});
	}
}