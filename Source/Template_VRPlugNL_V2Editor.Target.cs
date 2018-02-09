// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class Template_VRPlugNL_V2EditorTarget : TargetRules
{
	public Template_VRPlugNL_V2EditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;

		ExtraModuleNames.AddRange( new string[] { "Template_VRPlugNL_V2" } );
	}
}
