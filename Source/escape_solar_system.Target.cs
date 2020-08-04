// Copyright 2020 H2S. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class escape_solar_systemTarget : TargetRules
{
	public escape_solar_systemTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V2;

		ExtraModuleNames.AddRange( new string[] { "escape_solar_system" } );
	}
}
