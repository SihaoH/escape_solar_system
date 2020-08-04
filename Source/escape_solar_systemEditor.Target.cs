// Copyright 2020 H2S. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class escape_solar_systemEditorTarget : TargetRules
{
	public escape_solar_systemEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;

		ExtraModuleNames.AddRange( new string[] { "escape_solar_system" } );
	}
}
