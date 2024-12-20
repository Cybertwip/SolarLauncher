// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class PracaInzTarget : TargetRules
{
	public PracaInzTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;

		bOverrideBuildEnvironment = true;
		ExtraModuleNames.AddRange( new string[] { "PracaInz" } );
		CppStandard = CppStandardVersion.Cpp20;
	}
}
