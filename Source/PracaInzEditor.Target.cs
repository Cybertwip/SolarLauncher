// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class PracaInzEditorTarget : TargetRules
{
	public PracaInzEditorTarget(TargetInfo Target) : base(Target)
	{
		
		Type = TargetType.Editor;

		bOverrideBuildEnvironment = true;
		ExtraModuleNames.AddRange( new string[] { "PracaInz" } );

		CppStandard = CppStandardVersion.Cpp20;
	}
}
